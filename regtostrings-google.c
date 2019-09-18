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

static size_t           alloc_strings = 0,
                        num_strings = 0;
static ipp_loc_t        *strings = NULL;


/*
 * Local functions...
 */

static int	add_string(const char *locid, const char *str);
static int	add_strings(mxml_node_t *registry_node);
static int	check_string(const char *locid);
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

  temp->id = strdup(locid);
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

      if (collection && name && syntax &&
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
          name[0] != '<' &&
	  (!strcmp(collection, "Job Template") ||
           !strcmp(collection, "Operation") ||
           !strcmp(collection, "Subscription Template")))
      {
       /*
        * Job template or operation attribute that isn't otherwise localized.
        */

        if (!add_string(name, ipp_get_localized("", name, name, localized, sizeof(localized))))
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
          !strcmp(attribute, "current-page-order") ||
          !strcmp(attribute, "document-digital-signature") ||
          !strcmp(attribute, "document-format-details-supported") ||
          !strcmp(attribute, "document-format-varying-attributes") ||
          !strcmp(attribute, "finishings-col-supported") ||
          !strcmp(attribute, "identify-actions") ||
          !strcmp(attribute, "ipp-features-supported") ||
          !strcmp(attribute, "ipp-versions-supported") ||
          !strcmp(attribute, "job-destination-spooling-supported") ||
          !strcmp(attribute, "job-mandatory-attributes") ||
          !strncmp(attribute, "job-password", 12) ||
          !strcmp(attribute, "job-save-disposition-supported") ||
          !strcmp(attribute, "job-spooling-supported") ||
          !strcmp(attribute, "jpeg-features-supported") ||
          !strcmp(attribute, "media-col-supported") ||
          !strcmp(attribute, "media-key") ||
          !strcmp(attribute, "media-source-feed-direction") ||
          !strcmp(attribute, "media-source-feed-orientation") ||
          !strncmp(attribute, "output-device", 13) ||
          !strcmp(attribute, "pdf-features-supported") ||
          !strcmp(attribute, "pdf-versions-supported") ||
          !strncmp(attribute, "pdl-init", 8) ||
          !strcmp(attribute, "pdl-override-supported") ||
          !strcmp(attribute, "printer-settable-attributes-supported") ||
          !strcmp(attribute, "proof-print-supported") ||
          !strcmp(attribute, "pwg-raster-document-sheet-back") ||
          !strcmp(attribute, "pwg-raster-document-type-supported") ||
          !strcmp(attribute, "requested-attributes") ||
          !strcmp(attribute, "save-info-supported") ||
          !strcmp(attribute, "stitching-supported") ||
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

      if (!strncmp(attribute, "notify-", 7) && strcmp(attribute, "notify-events"))
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
	  printf("msgid \"%s\"\n", loc->id);
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