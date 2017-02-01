/*
 * Program to convert the IPP registrations into a PWG Semantic Model schema.
 *
 * Usage:
 *
 *    ./regtosm filename.xml output-directory version
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


/* Common IPP registry stuff */
#include "ipp-registry.h"


/*
 * Local functions...
 */

static char	*get_sm_name(const char *ipp, char *sm, size_t smsize);
static int	usage(void);


/*
 * 'main()' - Convert the IPP registry into a PWG Semantic Model schema.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line arguments */
{
  const char	*xmlin = NULL;		/* XML registration input file */
  mxml_node_t	*xml,			/* XML registration file top node */
		*registry_node;		/* Current registry node */
  FILE		*xmlfile;		/* XML registration file pointer */


  char temp[10];
  get_sm_name("foo", temp, sizeof(temp));
  usage();

#if 0
 /*
  * Load the XML registration file...
  */

  xmlin = argv[1];

  mxmlSetWrapMargin(INT_MAX);

  if ((xmlfile = fopen(xmlin, "rb")) == NULL)
  {
    perror(xmlin);
    return (1);
  }

  xml = mxmlLoadFile(NULL, xmlfile, load_cb);
  fclose(xmlfile);

  if (!xml)
  {
    fputs("Bad XML registration file.\n", stderr);
    return (1);
  }

 /*
  * Output keyword and enum strings...
  */

  qsort(strings, sizeof(strings) / sizeof(strings[0]), sizeof(ipp_loc_t),
        (int (*)(const void *, const void *))compare_loc);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) != NULL)
    write_strings(registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_ENUMS, MXML_DESCEND)) != NULL)
    write_strings(registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_KEYWORDS, MXML_DESCEND)) != NULL)
    write_strings(registry_node);
#endif /* 0 */

  return (0);
}


/*
 * 'get_sm_name()' - Convert an IPP keyword/name into a Semantic Model name.
 */

static char *				/* O - SM name */
get_sm_name(const char *ipp,		/* I - IPP keyword/name */
            char       *sm,		/* I - SM name buffer */
            size_t     smsize)		/* I - Size of name buffer */
{
  *sm = '\0';
  return (sm);
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
