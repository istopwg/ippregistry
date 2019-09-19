/*
 * Program to convert keyword and enum registrations to strings.
 *
 * Usage:
 *
 *    ./regtostrings [--code] [--language ll] [--po] filename.xml >filename.ext
 *
 * Copyright © 2018-2019 by The IEEE-ISTO Printer Working Group.
 * Copyright © 2008-2019 by Michael R Sweet
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 */

#include <mxml.h>
#include <limits.h>
#include <ctype.h>
#include <cups/cups.h>


/* Common IPP registry stuff */
#include "ipp-registry.h"

/* "Canned" localizations */
#include "ipp-strings.h"


/*
 * Output formats...
 */

#define FORMAT_STRINGS	0
#define FORMAT_PO	1
#define FORMAT_CODE	2


/*
 * Local globals...
 */

static const char	*language = NULL;
static http_t		*translation_service = NULL;
static size_t           alloc_strings = 0,
                        num_strings = 0;
static ipp_loc_t        *strings = NULL;


/*
 * Local functions...
 */

static int	add_string(const char *locid, const char *str);
static int	add_strings(mxml_node_t *registry_node);
static int	check_string(const char *locid);
static const char *decode_string(const char *data, char term, char *buffer, size_t bufsize);
static char	*encode_string(const char *s, char *bufptr, char *bufend);
static int	json_decode(const char *data, cups_option_t **vars);
static char	*json_encode(int num_vars, cups_option_t *vars);
static char	*translate_string(const char *s, char *buffer, size_t bufsize);
static int	usage(void);
static void	write_strings(int format);


/*
 * 'main()' - Write a strings file for all keywords and enums in the registry.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line arguments */
{
  int           i;                      /* Looping var */
  const char	*xmlin = NULL;		/* XML registration input file */
  mxml_node_t	*xml,			/* XML registration file top node */
		*registry_node;		/* Current registry node */
  FILE		*xmlfile;		/* XML registration file pointer */
  int           format = FORMAT_STRINGS;/* Output format */


 /*
  * Parse command-line...
  */

  for (i = 1; i < argc; i ++)
    if (!strcmp(argv[i], "--code"))
      format = FORMAT_CODE;
    else if (!strcmp(argv[i], "--po"))
      format = FORMAT_PO;
    else if (argv[i][0] == '-')
    {
      printf("Unknown option \'%s\'.\n", argv[i]);
      return (usage());
    }
    else
      xmlin = argv[i];

  if (!xmlin)
    return (usage());

 /*
  * Load the XML registration file...
  */

  mxmlSetWrapMargin(INT_MAX);

  if ((xmlfile = fopen(xmlin, "rb")) == NULL)
  {
    perror(xmlin);
    return (1);
  }

  xml = mxmlLoadFile(NULL, xmlfile, ipp_load_cb);
  fclose(xmlfile);

  if (!xml)
  {
    fputs("Bad XML registration file.\n", stderr);
    return (1);
  }

 /*
  * Output keyword and enum strings...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) != NULL)
  {
    if (!add_strings(registry_node))
      return (1);
  }

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ENUMS, MXML_DESCEND)) != NULL)
  {
    if (!add_strings(registry_node))
      return (1);
  }

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_KEYWORDS, MXML_DESCEND)) != NULL)
  {
    if (!add_strings(registry_node))
      return (1);
  }

  write_strings(format);

  return (0);
}


/*
 * 'add_string()' - Add a single localization string.
 */

static int                              /* O - 1 on success, 0 on failure */
add_string(const char *locid,           /* I - Localization ID */
           const char *str)             /* I - Localized string */
{
  ipp_loc_t     *temp;                  /* Temporary pointer */


  if (!str)
    return (1);

  if (num_strings > 0 && check_string(locid))
    return (1);

  if (num_strings >= alloc_strings)
  {
    alloc_strings += 1024;
    if ((temp = realloc(strings, alloc_strings * sizeof(ipp_loc_t))) == NULL)
    {
      perror("Unable to allocate memory for strings");
      return (0);
    }

    strings = temp;
  }

  temp = strings + num_strings;
  num_strings ++;

  temp->id  = strdup(locid);
  temp->str = strdup(str);

  if (temp->id == NULL || temp->str == NULL)
    return (0);

  if (num_strings > 1)
    qsort(strings, num_strings, sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc);

  return (1);
}


/*
 * 'add_strings()' - Add all strings for the given registry.
 */

static int                              /* O - 1 on success, 0 on failure */
add_strings(mxml_node_t *registry_node) /* I - Registry */
{
  mxml_node_t	*record_node,		/* Current record node */
		*attribute_node,	/* Attribute for localization */
		*collection_node,	/* Collection (for attributes) */
		*member_node,		/* Member attribute node (for attributes */
		*submember_node,	/* Sub-member attribute node (for attributes */
		*name_node,		/* Keyword string to be localized */
		*syntax_node,		/* Syntax string (for attributes) */
		*value_node;		/* Value for localization */
  char		locid[256],             /* Localization ID string */
                localized[1024];	/* Localized string */
  const char	*last_attribute = NULL;	/* Last attribute written */


  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record", NULL,
                                     NULL, MXML_NO_DESCEND))
  {
    attribute_node  = mxmlFindElement(record_node, record_node, "attribute",
                                      NULL, NULL, MXML_DESCEND_FIRST);
    collection_node = mxmlFindElement(record_node, record_node, "collection",
                                      NULL, NULL, MXML_DESCEND_FIRST);
    member_node     = mxmlFindElement(record_node, record_node,
                                      "member_attribute", NULL, NULL,
                                      MXML_DESCEND_FIRST);
    submember_node  = mxmlFindElement(record_node, record_node,
                                      "sub-member_attribute", NULL, NULL,
                                      MXML_DESCEND_FIRST);
    name_node       = mxmlFindElement(record_node, record_node, "name",
                                      NULL, NULL, MXML_DESCEND_FIRST);
    syntax_node     = mxmlFindElement(record_node, record_node, "syntax",
                                      NULL, NULL, MXML_DESCEND_FIRST);
    value_node      = mxmlFindElement(record_node, record_node, "value",
                                      NULL, NULL, MXML_DESCEND_FIRST);

    if (collection_node && name_node && syntax_node)
    {
     /*
      * See if this is an attribute we want to localize...
      */

      const char *collection = mxmlGetOpaque(collection_node),
      			*name = mxmlGetOpaque(submember_node ? submember_node : member_node ? member_node : name_node),
			*syntax = mxmlGetOpaque(syntax_node);

      if (collection && !strcmp(collection, "Job Template") &&
          name && syntax &&
          !strstr(name, "-default") &&
          !strstr(name, "-ready") &&
          !strstr(name, "-supported") &&
          strncmp(name, "access-", 7) &&
          strcmp(name, "attributes-charset") &&
          strcmp(name, "attributes-natural-language") &&
          strncmp(name, "compression", 11) &&
          strcmp(name, "current-page-order") &&
          strncmp(name, "destination-", 12) &&
          strcmp(name, "document-access") &&
          strcmp(name, "document-access-error") &&
          strcmp(name, "document-charset") &&
          strncmp(name, "document-d", 10) &&
          strncmp(name, "document-format", 15) &&
          strncmp(name, "document-m", 10) &&
          strncmp(name, "document-n", 10) &&
          strncmp(name, "document-p", 10) &&
          strncmp(name, "document-source", 15) &&
          strncmp(name, "fetch-", 6) &&
          strcmp(name, "first-index") &&
          strcmp(name, "identify-actions") &&
          strcmp(name, "images-completed") &&
          strcmp(name, "ipp-attribute-fidelity") &&
          strcmp(name, "job-authorization-uri") &&
          strcmp(name, "job-finishings-col") &&
          strcmp(name, "job-id") &&
          strcmp(name, "job-ids") &&
          strncmp(name, "job-impressions", 15) &&
          strncmp(name, "job-k-octets", 12) &&
          strncmp(name, "job-media-sheets", 16) &&
          strcmp(name, "job-message-from-operator") &&
          strcmp(name, "job-message-to-operator") &&
          strcmp(name, "job-pages-per-set") &&
          strncmp(name, "job-password", 12) &&
          strcmp(name, "job-uri") &&
          strcmp(name, "last-document") &&
          strcmp(name, "limit") &&
          strcmp(name, "material-key") &&
          strcmp(name, "media-key") &&
          strcmp(name, "media-order-count") &&
          strcmp(name, "message") &&
          strcmp(name, "my-jobs") &&
          strcmp(name, "notify-events") &&
          strcmp(name, "object-uuid") &&
          strcmp(name, "original-requesting-user-name") &&
          strncmp(name, "output-device", 13) &&
          strcmp(name, "overrides") &&
          strncmp(name, "pdl-init", 8) &&
          strcmp(name, "preferred-attributes") &&
          strcmp(name, "printer-uri") &&
          strcmp(name, "profile-uri-actual") &&
          strncmp(name, "requesting-user-", 16) &&
          strcmp(name, "which-jobs") &&
          strncmp(name, "x-image", 7) &&
          strncmp(name, "x-side", 6) &&
          strncmp(name, "y-image", 7) &&
          strncmp(name, "y-side", 6) &&
          !strstr(name, "(deprecated)") &&
          !strstr(name, "(extension)") &&
          !strstr(name, "(obsolete)") &&
          name[0] != '<')
      {
       /*
        * Job template or operation attribute that isn't otherwise localized.
        */

        const char *s = ipp_get_localized("", name, name, localized, sizeof(localized));

        if (s && !add_string(name, s))
          return (0);
      }

      continue;
    }

    if (!name_node)
      name_node = value_node;

    if (attribute_node && name_node && value_node)
    {
      const char *attribute = mxmlGetOpaque(attribute_node),
      			*name = mxmlGetOpaque(name_node),
			*value = mxmlGetOpaque(value_node);

      if (!strncmp(attribute, "compression", 11) ||
          !strcmp(attribute, "cover-back-supported") ||
          !strcmp(attribute, "cover-front-supported") ||
          !strcmp(attribute, "cover-sheet-info-supported") ||
          !strcmp(attribute, "current-page-order") ||
          !strcmp(attribute, "document-digital-signature") ||
          !strcmp(attribute, "document-format-details-supported") ||
          !strcmp(attribute, "document-format-varying-attributes") ||
          !strncmp(attribute, "document-privacy-", 17) ||
          !strcmp(attribute, "feed-orientation") ||
          !strcmp(attribute, "finishings-col-supported") ||
          !strcmp(attribute, "identify-actions") ||
          !strcmp(attribute, "ipp-features-supported") ||
          !strcmp(attribute, "ipp-versions-supported") ||
          !strcmp(attribute, "job-account-type") ||
          !strcmp(attribute, "job-accounting-sheets-type") ||
          !strcmp(attribute, "job-destination-spooling-supported") ||
          !strcmp(attribute, "job-mandatory-attributes") ||
          !strncmp(attribute, "job-password", 12) ||
          !strncmp(attribute, "job-privacy-", 12) ||
          !strcmp(attribute, "job-save-disposition-supported") ||
          !strcmp(attribute, "job-spooling-supported") ||
          !strcmp(attribute, "jpeg-features-supported") ||
          !strcmp(attribute, "media-col-supported") ||
          !strcmp(attribute, "media-key") ||
          !strcmp(attribute, "media-source-feed-direction") ||
          !strcmp(attribute, "media-source-feed-orientation") ||
          !strcmp(attribute, "multiple-document-handling") ||
          !strcmp(attribute, "multiple-object-handling") ||
          !strcmp(attribute, "multiple-operation-time-out-action") ||
          !strncmp(attribute, "notify-", 7) ||
          !strncmp(attribute, "output-device", 13) ||
          !strcmp(attribute, "page-delivery") ||
          !strcmp(attribute, "page-order-received") ||
          !strcmp(attribute, "pclm-raster-back-side") ||
          !strcmp(attribute, "pdf-features-supported") ||
          !strcmp(attribute, "pdf-versions-supported") ||
          !strncmp(attribute, "pdl-init", 8) ||
          !strcmp(attribute, "pdl-override-supported") ||
          !strcmp(attribute, "platform-shape") ||
          !strcmp(attribute, "presentation-direction-number-up") ||
          !strcmp(attribute, "printer-kind") ||
          !strncmp(attribute, "printer-privacy-", 16) ||
          !strcmp(attribute, "proof-print-supported") ||
          !strcmp(attribute, "pwg-raster-document-sheet-back") ||
          !strcmp(attribute, "pwg-raster-document-type-supported") ||
          !strcmp(attribute, "requested-attributes") ||
          !strcmp(attribute, "save-disposition") ||
          !strcmp(attribute, "save-info-supported") ||
          !strcmp(attribute, "stitching-supported") ||
          !strncmp(attribute, "subscription-privacy-", 21) ||
          !strcmp(attribute, "uri-authentication-supported") ||
          !strcmp(attribute, "uri-security-supported") ||
          !strcmp(attribute, "which-jobs") ||
          !strncmp(attribute, "x-image", 7) ||
          !strncmp(attribute, "x-side", 6) ||
          !strcmp(attribute, "xri-authentication-supported") ||
          !strcmp(attribute, "xri-security-supported") ||
          !strncmp(attribute, "y-image", 7) ||
          !strncmp(attribute, "y-side", 6) ||
          strstr(attribute, "(deprecated)") ||
          strstr(attribute, "(extension)") ||
          strstr(attribute, "(obsolete)"))
        continue;

      if ((!last_attribute || strcmp(attribute, last_attribute)) &&
          !strstr(attribute, "-default") && !strstr(attribute, "-supported") &&
          !strstr(attribute, "-ready"))
      {
        if (!add_string(attribute, ipp_get_localized("", attribute, attribute, localized, sizeof(localized))))
          return (0);

        last_attribute = attribute;
      }

      if (value[0] != '<' && value[0] != ' ' && strcmp(name, "Unassigned") &&
          strcmp(attribute, "operations-supported") &&
          (strcmp(attribute, "media") || strchr(value, '_') != NULL) &&
          !strstr(name, "(deprecated)") && !strstr(name, "(obsolete)"))
      {
        snprintf(locid, sizeof(locid), "%s.%s", attribute, value);

        if (check_string(locid))
        {
          fprintf(stderr, "ERROR: Redefined \"%s\".\n", locid);
          return (0);
        }

        if (!add_string(locid, ipp_get_localized(attribute, name, value, localized, sizeof(localized))))
          return (0);
      }
    }
  }

  return (1);
}


/*
 * 'check_string()' - Check for an existing string...
 */

static int                              /* O - 1 if present, 0 if not */
check_string(const char *locid)         /* I - String */
{
  ipp_loc_t     key;                    /* Search key */


  if (num_strings == 0)
    return (0);

  key.id = locid;

  return (bsearch(&key, strings, num_strings, sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc) != NULL);
}


/*
 * 'decode_string()' - Decode a URL-encoded string.
 */

static const char *                     /* O - New pointer into string */
decode_string(const char *data,         /* I - Pointer into data string */
              char       term,          /* I - Terminating character */
              char       *buffer,       /* I - String buffer */
              size_t     bufsize)       /* I - Size of string buffer */
{
  int	ch;				/* Current character */
  char	*ptr,				/* Pointer info buffer */
	*end;				/* Pointer to end of buffer */


  for (ptr = buffer, end = buffer + bufsize - 1; *data && *data != term; data ++)
  {
    if ((ch = *data) == '\\')
    {
     /*
      * "\something" is an escaped character...
      */

      data ++;

      switch (*data)
      {
        case '\\' :
            ch = '\\';
            break;
        case '\"' :
            ch = '\"';
            break;
        case '/' :
            ch = '/';
            break;
        case 'b' :
            ch = 0x08;
            break;
        case 'f' :
            ch = 0x0c;
            break;
        case 'n' :
            ch = 0x0a;
            break;
        case 'r' :
            ch = 0x0d;
            break;
        case 't' :
            ch = 0x09;
            break;
        case 'u' :
            data ++;
            if (isxdigit(data[0] & 255) && isxdigit(data[1] & 255) && isxdigit(data[2] & 255) && isxdigit(data[3] & 255))
            {
              if (isalpha(data[0]))
                ch = (tolower(data[0]) - 'a' + 10) << 12;
	      else
	        ch = (data[0] - '0') << 12;

              if (isalpha(data[1]))
                ch |= (tolower(data[1]) - 'a' + 10) << 8;
	      else
	        ch |= (data[1] - '0') << 8;

              if (isalpha(data[2]))
                ch |= (tolower(data[2]) - 'a' + 10) << 4;
	      else
	        ch |= (data[2] - '0') << 4;

              if (isalpha(data[3]))
                ch |= tolower(data[3]) - 'a' + 10;
	      else
	        ch |= data[3] - '0';
              break;
            }

            /* Fall through to default on error */
	default :
	    *buffer = '\0';
	    return (NULL);
      }
    }

    if (ch && ptr < end)
      *ptr++ = (char)ch;
  }

  *ptr = '\0';

  return (data);
}


/*
 * 'encode_string()' - URL-encode a string.
 *
 * The new buffer pointer can go past bufend, but we don't write past there...
 */

static char *                           /* O - New buffer pointer */
encode_string(const char *s,            /* I - String to encode */
              char       *bufptr,       /* I - Pointer into buffer */
              char       *bufend)       /* I - End of buffer */
{
  if (bufptr < bufend)
    *bufptr++ = '\"';

  while (*s && bufptr < bufend)
  {
    if (*s == '\b')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = 'b';
    }
    else if (*s == '\f')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = 'f';
    }
    else if (*s == '\n')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = 'n';
    }
    else if (*s == '\r')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = 'r';
    }
    else if (*s == '\t')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = 't';
    }
    else if (*s == '\\')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = '\\';
    }
    else if (*s == '\"')
    {
      *bufptr++ = '\\';
      if (bufptr < bufend)
        *bufptr++ = '\"';
    }
    else if (*s >= ' ')
      *bufptr++ = *s;

    s ++;
  }

  if (bufptr < bufend)
    *bufptr++ = '\"';

  *bufptr = '\0';

  return (bufptr);
}


/*
 * 'json_decode()' - Decode an application/json object.
 */

static int				/* O - Number of JSON member variables or 0 on error */
json_decode(const char    *data,	/* I - JSON data */
            cups_option_t **vars)	/* O - JSON member variables or @code NULL@ on error */
{
  int	num_vars = 0;			/* Number of form variables */
  char	name[1024],			/* Variable name */
	value[4096],			/* Variable value */
	*ptr,				/* Pointer into value */
	*end;				/* End of value */


 /*
  * Scan the string for "name":"value" pairs, unescaping values as needed.
  */

  *vars = NULL;

  if (!data || *data != '{')
    return (0);

  data ++;

  while (*data)
  {
   /*
    * Skip leading whitespace/commas...
    */

    while (*data && (isspace(*data & 255) || *data == ','))
      data ++;

   /*
    * Get the member variable name, unless we have the end of the object...
    */

    if (*data == '}')
      break;
    else if (*data != '\"')
      goto decode_error;

    data = decode_string(data + 1, '\"', name, sizeof(name));

    if (*data != '\"')
      goto decode_error;

    data ++;

    if (*data != ':')
      goto decode_error;

    data ++;

    if (*data == '\"')
    {
     /*
      * Quoted string value...
      */

      data = decode_string(data + 1, '\"', value, sizeof(value));

      if (*data != '\"')
	goto decode_error;

      data ++;
    }
    else if (*data == '[')
    {
     /*
      * Array value...
      */

      ptr    = value;
      end    = value + sizeof(value) - 1;
      *ptr++ = '[';

      for (data ++; *data && *data != ']';)
      {
        if (*data == ',')
        {
          if (ptr < end)
            *ptr++ = *data++;
	  else
	    goto decode_error;
        }
        else if (*data == '\"')
        {
         /*
          * Quoted string value...
          */

          do
          {
	    if (*data == '\\')
	    {
	      if (ptr < end)
		*ptr++ = *data++;
	      else
		goto decode_error;

              if (!strchr("\\\"/bfnrtu", *data))
                goto decode_error;

	      if (*data == 'u')
	      {
		if (ptr < (end - 5))
		  *ptr++ = *data++;
		else
		  goto decode_error;

	        if (isxdigit(data[0] & 255) && isxdigit(data[1] & 255) && isxdigit(data[2] & 255) && isxdigit(data[3] & 255))
	        {
		  *ptr++ = *data++;
		  *ptr++ = *data++;
		  *ptr++ = *data++;
		  /* 4th character is copied below */
	        }
	        else
	          goto decode_error;
	      }
	    }

	    if (ptr < end)
	      *ptr++ = *data++;
	    else
	      goto decode_error;
	  }
	  while (*data && *data != '\"');

	  if (*data == '\"')
	  {
	    if (ptr < end)
	      *ptr++ = *data++;
	    else
	      goto decode_error;
	  }
        }
        else if (*data == '{' || *data == '[')
        {
         /*
          * Unsupported nested array or object value...
          */

	  goto decode_error;
	}
	else
	{
	 /*
	  * Number, boolean, etc.
	  */

          while (*data && *data != ',' && !isspace(*data & 255))
          {
            if (ptr < end)
              *ptr++ = *data++;
	    else
	      goto decode_error;
	  }
	}
      }

      if (*data != ']' || ptr >= end)
        goto decode_error;

      *ptr++ = *data++;
      *ptr   = '\0';

      data ++;
    }
    else if (*data == '{')
    {
     /*
      * Unsupported object value...
      */

      goto decode_error;
    }
    else
    {
     /*
      * Number, boolean, etc.
      */

      for (ptr = value; *data && *data != ',' && !isspace(*data & 255); data ++)
        if (ptr < (value + sizeof(value) - 1))
          *ptr++ = *data;

      *ptr = '\0';
    }

   /*
    * Add the variable...
    */

    num_vars = cupsAddOption(name, value, num_vars, vars);
  }

  return (num_vars);

 /*
  * If we get here there was an error in the form data...
  */

  decode_error:

  cupsFreeOptions(num_vars, *vars);

  *vars = NULL;

  return (0);
}


/*
 * 'json_encode()' - Encode variables as a JSON object.
 *
 * The caller is responsible for calling @code free@ on the returned string.
 */

static char *				/* O - Encoded data or @code NULL@ on error */
json_encode(
    int           num_vars,		/* I - Number of JSON member variables */
    cups_option_t *vars)		/* I - JSON member variables */
{
  char		buffer[65536],		/* Temporary buffer */
		*bufptr = buffer,	/* Current position in buffer */
		*bufend = buffer + sizeof(buffer) - 2;
					/* End of buffer */
  const char	*valptr;		/* Pointer into value */
  int		is_number;		/* Is the value a number? */


  *bufptr++ = '{';
  *bufend   = '\0';

  while (num_vars > 0)
  {
    bufptr = encode_string(vars->name, bufptr, bufend);

    if (bufptr >= bufend)
      return (NULL);

    *bufptr++ = ':';

    if (vars->value[0] == '[')
    {
     /*
      * Array value, already encoded...
      */

      strncpy(bufptr, vars->value, bufend - bufptr);
      bufptr += strlen(bufptr);
    }
    else
    {
      is_number = 0;

      if (vars->value[0] == '-' || isdigit(vars->value[0] & 255))
      {
	for (valptr = vars->value + 1; *valptr && (isdigit(*valptr & 255) || *valptr == '.'); valptr ++);

	if (*valptr == 'e' || *valptr == 'E' || !*valptr)
	  is_number = 1;
      }

      if (is_number)
      {
       /*
	* Copy number literal...
	*/

	for (valptr = vars->value; *valptr; valptr ++)
	{
	  if (bufptr < bufend)
	    *bufptr++ = *valptr;
	}
      }
      else
      {
       /*
	* Copy string value...
	*/

	bufptr = encode_string(vars->value, bufptr, bufend);
      }
    }

    num_vars --;
    vars ++;

    if (num_vars > 0)
    {
      if (bufptr >= bufend)
        return (NULL);

      *bufptr++ = ',';
    }
  }

  *bufptr++ = '}';
  *bufptr   = '\0';

  return (strdup(buffer));
}


/*
 * 'translate_string()' - Translate a string using the Google translation API.
 */

static char *				/* O - Translated string */
translate_string(const char *s,		/* I - String to translate */
                 char       *buffer,	/* I - Buffer */
                 size_t     bufsize)	/* I - Size of buffer */
{
  int		count;			/* */
  int		num_json = 0;		/* Number of variables */
  cups_option_t	*json = NULL;		/* Variables */
  char		*data;			/* Request/response data */
  size_t	content_length;		/* Content-Length */


  strncpy(buffer, s, bufsize - 1);
  buffer[bufsize - 1] = '\0';

  if (!language)
    return (buffer);

  if (!translation_service)
  {
    translation_service = httpConnect2("translation.googleapis.com", 443, NULL, 30000, NULL);

    if (!translation_service)
    {
      fprintf(stderr, "Unable to connect to Google translation service: %s\n", cupsLastErrorString());

      language = NULL;

      return (buffer);
    }
  }

  if (translation_service)
  {
    num_json = cupsAddOption("format", "text", num_json, &json);
//    num_json = cupsAddOption("key", "????", num_json, &json);
    num_json = cupsAddOption("q", s, num_json, &json);
    num_json = cupsAddOption("source", "en", num_json, &json);
    num_json = cupsAddOption("target", language, num_json, &json);

    data = json_encode(num_json, json);

    cupsFreeOptions(num_json, json);

    httpClearFields(translation_service);
    httpSetField(translation_service, HTTP_FIELD_CONTENT_TYPE, "application/json");
    httpSetLength2(translation_service, content_length = strlen(data));

    if (httpPost(translation_service, "/language/translate/v2"))
    {
      if (httpReconnect2(translation_service, 30000, NULL))
      {
        free(data);
        return (buffer);
      }

      if (httpPost(translation_service, "/language/translate/v2"))
      {
        fprintf(stderr, "Unable to POST to Google translation service: %s\n", cupsLastErrorString());

        free(data);
        return (buffer);
      }
    }

    if (httpWrite2(translation_service, data, content_length) < (ssize_t)content_length)
    {
      
  }

  return (buffer);
}


/*
 * 'usage()' - Show program usage.
 */

static int				/* O - Exit status */
usage(void)
{
  puts("\nUsage: ./regtostrings [--code] [--po] filename.xml >filename.{c,po,strings}\n");
  return (1);
}


/*
 * 'write_strings()' - Write strings for registered enums and keywords.
 */

static void
write_strings(int format)               /* I - Output format - strings or PO */
{
  size_t        count;                  /* Number of records left */
  ipp_loc_t     *loc;                   /* Current localization record */
  char		buffer[8192];		/* Translation buffer */


  qsort(strings, num_strings, sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc);

  switch (format)
  {
    case FORMAT_CODE :
	for (loc = strings, count = num_strings; count > 0; loc ++, count --)
	{
	  printf("/* TRANSLATORS: %s */\n", loc->str);
	  printf("_(\"%s\");\n", loc->id);
	}
	break;

    case FORMAT_PO :
	for (loc = strings, count = num_strings; count > 0; loc ++, count --)
	{
	  printf("#. TRANSLATORS: %s\n", loc->str);
	  printf("msgid \"%s\"\n", loc->id);
	  if (language)
	    printf("msgstr \"%s\"\n", translate_string(loc->str, buffer, sizeof(buffer)));
	  else
	    printf("msgstr \"%s\"\n", loc->str);
	}
	break;

    default :
    case FORMAT_STRINGS :
	for (loc = strings, count = num_strings; count > 0; loc ++, count --)
	  printf("\"%s\" = \"%s\";\n", loc->id, loc->str);
	break;
  }
}
