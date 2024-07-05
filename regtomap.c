//
// Program to generate a Job Template attribute map table.
//
// Usage:
//
//    ./regtomap [--faxout] [--jekyll] [--print3d] [--scan] FILENAME.xml >FILENAME.html
//
// Copyright © 2024 by The IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#include "ipp-registry.h"


//
// Local types...
//

typedef struct reg_attr_s		// Registry attribute
{
  const char	*group,			// Attribute group (collection) name
		*name,			// Attribute name
		*syntax,		// Attribute syntax
		*xref_url;		// Reference URL
  char		xref_name[32];		// Reference name
} reg_attr_t;

typedef enum service_type_e		// Service type
{
  SERVICE_TYPE_FAXOUT,
  SERVICE_TYPE_PRINT,
  SERVICE_TYPE_PRINT3D,
  SERVICE_TYPE_SCAN
} service_type_t;


//
// Local functions...
//

static int		compare_attributes(reg_attr_t *a, reg_attr_t *b);
static reg_attr_t	*find_attribute(const char *name, size_t num_attrs, reg_attr_t *attrs);
static void		print_attribute(reg_attr_t *attr, reg_attr_t *attr_default, reg_attr_t *attr_supported, reg_attr_t *attr_max);
static int		usage(void);


//
// 'main()' - Write a strings file for all keywords and enums in the registry.
//

int					// O - Exit status
main(int  argc,				// I - Number of command-line args
     char *argv[])			// I - Command-line arguments
{
  int           i;                      // Looping var
  bool		jekyll = false;		// Generate a HTML file for the Jekyll-based web site?
  size_t	num_attrs = 0,		// Number of attributes
		alloc_attrs = 0,	// Allocated attributes
		remaining;		// Looping var
  reg_attr_t	*attrs = NULL,		// Attribute list
		*attr;			// Current attribute
  const char	*xmlin = NULL;		// XML registration input file
  mxml_options_t *xmloptions;		// XML load options
  mxml_node_t	*xml,			// XML registration file top node
		*registry_node,		// Attribute registry node
		*record_node,		// Current record node
		*collection_node,	// Collection
		*name_node,		// Keyword string to be localized
		*syntax_node,		// Syntax string
		*xref_node;		// Reference document
  service_type_t service_type = SERVICE_TYPE_PRINT;
					// Service type
  static const char * const titles[] =	// Titles for each service type
  {
    "IPP FaxOut Job Template Attributes",
    "IPP Print Job Template Attributes",
    "IPP 3D Print Job Template Attributes",
    "IPP Scan Job Template Attributes"
  };


  // Parse command-line...
  for (i = 1; i < argc; i ++)
  {
    if (!strcmp(argv[i], "--faxout"))
    {
      service_type = SERVICE_TYPE_FAXOUT;
    }
    else if (!strcmp(argv[i], "--jekyll"))
    {
      jekyll = true;
    }
    else if (!strcmp(argv[i], "--print3d"))
    {
      service_type = SERVICE_TYPE_PRINT3D;
    }
    else if (!strcmp(argv[i], "--scan"))
    {
      service_type = SERVICE_TYPE_SCAN;
    }
    else if (argv[i][0] == '-' || xmlin)
    {
      fprintf(stderr, "regtostrings: Unknown option \'%s\'.\n", argv[i]);
      return (usage());
    }
    else
    {
      xmlin = argv[i];
    }
  }

  if (!xmlin)
    return (usage());

  // Load the XML registration file...
  xmloptions = mxmlOptionsNew();
  mxmlOptionsSetErrorCallback(xmloptions, (mxml_error_cb_t)ipp_error_cb, (void *)"register");
  mxmlOptionsSetTypeCallback(xmloptions, ipp_type_cb, /*cbdata*/NULL);

  if ((xml = mxmlLoadFilename(/*top*/NULL, xmloptions, xmlin)) == NULL)
    return (1);

  // Get the attribute registry...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("regtomap: No IPP attribute registry section found.\n", stderr);
    return (1);
  }

  // Loop through the registry...
  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    if (mxmlFindElement(record_node, record_node, "member_attribute", NULL, NULL, MXML_DESCEND_FIRST))
      continue;

    collection_node = mxmlFindElement(record_node, record_node, "collection", NULL, NULL, MXML_DESCEND_FIRST);
    name_node       = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    syntax_node     = mxmlFindElement(record_node, record_node, "syntax", NULL, NULL, MXML_DESCEND_FIRST);
    xref_node       = mxmlFindElement(record_node, record_node, "xref", NULL, NULL, MXML_DESCEND_FIRST);

    if (collection_node && name_node && syntax_node && xref_node)
    {
      // Allocate memory
      if (num_attrs >= alloc_attrs)
      {
        if ((attr = realloc(attrs, (alloc_attrs + 100) * sizeof(reg_attr_t))) == NULL)
        {
          fputs("regtomap: Out of memory.\n", stderr);
          return (1);
        }

        attrs       = attr;
        alloc_attrs += 100;
      }

      // Add a record
      attr        = attrs + num_attrs;
      attr->group = mxmlGetOpaque(collection_node);
      attr->name  = mxmlGetOpaque(name_node);

      if (strstr(attr->name, "(deprecated)") || strstr(attr->name, "(obsolete)"))
        continue;

      if (strcmp(attr->group, "Job Template") && strcmp(attr->group, "Printer Description"))
        continue;

      attr->syntax   = mxmlGetOpaque(syntax_node);
      attr->xref_url = mxmlElementGetAttr(xref_node, "data");

      if (!strncmp(attr->xref_url, "rfc", 3))
      {
        snprintf(attr->xref_name, sizeof(attr->xref_name), "RFC&nbsp;%s", attr->xref_url + 3);
      }
      else
      {
        const char *text = mxmlGetOpaque(xref_node);
					// Reference text

        if (service_type == SERVICE_TYPE_FAXOUT)
        {
          if (strcmp(text, "PWG5100.15"))
	    continue;

	  if (strcmp(attr->name, "copies") && strcmp(attr->name, "media") && strcmp(attr->name, "media-col") && strcmp(attr->name, "multiple-document-handling") && strcmp(attr->name, "number-up") && strcmp(attr->name, "orientation-requested") && strcmp(attr->name, "overrides") && strcmp(attr->name, "page-ranges") && strcmp(attr->name, "print-color-mode") && strcmp(attr->name, "print-quality") && strcmp(attr->name, "print-rendering-intent") && strcmp(attr->name, "printer-resolution"))
	    continue;
	}
        else if (service_type == SERVICE_TYPE_PRINT && (!strcmp(text, "PWG5100.15") || !strcmp(text, "PWG5100.17") || !strcmp(text, "PWG5100.21")))
        {
          continue;
        }
        else if (service_type == SERVICE_TYPE_PRINT3D)
        {
          if (strcmp(text, "PWG5100.21"))
	    continue;

	  if (strcmp(attr->name, "copies") && strcmp(attr->name, "print-quality"))
	    continue;
	}
        else if (service_type == SERVICE_TYPE_SCAN && strcmp(text, "PWG5100.17"))
        {
          continue;
        }

        if (!strncmp(text, "PWG", 3))
        {
          // Add a non-breaking space between "PWG" and the number...
          snprintf(attr->xref_name, sizeof(attr->xref_name), "%c%c%c&nbsp;%s", text[0], text[1], text[2], text + 3);
        }
        else
        {
          // Use the literal string
          ipp_copy_string(attr->xref_name, text, sizeof(attr->xref_name));
        }
      }

      num_attrs ++;
    }
  }

  if (num_attrs > 1)
    qsort(attrs, num_attrs, sizeof(reg_attr_t), (int (*)(const void *, const void *))compare_attributes);

  // Now produce the output...
  if (jekyll)
  {
    puts("---");
    printf("title: %s\n", titles[service_type]);
    puts("layout: default");
    puts("---");
  }
  else
  {
    puts("<!doctype html>");
    puts("<html>");
    puts("  <head>");
    printf("    <title>%s</title>\n", titles[service_type]);
    puts("    <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH\" crossorigin=\"anonymous\">");
    puts("    <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz\" crossorigin=\"anonymous\"></script>");
    puts("  </head>");
    puts("  <body>");
    printf("    <h1>%s</h1>\n", titles[service_type]);
  }

  printf("    <table class=\"table table-condensed table-striped standard\" summary=\"%s\">\n", titles[service_type]);
  puts("      <thead>");
  puts("        <tr>");
  puts("          <th>Job Template Attribute</th>");
  puts("          <th>Default Attribute</th>");
  puts("          <th>Supported Attribute(s)</th>");
  puts("          <th>Reference</th>");
  puts("        </tr>");
  puts("      </thead>");
  puts("      <tbody>");

  for (attr = attrs, remaining = num_attrs; remaining > 0; attr ++, remaining --)
  {
    char	name_default[128],	// xxx-default
		name_supported[128],	// xxx-supported
		name_max[128];		// max-xxx-supported

    if (attr->group[0] != 'J')
      continue;

    snprintf(name_default, sizeof(name_default), "%s-default", attr->name);
    snprintf(name_supported, sizeof(name_supported), "%s-supported", attr->name);
    snprintf(name_max, sizeof(name_max), "max-%s-supported", attr->name);

    print_attribute(attr, find_attribute(name_default, num_attrs, attrs), find_attribute(name_supported, num_attrs, attrs), find_attribute(name_max, num_attrs, attrs));
  }

  puts("      </tbody>");
  puts("    </table>");



  if (!jekyll)
  {
    puts("  </body>");
    puts("</html>");
  }

  return (0);
}


//
// 'compare_attributes()' - Compare two attributes.
//

static int				// O - Result of comparison
compare_attributes(reg_attr_t *a,	// I - First attribute
                   reg_attr_t *b)	// I - Second attribute
{
  return (strcmp(a->name, b->name));
}


//
// 'find_attribute()' - Find an attribute.
//

static reg_attr_t *			// O - Attribute or `NULL`
find_attribute(const char *name,	// I - Attribute name
               size_t     num_attrs,	// I - Number of attributes
               reg_attr_t *attrs)	// I - Attributes
{
  reg_attr_t	key;			// Search key


  key.name = name;

  return ((reg_attr_t *)bsearch(&key, attrs, num_attrs, sizeof(reg_attr_t), (int (*)(const void *, const void *))compare_attributes));
}


//
// 'print_attribute()' - Print an attribute

static void
print_attribute(
    reg_attr_t *attr,			// I - Job template attribute
    reg_attr_t *attr_default,		// I - xxx-default attribute, if any
    reg_attr_t *attr_supported,		// I - xxx-supported attribute, if any
    reg_attr_t *attr_max)		// I - max-xxx attribute, if any
{
  printf("        <tr><td>%s (%s)</td><td>", attr->name, attr->syntax);
  if (attr_default)
    printf("%s (%s)", attr_default->name, attr_default->syntax);
  fputs("</td><td>", stdout);
  if (attr_supported)
    printf("%s (%s)", attr_supported->name, attr_supported->syntax);
  if (attr_max)
    printf("<br>%s (%s)", attr_max->name, attr_max->syntax);
  if (!strncmp(attr->xref_url, "rfc", 3))
    printf("</td><td><a href=\"https://datatracker.ietf.org/doc/html/%s\" target=\"_blank\">%s</a></td></tr>\n", attr->xref_url, attr->xref_name);
  else
    printf("</td><td><a href=\"%s\" target=\"_blank\">%s</a></td></tr>\n", attr->xref_url, attr->xref_name);
}


//
// 'usage()' - Show program usage.
//

static int				// O - Exit status
usage(void)
{
  puts("\nUsage: ./regtomap [--faxout] [--jekyll] [--print3d] [--scan] FILENAME.xml >FILENAME.html");
  return (1);
}
