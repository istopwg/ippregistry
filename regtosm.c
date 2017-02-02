/*
 * Program to convert the IPP registrations into a PWG Semantic Model schema.
 *
 * Usage:
 *
 *    ./regtosm [-n nsurl-url] [-v version] filename.xml output-directory version
 *
 * Copyright (c) 2008-2017 by Michael R Sweet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <mxml.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>


/* Common IPP registry stuff */
#include "ipp-registry.h"


/*
 * Local functions...
 */

static void		create_well_known_values(mxml_node_t *xsdnode, mxml_node_t *registry_node);
static FILE		*create_xsd_file(const char *directory, const char *name);
static mxml_node_t	*create_xsd_root(const char *nsurl, const char *version, const char *annotation);
static int		get_current_date(int *month);
static char		*get_sm_name(const char *ipp, char *sm, size_t smsize);
static const char	*save_cb(mxml_node_t *node, int column);
static int		usage(void);


/*
 * 'main()' - Convert the IPP registry into a PWG Semantic Model schema.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line arguments */
{
  int		i;			/* Looping var */
  char		*opt;			/* Command-line option */
  const char	*xmlin = NULL,		/* XML registration input file */
		*directory = NULL;	/* Output directory */
  mxml_node_t	*xml,			/* XML registration file top node */
		*registry_node;		/* Current registry node */
  FILE		*xmlfile;		/* XML registration file pointer */
  char		nsurl[1024] = "",	/* Namespace URL */
		version[256] = "";	/* Schema version */
  mxml_node_t	*xsdnode;		/* XSD node */
  FILE		*xsdfile;		/* XSD file pointer */


 /*
  * Process command-line arguments...
  */

  for (i = 1; i < argc; i ++)
  {
    if (argv[i][0] == '-')
    {
      for (opt = argv[i] + 1; *opt; opt ++)
      {
        switch (*opt)
        {
          case 'n' : /* -n namespace-url */
              i ++;
              if (i < argc)
              {
		strncpy(nsurl, argv[i], sizeof(nsurl) - 1);
		nsurl[sizeof(nsurl) - 1] = '\0';
	      }
	      else
	      {
	        fputs("regtosm: Missing namespace URL after '-n'.\n", stderr);
	        return (usage());
	      }
	      break;

          case 'v' : /* -v version */
              i ++;
              if (i < argc)
              {
		strncpy(version, argv[i], sizeof(version) - 1);
		version[sizeof(version) - 1] = '\0';
	      }
	      else
	      {
	        fputs("regtosm: Missing version after '-v'.\n", stderr);
	        return (usage());
	      }
	      break;

          default :
              fprintf(stderr, "regtosm: Unknown command-line option '-%c'.\n", *opt);
              return (usage());
        }
      }
    }
    else if (!xmlin)
      xmlin = argv[i];
    else if (!directory)
      directory = argv[i];
    else
    {
      fprintf(stderr, "regtosm: Unknown command-line argument '%s'.\n", argv[i]);
      return (usage());
    }
  }

  if (!xmlin || !directory)
    return (usage());

 /*
  * Provide defaults for namespace URL and version...
  */

  if (!nsurl[0])
  {
   /* Default namespace URL is "http://www.pwg.org/schemas/YYYY/MM/sm" */
    int month, year = get_current_date(&month);

    snprintf(nsurl, sizeof(nsurl), "http://www.pwg.org/schemas/%04d/%02d/sm", year, month);
  }

  if (!version[0])
  {
   /* Default version is "2.9<number-of-days-since-1970>" */
    snprintf(version, sizeof(version), "2.9%ld", (long)(time(NULL) / 86400));
  }

 /*
  * Load the XML registration file...
  */

  mxmlSetWrapMargin(INT_MAX);

  if ((xmlfile = fopen(xmlin, "rb")) == NULL)
  {
    fprintf(stderr, "regtosm: Unable to open XML registration file \"%s\": %s\n", xmlin, strerror(errno));
    return (1);
  }

  xml = mxmlLoadFile(NULL, xmlfile, ipp_load_cb);
  fclose(xmlfile);

  if (!xml)
  {
    fprintf(stderr, "regtosm: Bad XML registration file \"%s\".\n", xmlin);
    return (1);
  }

 /*
  * Output well-known value strings strings...
  */

#if 0
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) != NULL)
    write_strings(registry_node);
#endif /* 0 */

  puts("Generating PwgWellKnownValues.xsd...");

  xsdnode = create_xsd_root(nsurl, version, "Well known values (i.e. keywords) used by semantic elements.");

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ENUMS, MXML_DESCEND)) != NULL)
    create_well_known_values(xsdnode, registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_KEYWORDS, MXML_DESCEND)) != NULL)
    create_well_known_values(xsdnode, registry_node);

  if ((xsdfile = create_xsd_file(directory, "PwgWellKnownValues")) != NULL)
  {
    mxmlSaveFile(mxmlGetParent(xsdnode), xsdfile, save_cb);
    fclose(xsdfile);
  }
  else
    return (1);

  mxmlDelete(mxmlGetParent(xsdnode));

  return (0);
}


/*
 * 'create_well_known_values()' - Create the well-known values nodes.
 */

static void
create_well_known_values(
    mxml_node_t *xsdnode,		/* I - xs:schema node */
    mxml_node_t *registry_node)		/* I - IPP registry node */
{
  mxml_node_t	*record_node,		/* Current record node */
		*attribute_node,	/* Attribute for localization */
		*name_node,		/* Keyword string to be localized */
		*value_node;		/* Value for localization */
  char		smname[1024],		/* SM name */
		smvalue[1024];		/* SM value */
  mxml_node_t	*xs_simpleType,		/* Simple type for well-known value */
		*xs_restriction,	/* Restrictions on simpleType */
		*xs_maxLength,		/* Maximum length of value */
		*xs_enumeration;	/* Enumerated value */
  const char	*last_attribute = NULL;	/* Last attribute written */


  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_NO_DESCEND))
  {
    attribute_node  = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL, MXML_DESCEND_FIRST);
    value_node      = mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST);

    if (attribute_node && value_node)
    {
      const char *attribute = mxmlGetOpaque(attribute_node),
                 *value = mxmlGetOpaque(value_node);

      if (!last_attribute || strcmp(last_attribute, attribute))
      {
       /* Start a new xs:simpleType */
        last_attribute = attribute;

        xs_simpleType = mxmlNewElement(xsdnode, "xs:simpleType");
        mxmlElementSetAttrf(xs_simpleType, "name", "%sWKV", get_sm_name(attribute, smname, sizeof(smname)));

	xs_restriction = mxmlNewElement(xs_simpleType, "xs:restriction");
	mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

	xs_maxLength = mxmlNewElement(xs_restriction, "xs:maxLength");
	mxmlElementSetAttr(xs_maxLength, "value", "255");
      }

      xs_enumeration = mxmlNewElement(xs_restriction, "xs_enumeration");
      mxmlElementSetAttr(xs_enumeration, "value", get_sm_name(value, smvalue, sizeof(smvalue)));
    }
  }
}


/*
 * 'create_xsd_file()' - Create an XML schema description file.
 *
 * Also created the output directory, if not present.
 */

static FILE *				/* O - FILE pointer or NULL on error */
create_xsd_file(const char *directory,	/* I - Directory */
                const char *name)	/* I - Base filename (no extension) */
{
  FILE	*fp;				/* Output file */
  char	filename[1024];			/* Output filename */


  if (access(directory, 0))
  {
    if (errno == ENOENT)
    {
      if (mkdir(directory, 0777))
      {
	fprintf(stderr, "regtosm: Unable to create directory \"%s\": %s\n", directory, strerror(errno));
	return (NULL);
      }
    }
    else
    {
      fprintf(stderr, "regtosm: Unable to access directory \"%s\": %s\n", directory, strerror(errno));
      return (NULL);
    }
  }

  snprintf(filename, sizeof(filename), "%s/%s.xsd", directory, name);

  if ((fp = fopen(filename, "w")) == NULL)
  {
    fprintf(stderr, "regtosm: Unable to create \"%s\": %s\n", filename, strerror(errno));
    return (NULL);
  }

  return (fp);
}


/*
 * 'create_xsd_root()' - Create a standard root node for an XML schema
 *                       description.
 *
 * Note: Returns a pointer to the "xs:schema" node; use mxmlGetParent() to get
 * the root node.
 */

static mxml_node_t *			/* O - xs:schema node */
create_xsd_root(const char *nsurl,	/* I - Namespace URL */
	        const char *version,	/* I - Schema version */
                const char *annotation)/* I - Comment for header */
{
  mxml_node_t	*root,			/* Root node */
		*xs_schema,		/* xs:schema node */
		*xs_documentation,	/* xs:documentation node */
		*xs_annotation;		/* xs:annotation node */
  char		header[2048];		/* Standard header text */


  root      = mxmlNewXML("1.0");
  xs_schema = mxmlNewElement(root, "xs:schema");

  mxmlElementSetAttr(xs_schema, "xmlns", nsurl);
  mxmlElementSetAttr(xs_schema, "xmlns:xs", "http://www.w3.org/2001/XMLSchema");
  mxmlElementSetAttr(xs_schema, "xmlns:pwg", nsurl);
  mxmlElementSetAttr(xs_schema, "targetNamespace", nsurl);
  mxmlElementSetAttr(xs_schema, "elementFormDefault", "qualified");
  mxmlElementSetAttr(xs_schema, "attributeFormDefault", "qualified");
  mxmlElementSetAttr(xs_schema, "version", version);

  xs_documentation = mxmlNewElement(xs_schema, "xs:documentation");
  xs_annotation    = mxmlNewElement(xs_documentation, "xs:annotation");

  snprintf(header, sizeof(header),
           "PWG Semantic Model v%s\n"
           "Copyright © 2002-%d The IEEE-ISTO Printer Working Group.\n\n"
           "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
           "of this software and associated documentation files (the \"Software\"), to deal\n"
           "in the Software without restriction, including without limitation the rights\n"
           "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
           "copies of the Software, and to permit persons to whom the Software is\n"
           "furnished to do so, subject to the following conditions:\n"
           "\n"
           "The above copyright notice and this permission notice shall be included in all\n"
           "copies or substantial portions of the Software.\n"
           "\n"
           "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
           "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
           "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
           "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
           "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
           "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
           "SOFTWARE.",
           version, get_current_date(NULL));

  mxmlNewOpaque(xs_annotation, header);

  if (annotation)
  {
    xs_documentation = mxmlNewElement(xs_schema, "xs:documentation");
    xs_annotation    = mxmlNewElement(xs_documentation, "xs:annotation");

    mxmlNewOpaque(xs_annotation, annotation);
  }

  return (xs_schema);
}


/*
 * 'get_current_date()' - Get the current year and month.
 */

static int				/* O - Current year */
get_current_date(int *month)		/* O - Current month (NULL = don't care) */
{
  time_t	curtime;		/* Current time */
  struct tm	*curdate;		/* Current date */


  curtime = time(NULL);
  curdate = localtime(&curtime);

  if (month)
    *month = curdate->tm_mon + 1;

  return (curdate->tm_year + 1900);
}


/*
 * 'get_sm_name()' - Convert an IPP keyword/name into a Semantic Model name.
 */

static char *				/* O - SM name */
get_sm_name(const char *ipp,		/* I - IPP keyword/name */
            char       *sm,		/* I - SM name buffer */
            size_t     smsize)		/* I - Size of name buffer */
{
  char	*smptr = sm, *smend = sm + smsize - 1;
					/* Pointer into SM name buffer and end */


 /*
  * Convert "foo-bar-bla" into "FooBarBla".
  */

  *smptr++ = toupper(*ipp++);

  while (*ipp && smptr < smend)
  {
    if (*ipp == '-' && ipp[1])
    {
      ipp ++;
      *smptr++ = toupper(*ipp++);
    }
    else
      *smptr++ = *ipp++;
  }

  *smptr = '\0';

  return (sm);
}


/*
 * 'save_cb()' - Save (whitespace) callback...
 */

static const char *			/* O - Whitespace to output */
save_cb(mxml_node_t *node,		/* I - Current node */
        int         where)		/* I - Where we are */
{
  int	level;				/* Indentation level */
  static const char *spaces = "                                        ";
					/* 40 spaces */


  switch (where)
  {
    default :
        return (NULL);

    case MXML_WS_AFTER_OPEN :
        if (!strcmp(mxmlGetElement(node), "xs:annotation"))
          return (NULL);
    case MXML_WS_AFTER_CLOSE :
        return ("\n");

    case MXML_WS_BEFORE_CLOSE :
        if (!strcmp(mxmlGetElement(node), "xs:annotation"))
          return (NULL);
    case MXML_WS_BEFORE_OPEN :
	for (level = -4; node; node = node->parent, level += 2);

	if (level <= 0)
	  return (NULL);
	else if (level > 40)
	  return (spaces);
	else
	  return (spaces + 40 - level);
  }
}


/*
 * 'usage()' - Show program usage.
 */

static int				/* O - Exit status */
usage(void)
{
  puts("\nUsage: ./regtosm filename.xml output-directory\n");
  return (1);
}


#if 0
/*
 * 'write_strings()' - Write strings for registered enums and keywords.
 */

static void
write_strings(
    mxml_node_t *registry_node)		/* I - Registry */
{
  mxml_node_t	*record_node,		/* Current record node */
		*attribute_node,	/* Attribute for localization */
		*collection_node,	/* Collection (for attributes) */
		*member_node,		/* Member attribute node (for attributes */
		*name_node,		/* Keyword string to be localized */
		*syntax_node,		/* Syntax string (for attributes) */
		*value_node;		/* Value for localization */
  char		localized[1024];	/* Localized string */
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
      			*name = mxmlGetOpaque(name_node),
			*syntax = mxmlGetOpaque(syntax_node);

      if (collection && name && syntax && !member_node &&
          !strstr(name, "-default") &&
          !strstr(name, "-ready") &&
          !strstr(name, "-supported") &&
          strcmp(name, "attributes-charset") &&
          strcmp(name, "attributes-natural-language") &&
          strcmp(name, "current-page-order") &&
          strcmp(name, "document-access-error") &&
          strcmp(name, "document-charset") &&
          strcmp(name, "document-format") &&
          strcmp(name, "document-format-details") &&
          strcmp(name, "document-natural-language") &&
          strcmp(name, "first-index") &&
          strcmp(name, "job-finishings-col") &&
          strcmp(name, "job-id") &&
          strcmp(name, "job-ids") &&
          strcmp(name, "job-impressions") &&
          strcmp(name, "job-k-octets") &&
          strcmp(name, "job-media-sheets") &&
          strcmp(name, "job-message-from-operator") &&
          strcmp(name, "job-message-to-operator") &&
          strcmp(name, "job-uri") &&
          strcmp(name, "last-document") &&
          strcmp(name, "limit") &&
          strcmp(name, "message") &&
          strcmp(name, "my-jobs") &&
          strcmp(name, "notify-charset") &&
          strcmp(name, "original-requesting-user-name") &&
          strcmp(name, "overrides") &&
          strcmp(name, "pdl-init-file") &&
          strcmp(name, "printer-uri") &&
	  (!strcmp(collection, "Job Template") ||
           !strcmp(collection, "Operation") ||
           !strcmp(collection, "Subscription Template")) &&
           !strstr(syntax, "keyword") && !strstr(syntax, "enum"))
      {
       /*
        * Job template or operation attribute that isn't otherwise localized.
        */

        printf("\"%s\" = \"%s\";\n", name,
               get_localized("", name, name, localized, sizeof(localized)));
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

      if (!strcmp(attribute, "cover-back-supported") ||
          !strcmp(attribute, "cover-front-supported") ||
          !strcmp(attribute, "current-page-order") ||
          !strcmp(attribute, "document-digital-signature") ||
          !strcmp(attribute, "document-format-details-supported") ||
          !strcmp(attribute, "document-format-varying-attributes") ||
          !strcmp(attribute, "finishings-col-supported") ||
          !strcmp(attribute, "ipp-features-supported") ||
          !strcmp(attribute, "ipp-versions-supported") ||
          !strcmp(attribute, "job-mandatory-attributes") ||
          !strcmp(attribute, "job-password-encryption") ||
          !strcmp(attribute, "job-save-disposition-supported") ||
          !strcmp(attribute, "job-spooling-supported") ||
          !strcmp(attribute, "media-col-supported") ||
          !strcmp(attribute, "media-key") ||
          !strcmp(attribute, "media-source-feed-direction") ||
          !strcmp(attribute, "media-source-feed-orientation") ||
          !strcmp(attribute, "notify-pull-method") ||
          !strcmp(attribute, "notify-pull-method-supported") ||
          !strcmp(attribute, "pdl-init-file-supported") ||
          !strcmp(attribute, "pdl-override-supported") ||
          !strcmp(attribute, "printer-settable-attributes-supported") ||
          !strcmp(attribute, "proof-print-supported") ||
          !strcmp(attribute, "pwg-raster-document-sheet-back") ||
          !strcmp(attribute, "pwg-raster-document-type-supported") ||
          !strcmp(attribute, "save-info-supported") ||
          !strcmp(attribute, "stitching-supported") ||
          !strcmp(attribute, "uri-authentication-supported") ||
          !strcmp(attribute, "uri-security-supported") ||
          !strcmp(attribute, "which-jobs") ||
          !strcmp(attribute, "xri-authentication-supported") ||
          !strcmp(attribute, "xri-security-supported"))
        continue;

      if ((!last_attribute || strcmp(attribute, last_attribute)) &&
          !strstr(attribute, "-default") && !strstr(attribute, "-supported") &&
          !strstr(attribute, "-ready"))
      {
        printf("\"%s\" = \"%s\";\n", attribute,
               get_localized("", attribute, attribute, localized,
                             sizeof(localized)));
        last_attribute = attribute;
      }

      if (value[0] != '<' && value[0] != ' ' && strcmp(name, "Unassigned") &&
          strcmp(attribute, "operations-supported") &&
          (strcmp(attribute, "media") || strchr(value, '_') != NULL))
	printf("\"%s.%s\" = \"%s\";\n", attribute, value,
	       get_localized(attribute, name, value, localized,
	       sizeof(localized)));
      else if (!strcmp(attribute, "operations-supported") &&
               strncmp(name, "Reserved (", 10))
        printf("\"%s.%ld\" = \"%s\";\n", attribute, strtol(value, NULL, 0),
               name);
    }
  }
}
#endif /* 0 */
