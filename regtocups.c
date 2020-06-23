/*
 * Program to convert registrations to CUPS code.
 *
 * Copyright © 2018-2020 by The IEEE-ISTO Printer Working Group.
 * Copyright © 2008-2018 by Michael R Sweet
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 *
 * Usage:
 *
 *    ./regtocups --encode [OPTIONS] SOURCE.xml >DESTINATION.c
 *    ./regtocups --validate [OPTIONS] SOURCE.xml >DESTINATION.c
 *
 * Options:
 *
 *   -f FUNCTION-NAME     Specify the function name.
 *   -g "GROUP-NAME"      Specify the attribute group name.
 *   -n "ATTRIBUTE-NAME"  Specify one or more attributes.
 */

#include <cups/cups.h>
#include <mxml.h>
#include <limits.h>
#include <ctype.h>


/* Common IPP registry stuff */
#include "ipp-registry.h"

/* "Canned" localizations */
#include "ipp-strings.h"


/*
 * Local functions...
 */

static int		usage(void);
static void		write_strings(mxml_node_t *registry_node, const char *group_name, cups_array_t *attrnames);


/*
 * 'main()' - Write a C header file for specific keywords in the registry.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line arguments */
{
  const char	*xmlin = NULL;		/* XML registration input file */
  mxml_node_t	*xml,			/* XML registration file top node */
		*registry_node;		/* Current registry node */
  FILE		*xmlfile;		/* XML registration file pointer */


  if (argc != 2)
    return (usage());

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

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_KEYWORDS, MXML_DESCEND)) != NULL)
  {
    puts("/* \"media-source\" strings */");
    puts("static const char * const media_sources[][2] =");
    puts("{");
    write_strings(registry_node, "media-source");
    puts("};\n\n");

    puts("/* \"media-type\" strings */");
    puts("static const char * const media_types[][2] =");
    puts("{");
    write_strings(registry_node, "media-type");
    puts("};\n\n");
  }

  return (0);
}


/*
 * 'usage()' - Show program usage.
 */

static int				/* O - Exit status */
usage(void)
{
  puts("\nUsage: ./regtoc filename.xml >filename.c\n");
  return (1);
}


/*
 * 'write_strings()' - Write strings for registered enums and keywords.
 */

static void
write_strings(
    mxml_node_t *registry_node,		/* I - Registry */
    const char  *attrname)		/* I - Attribute name */
{
  mxml_node_t	*record_node,		/* Current record node */
		*attribute_node,	/* Attribute for localization */
		*value_node;		/* Value for localization */
  char		localized[1024];	/* Localized string */


  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record", NULL,
                                     NULL, MXML_NO_DESCEND))
  {
    attribute_node  = mxmlFindElement(record_node, record_node, "attribute",
                                      NULL, NULL, MXML_DESCEND_FIRST);
    value_node      = mxmlFindElement(record_node, record_node, "value",
                                      NULL, NULL, MXML_DESCEND_FIRST);

    if (attribute_node && value_node)
    {
     /*
      * See if this is an attribute we want to localize...
      */

      const char *attribute = mxmlGetOpaque(attribute_node),
                 *value = mxmlGetOpaque(value_node);

      if (!strcmp(attribute, attrname) && value)
      {
       /*
        * Job template or operation attribute that isn't otherwise localized.
        */

        printf("  { \"%s\", _(\"%s\") },\n", value, ipp_get_localized(attribute, value, value, localized, sizeof(localized)));
      }

      continue;
    }
  }
}
