/*
 * Program to convert keyword and enum registrations to C strings.
 *
 * Usage:
 *
 *    ./regtoc filename.xml >filename.h
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

/* "Canned" localizations */
#include "ipp-strings.h"


/*
 * Local functions...
 */

static int		usage(void);
static void		write_strings(mxml_node_t *registry_node, const char *attrname);


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
    value_node      = mxmlFindElement(record_node, record_node, "value",
                                      NULL, NULL, MXML_DESCEND_FIRST);

    if (attribute_node && name_node)
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
