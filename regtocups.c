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
 *    ./regtocups encode [OPTIONS] SOURCE.xml >DESTINATION.c
 *    ./regtocups validate [OPTIONS] SOURCE.xml >DESTINATION.c
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
#include <stdbool.h>

/* Common IPP registry stuff */
#include "ipp-registry.h"


/*
 * Local types...
 */

typedef struct reg_value_s		/**** Syntax value description ****/
{
  bool		is_1setOf;		/* 1setOf? */
  ipp_tag_t	value_tag;		/* Value tag */
  int		min_value,		/* Minimum value/length */
		max_value;		/* Maximum value/length */
} reg_value_t;

/*
 * Local functions...
 */

static int		parse_syntax(const char *syntax, int max_values, reg_value_t *values);
static int		usage(FILE *fp);


/*
 * 'main()' - Write a C header file for specific keywords in the registry.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line arguments */
{
  int		i;			/* Looping var */
  const char	*opt,			/* Current option */
		*subcmd,		/* Subcommand */
		*xmlin = NULL,		/* XML registration input file */
		*function = NULL,	/* Function name */
		*group = NULL;		/* Group name, if any */
  int		num_attrs = 0;		/* Number of attribute names */
  const char	*attrs[1000];		/* Attribute names */
  char		*nameptr;		/* Pointer into name string */
  mxml_node_t	*xml,			/* XML registration file top node */
		*registry_node,		/* Current registry node */
		*record_node;		/* Current record node */
  const char	*collection,		/* Collection (group) for record */
		*name,			/* Name for record */
		*syntax;		/* Syntax for record */
  int		num_values;		/* Number of syntax values */
  reg_value_t	values[4];		/* Syntax values */
  FILE		*xmlfile;		/* XML registration file pointer */


 /*
  * Parse the command-line...
  */

  for (i = 1; i < argc; i ++)
  {
    if (!strcmp(argv[i], "--help"))
    {
      return (usage(stdout));
    }
    else if (!strncmp(argv[i], "--", 2))
    {
      fprintf(stderr, "regtocups: Unsupported option '%s'.\n", argv[i]);
      return (usage(stderr));
    }
    else if (argv[i][0] == '-')
    {
      for (opt = argv[i] + 1; *opt; opt ++)
      {
        switch (*opt)
        {
          case 'f' : /* -f FUNCTION-NAME */
              if (function)
              {
                fputs("regtocups: Too many function names specified.\n", stderr);
                return (usage(stderr));
              }

              i ++;
              if (i >= argc)
              {
                fputs("regtocups: Expected function name after '-f'.\n", stderr);
                return (usage(stderr));
              }

              function = argv[i];
              break;
              
          case 'g' : /* -g GROUP-NAME */
              if (group)
              {
                fputs("regtocups: Too many group names specified.\n", stderr);
                return (usage(stderr));
              }

              i ++;
              if (i >= argc)
              {
                fputs("regtocups: Expected group name after '-g'.\n", stderr);
                return (usage(stderr));
              }

              group = argv[i];
              break;

          case 'n' : /* -a ATTRIBUTE-NAME */
              if (num_attrs > (int)(sizeof(attrs) / sizeof(attrs[0])))
              {
                fputs("regtocups: Too many attribute names specified.\n", stderr);
                return (usage(stderr));
              }

              i ++;
              if (i >= argc)
              {
                fputs("regtocups: Expected attribute name(s) after '-n'.\n", stderr);
                return (usage(stderr));
              }

              nameptr = argv[i];
              while (num_attrs < (int)(sizeof(attrs) / sizeof(attrs[0])) && (attrs[num_attrs] = strsep(&nameptr, " \t,")) != NULL)
                num_attrs ++;
              break;

          default :
              fprintf(stderr, "regtocups: Unknown option '-%c'.\n", *opt);
              return (usage(stderr));
        }
      }
    }
    else if (!strcmp(argv[i], "encode") || !strcmp(argv[i], "validate"))
    {
      subcmd = argv[i];
    }
    else if (xmlin)
    {
      fprintf(stderr, "regtocups: Unknown argument '%s'.\n", argv[i]);
      return (usage(stderr));
    }
    else
      xmlin = argv[i];
  }

  if (!subcmd || !xmlin)
    return (usage(stderr));

  if (!function)
    function = subcmd;

  if (!group)
    group = "Job Template";

 /*
  * Load the XML registration file...
  */

  if ((xmlfile = fopen(xmlin, "rb")) == NULL)
  {
    fprintf(stderr, "regtocups: Unable to open '%s': %s\n", xmlin, strerror(errno));
    return (1);
  }

  xml = mxmlLoadFile(NULL, xmlfile, ipp_load_cb);
  fclose(xmlfile);

  if (!xml)
  {
    fprintf(stderr, "regtocups: Bad registration file '%s'.\n", xmlin);
    return (1);
  }

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) == NULL)
  {
    fprintf(stderr, "regtocups: No attributes in registration file '%s'.\n", xmlin);
    return (1);
  }

 /*
  * Run sub-command...
  */

  if (!strcmp(subcmd, "encode"))
  {
   /*
    * Write a function to encode IPP attributes from a cups_option_t array.
    */

    printf("void\n"
           "%s(\n"
           "    ipp_t         *ipp,\n"
           "    int           num_options,\n"
           "    cups_option_t *options)\n"
           "{\n"
           "  while (num_options > 0)\n"
           "  {\n"
           "    const char *value;\n", function);

    for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_NO_DESCEND))
    {
     /*
      * See if we want to encode this attribute...
      */

      collection = mxmlGetOpaque(mxmlFindElement(record_node, record_node, "collection", NULL, NULL, MXML_DESCEND));
      name       = mxmlGetOpaque(mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND));
      syntax     = mxmlGetOpaque(mxmlFindElement(record_node, record_node, "syntax", NULL, NULL, MXML_DESCEND));

      if (!name || !syntax || !collection)
        continue;

      if (group && strcmp(group, collection))
        continue;

      if (num_attrs > 0)
      {
       /*
        * Filter to the list of attributes provided...
        */

        for (i = 0; i < num_attrs; i ++)
        {
          if (!strcmp(name, attrs[i]))
            break;
        }

        if (i >= num_attrs)
          continue;
      }

      if ((num_values = parse_syntax(syntax, (int)(sizeof(values) / sizeof(values[0])), values)) == 0)
      {
        fprintf(stderr, "regtocups: Bad syntax '%s' for attribute '%s'.\n", syntax, name);
	return (1);
      }

     /*
      * Write the encoder...
      */

      printf("\n"
             "    if (!strcmp(options->name, \"%s\"))\n"
             "    {\n", name);

      switch (values[0].value_tag)
      {
        case IPP_TAG_INTEGER :
            printf("      ippAddInteger(ipp, IPP_TAG_%s, IPP_TAG_INTEGER, options->name,  atoi(options->value));\n", "JOB");
            break;

        case IPP_TAG_ENUM :
            printf("      ippAddInteger(ipp, IPP_TAG_%s, IPP_TAG_ENUM, options->name,  ippEnumValue(options->name, options->value));\n", "JOB");
            break;

        case IPP_TAG_CHARSET :
        case IPP_TAG_KEYWORD :
        case IPP_TAG_LANGUAGE :
        case IPP_TAG_MIMETYPE :
        case IPP_TAG_NAME :
        case IPP_TAG_TEXT :
        case IPP_TAG_URI :
        case IPP_TAG_URISCHEME :
            printf("      ippAddString(ipp, IPP_TAG_%s, IPP_TAG_%s, options->name, NULL, options->value);\n", "JOB", "KEYWORD");
            break;

        default :
            fprintf(stderr, "regtocups: Unsupported syntax '%s' for attribute '%s'.\n", syntax, name);
            return (1);
      }

      puts("    }");
    }

    puts("\n"
         "    num_options --;\n"
         "    options ++;\n"
         "  }\n"
         "}");
  }
  else
  {
   /*
    * Write a function to validate IPP attributes in an ipp_t message.
    */

    printf("bool\n"
           "%s(\n"
           "    ipp_t         *ipp)\n"
           "{\n"
           "  ipp_attribute_t *attr;\n"
           "\n"
           "\n"
           "  for (attr = ippFirstAttribute(ipp); attr; attr = ippNextAttribute(ipp))\n"
           "  {\n"
           "    const char *name = ippGetName(attr);\n"
           "    ipp_tag_t value_tag = ippGetValueTag(attr);\n"
           "    ipp_tag_t group_tag = ippGetGroupTag(attr);\n"
           "    int count = ippGetCount(attr);\n"
           "\n"
           "    if (!name)\n"
           "      continue;\n", function);

    puts("  }\n"
         "\n"
         "  return (true);\n"
         "}");
  }

  return (0);
}


/*
 * 'parse_syntax()' - Parse a syntax string.
 */

static int				/* O - Number of values */
parse_syntax(const char  *syntax,	/* I - Syntax string */
             int         max_values,	/* I - Maximum number of values */
             reg_value_t *values)	/* I - Values */
{
  int	num_values = 0;			/* Number of values */
  bool	is_1setOf = false;		/* 1setOf syntax? */
  int	min_value,			/* Minimum value for syntax */
	max_value;			/* Maximum value for syntax */
  static const char *sepchars = " \t()|";
					/* Separator strings */


  while (*syntax && num_values < max_values)
  {
    while (strchr(sepchars, *syntax) && *syntax)
      syntax ++;

    if (!strncmp(syntax, "1setOf", 6) && strchr(sepchars, syntax[6]))
    {
      is_1setOf = true;
      syntax += 7;
      continue;
    }
    else if (!strncmp(syntax, "name", 4) && strchr(sepchars, syntax[4]))
    {
      values->value_tag = IPP_TAG_NAME;

      min_value = 0;
      max_value = 255;
      syntax    += 4;
    }
    else if (!strncmp(syntax, "text", 4) && strchr(sepchars, syntax[4]))
    {
      values->value_tag = IPP_TAG_TEXT;

      min_value = 0;
      max_value = 1023;
      syntax    += 4;
    }
    else if (!strncmp(syntax, "keyword", 7) && strchr(sepchars, syntax[7]))
    {
      values->value_tag = IPP_TAG_KEYWORD;

      min_value = 0;
      max_value = 255;
      syntax    += 7;
    }
    else if (!strncmp(syntax, "enum", 4) && strchr(sepchars, syntax[4]))
    {
      values->value_tag = IPP_TAG_ENUM;

      min_value = 0;
      max_value = INT_MAX;
      syntax    += 4;
    }
    else if (!strncmp(syntax, "integer", 7) && strchr(sepchars, syntax[7]))
    {
      values->value_tag = IPP_TAG_NAME;

      min_value = INT_MIN;
      max_value = INT_MAX;
      syntax    += 7;
    }
    else if (!strncmp(syntax, "rangeOfInteger", 14) && strchr(sepchars, syntax[14]))
    {
      values->value_tag = IPP_TAG_RANGE;

      min_value = INT_MIN;
      max_value = INT_MAX;
      syntax    += 14;
    }
    else if (!strncmp(syntax, "dateTime", 8) && strchr(sepchars, syntax[8]))
    {
      values->value_tag = IPP_TAG_DATE;

      min_value = 0;
      max_value = 0;
      syntax    += 8;
    }
    else if (!strncmp(syntax, "collection", 10) && strchr(sepchars, syntax[10]))
    {
      values->value_tag = IPP_TAG_BEGIN_COLLECTION;

      min_value = 0;
      max_value = 0;
      syntax    += 10;
    }
    else if (!strncmp(syntax, "boolean", 7) && strchr(sepchars, syntax[7]))
    {
      values->value_tag = IPP_TAG_BOOLEAN;

      min_value = 0;
      max_value = 0;
      syntax    += 7;
    }
    else if (!strncmp(syntax, "unknown", 7) && strchr(sepchars, syntax[7]))
    {
      values->value_tag = IPP_TAG_UNKNOWN;

      min_value = 0;
      max_value = 0;
      syntax    += 7;
    }
    else if (!strncmp(syntax, "no-value", 8) && strchr(sepchars, syntax[8]))
    {
      values->value_tag = IPP_TAG_BOOLEAN;

      min_value = 0;
      max_value = 0;
      syntax    += 7;
    }
    else if ((!strncmp(syntax, "type1", 5) || !strncmp(syntax, "type2", 5)) && strchr(sepchars, syntax[5]))
    {
      syntax += 5;
      continue;
    }
    else
    {
     /*
      * Unknown/bad syntax...
      */

      num_values = 0;
      break;
    }

    values->is_1setOf = is_1setOf;

    if (*syntax == '(')
    {
      if (min_value == 0 && max_value == 0)
      {
        num_values = 0;
        break;
      }
      else if (!strncmp(syntax, "(MAX)", 5))
      {
	syntax += 5;
	values->min_value = 0;
	values->max_value = max_value;
      }
      else if (!strncmp(syntax, "(MIN:MAX)", 9))
      {
	syntax += 9;
	values->min_value = min_value;
	values->max_value = max_value;
      }
      else
      {
       /*
	* Get (max) or (min:max)
	*/

	int limit = (int)strtol(syntax + 1, (char **)&syntax, 10);
				      /* Current limit */

	if (*syntax == ':')
	{
	 /*
	  * Get range...
	  */

	  values->min_value = limit;

	  if (!strncmp(syntax, ":MAX)", 5))
	  {
	    values->max_value = max_value;
	    syntax += 4;
	  }
	  else
	  {
	    values->max_value = (int)strtol(syntax + 1, (char **)&syntax, 10);
	  }

	  if (*syntax != ')')
	  {
	   /*
	    * Bad range...
	    */

	    num_values = 0;
	    break;
	  }
	}
	else if (*syntax == ')')
	{
	 /*
	  * Just an upper limit...
	  */

	  values->max_value = limit;
	}
	else
	{
	 /*
	  * Bad syntax...
	  */

	  num_values = 0;
	  break;
	}

	syntax ++;
      }
    }
    else
    {
      values->min_value = min_value;
      values->max_value = max_value;
    }

    num_values ++;
    values ++;
  }

  return (num_values);
}


/*
 * 'usage()' - Show program usage.
 */

static int				/* O - Exit status */
usage(FILE *fp)				/* I - Output file */
{
  fputs("\nUsage: ./regtocups filename.xml >filename.c\n", fp);
  return (1);
}
