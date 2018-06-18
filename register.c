/*
 * Program to add registration info to the IANA IPP registry.
 *
 * Usage:
 *
 *    ./register options filename.xml [regtext.txt]
 *
 * If no regtext.txt file is supplied, reads registration from stdin.
 *
 * Options:
 *
 *    -f			Force writing of new file
 *    -o newfile.xml		New XML file (instead of replacing filename.xml)
 *    -t "Standard Title"	Title of standard
 *    -x "Standard URL"		URL for standard (or rfcNNNN)
 *
 * Copyright © 2018 by The IEEE-ISTO Printer Working Group.
 * Copyright © 2008-2017 by Michael R Sweet
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 */

#include <mxml.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>


/* Common IPP registry stuff */
#include "ipp-registry.h"


/*
 * Constants...
 */

enum
{
  IN_NOTHING,				/* Nothing to process */
  IN_OPER_ATTRS,			/* Operation attributes */
  IN_PDESC_ATTRS,			/* Printer Description attributes */
  IN_PSTAT_ATTRS,			/* Printer Status attributes */
  IN_JDESC_ATTRS,			/* Job Description attributes */
  IN_JSTAT_ATTRS,			/* Job Status attributes */
  IN_JTMPL_ATTRS,			/* Job Template attributes */
  IN_DDESC_ATTRS,			/* Document Description attributes */
  IN_DSTAT_ATTRS,			/* Document Status attributes */
  IN_DTMPL_ATTRS,			/* Document Template attributes */
  IN_RDESC_ATTRS,			/* Resource Description attributes */
  IN_RSTAT_ATTRS,			/* Resource Status attributes */
  IN_SDESC_ATTRS,			/* Subscription Description attributes */
  IN_SSTAT_ATTRS,			/* Subscription Status attributes */
  IN_STMPL_ATTRS,			/* Subscription Template attributes */
  IN_SYSDESC_ATTRS,			/* System Description attributes */
  IN_SYSSTAT_ATTRS,			/* System Status attributes */
  IN_EVENT_ATTRS,			/* Event Notification attributes */
  IN_KEYWORD_VALS,			/* Keyword attribute values */
  IN_ENUM_VALS,				/* Enum Attribute values */
  IN_OPERATIONS,			/* Operations */
  IN_STATUS_CODES,			/* Status codes */
  IN_OBJECTS,                           /* Objects */
  IN_ATTR_GROUP_VALS,                   /* Attribute Group Values */
  IN_OUT_OF_BAND_VALS,                  /* Out-of-Band Values */
  IN_MAX
};


/*
 * Structures...
 */

typedef struct				/* Record for sorting */
{
  int		index;			/* Index (to detect changes) */
  mxml_node_t	*node;			/* Node for this record */
  const char	*keys[4];		/* Sorting keys */
} reg_record_t;


/*
 * Local functions...
 */

static int		add_attr(mxml_node_t *xml, const char *collection,
			         const char *attrname, const char *membername,
			         const char *submembername, const char *syntax,
			         const char *xref, const char *xrefname);
static int		add_attr_group(mxml_node_t *xml, const char *value, const char *name, const char *xref, const char *xrefname);
static int		add_object(mxml_node_t *xml, const char *name, const char *xref, const char *xrefname);
static int		add_operation(mxml_node_t *xml, const char *name,
			              const char *xref, const char *xrefname);
static int		add_out_of_band(mxml_node_t *xml, const char *value, const char *name, const char *xref, const char *xrefname);
static int		add_status_code(mxml_node_t *xml, const char *value,
			                const char *name, const char *xref,
			                const char *xrefname);
static int		add_value(mxml_node_t *xml, const char *enumval,
			          const char *keyword, const char *attrname,
			          const char *syntax, const char *xref,
			          const char *xrefname);
static int		add_valattr(mxml_node_t *xml, const char *registry,
			            const char *attrname, const char *syntax,
			            const char *xref, const char *xrefname);
static int		add_value(mxml_node_t *xml, const char *enumval,
			          const char *keyword, const char *attrname,
			          const char *syntax, const char *xref,
			          const char *xrefname);
static int		compare_record(reg_record_t *a, reg_record_t *b);
static int		compare_strings(const char *s, const char *t);
static int		read_text(mxml_node_t *xml, FILE *textfile,
			          const char *title,
			          const char *xref);
static const char	*save_cb(mxml_node_t *node, int column);
static int		usage(const char *opt);
static int		validate_registry(mxml_node_t *xml, const char *registry,
			                  const char *regname, int num_keys,
			                  const char * const *keys);
static const char	*xref_name(const char *xref, const char *title);


/*
 * 'main()' - Handle additions to the IANA registry.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line arguments */
{
  int		i;			/* Looping var */
  const char	*xmlin = NULL,		/* XML registration input file */
		*xmlout = NULL,		/* XML registration output file */
		*textin = NULL,		/* Text registration input file */
		*title = NULL,		/* Title for standard */
		*xref = NULL,		/* URL for standard (or rfcNNNN) */
		*opt;			/* Current option character */
  mxml_node_t	*xml;			/* XML registration file top node */
  char		xmlbackup[1024];	/* Backup file */
  FILE		*xmlfile,		/* XML registration file pointer */
		*textfile;		/* Text registration file */
  int		changed = 0;		/* Was the registration data changed? */
  static const char * const attribute_keys[] = { "collection", "name", "member_attribute", "sub-member_attribute" };
  static const char * const attribute_group_keys[] = { "value" };
  static const char * const keyword_keys[] = { "attribute", "value" };
  static const char * const enum_keys[] = { "attribute", "value" };
  static const char * const object_keys[] = { "name" };
  static const char * const operation_keys[] = { "name" };
  static const char * const out_of_band_keys[] = { "value" };
  static const char * const status_code_keys[] = { "value" };
					/* Sorting keys for the different registries */


 /*
  * Parse command-line arguments...
  */

  for (i = 1; i < argc; i ++)
  {
    if (argv[i][0] == '-')
    {
      for (opt = argv[i] + 1; *opt; opt ++)
      {
        switch (*opt)
        {
          case 'f' : /* Force output */
              changed = 1;
              break;

          case 'o' : /* Output file */
              if (xmlout)
              {
                fputs("Output file may only be specified once.\n", stderr);
                return (usage(NULL));
              }

              i ++;
              if (i >= argc)
              {
                fputs("Expected filename after '-o'.\n", stderr);
                return (usage(NULL));
              }

              xmlout = argv[i];
              break;

          case 't' : /* Standard title */
              if (title)
              {
                fputs("Title may only be specified once.\n", stderr);
                return (usage(NULL));
              }

              i ++;
              if (i >= argc)
              {
                fputs("Expected title after '-t'.\n", stderr);
                return (usage(NULL));
              }

              title = argv[i];
              break;

          case 'x' :	/* Standard URL or RFC number */
              if (xref)
              {
                fputs("URL may only be specified once.\n", stderr);
                return (usage(NULL));
              }

              i ++;
              if (i >= argc)
              {
                fputs("Expected URL or RFC number after '-x'.\n", stderr);
                return (usage(NULL));
              }

              if (strncmp(argv[i], "rfc", 3) && strncmp(argv[i], "http://", 7) && strncmp(argv[i], "https://", 8) && strncmp(argv[i], "ftp://", 6))
              {
                fputs("Standard URL must be 'rfcNNNN', 'ftp://...', 'http://...', or 'https://...'.\n", stderr);
                return (usage(NULL));
              }

              xref = argv[i];
              break;

          default :
              return (usage(opt));
        }
      }
    }
    else if (!xmlin)
      xmlin = argv[i];
    else if (!textin)
      textin = argv[i];
    else
    {
      fprintf(stderr, "Unknown argument '%s'.\n", argv[i]);
      return (usage(NULL));
    }
  }

  if (!title)
    title = "TEST";

  if (!xref)
    xref = "test";

  if (!xmlin)
  {
    fputs("Missing required input file.\n", stderr);
    return (usage(NULL));
  }

  if (!xmlout)
    xmlout = xmlin;

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
  * Verify that the XML file is well-formed...
  */

  changed |= validate_registry(xml, IPP_REGISTRY_OBJECTS, "Objects",
                               (int)(sizeof(object_keys) /
                                     sizeof(object_keys[0])),
                               object_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_ATTRIBUTES, "Attributes",
                               (int)(sizeof(attribute_keys) /
                                     sizeof(attribute_keys[0])),
			       attribute_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_KEYWORDS, "Keyword Values",
                               (int)(sizeof(keyword_keys) /
                                     sizeof(keyword_keys[0])),
			       keyword_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_ENUMS, "Enum Values",
                               (int)(sizeof(enum_keys) /
                                     sizeof(enum_keys[0])),
			       enum_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_ATTRIBUTE_GROUPS, "Attribute Groups",
                               (int)(sizeof(attribute_group_keys) /
                                     sizeof(attribute_group_keys[0])),
			       attribute_group_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_OUT_OF_BANDS, "Out-of-Band Values",
                               (int)(sizeof(out_of_band_keys) /
                                     sizeof(out_of_band_keys[0])),
			       out_of_band_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_OPERATIONS, "Operations",
                               (int)(sizeof(operation_keys) /
                                     sizeof(operation_keys[0])),
			       operation_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_STATUS_CODES, "Status Codes",
                               (int)(sizeof(status_code_keys) /
                                     sizeof(status_code_keys[0])),
			       status_code_keys);

 /*
  * Read the text registrations...
  */

  if (textin)
    textfile = fopen(textin, "rb");
  else
    textfile = stdin;

  changed |= read_text(xml, textfile, title, xref);

  if (textfile != stdin)
    fclose(textfile);

 /*
  * Save the XML registration file if there were changes...
  */

  if (changed)
  {
    mxml_node_t	*updated = mxmlFindElement(xml, xml, "updated", NULL, NULL, MXML_DESCEND);

    if (updated)
    {
      time_t curtime = time(NULL);	/* Current time in seconds */
      struct tm *curdate = gmtime(&curtime);
					/* Current date in UTC */
      char datestr[64];			/* Date string YYYY-MM-DD */

      strftime(datestr, sizeof(datestr), "%Y-%m-%d", curdate);

      mxmlSetOpaque(updated, datestr);
    }

    if (xmlin == xmlout)
    {
      snprintf(xmlbackup, sizeof(xmlbackup), "%s.O", xmlin);
      if (rename(xmlin, xmlbackup))
      {
        perror(xmlin);
        return (1);
      }
    }

    if ((xmlfile = fopen(xmlout, "wb")) == NULL)
    {
      perror(xmlout);
      if (xmlin == xmlout)
        rename(xmlbackup, xmlin);
      return (1);
    }

    i = mxmlSaveFile(xml, xmlfile, save_cb);
    fclose(xmlfile);

    if (i)
    {
      perror(xmlout);
      if (xmlin == xmlout)
        rename(xmlbackup, xmlin);
      return (1);
    }
    else
      fprintf(stderr, "register: Wrote '%s'.\n", xmlout);
  }
  else
    fputs("register: No changes written.\n", stderr);

  return (!changed);
}


/*
 * 'add_attr()' - Add a (member) attribute in the right position of the XML registry.
 */

static int				/* O - 1 if something was added, 0 if not */
add_attr(mxml_node_t *xml,		/* I - XML registry */
         const char  *collection,	/* I - Collection within the attributes registry */
	 const char  *attrname,		/* I - Attribute name */
	 const char  *membername,	/* I - Member attribute name or NULL */
	 const char  *submembername,	/* I - Sub-member attribute name or NULL */
	 const char  *syntax,		/* I - Syntax string */
	 const char  *xref,		/* I - Standard URL/number */
	 const char  *xrefname)		/* I - Standard name or NULL */
{
  int		changed = 0,		/* Did something change? */
		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*collection_node,	/* <collection> */
		*name_node,		/* <name> */
		*membername_node,	/* <member_attribute> */
		*submembername_node,	/* <sub-member_attribute> */
		*syntax_node,		/* <syntax> */
		*xref_node;		/* <xref> */
  const char	*last_collection = NULL,/* Last collection */
		*last_name = NULL,	/* Last attribute name */
		*last_member = NULL,	/* Last member attribute */
		*last_submember = NULL;	/* Last sub-member attribute */
  char		extname[256];		/* attribute-name(extension) */


 /*
  * Find any existing definition of the attribute...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_ATTRIBUTES "' registry in XML file.\n",
          stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    collection_node = mxmlFindElement(record_node, record_node, "collection", NULL, NULL,
                                      MXML_DESCEND_FIRST);
    if (!collection_node || !mxmlGetOpaque(collection_node))
    {
      fputs("register: Attribute record missing <collection> in XML file.\n", stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if (last_collection)
    {
      if (compare_strings(last_collection, mxmlGetOpaque(collection_node)))
      {
        last_name = last_member = last_submember = NULL;

	if (!compare_strings(last_collection, collection))
	  break;
      }
    }

    last_collection = mxmlGetOpaque(collection_node);
    if (compare_strings(last_collection, collection))
      continue;

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
				MXML_DESCEND_FIRST);
    if (!name_node)
    {
      name_node = mxmlFindElement(record_node, record_node, "attribute", NULL,
                                  NULL, MXML_DESCEND_FIRST);

      if (!name_node)
      {
	fputs("register: Attribute record missing <name> in XML file.\n", stderr);
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }
      else
      {
        fprintf(stderr, "register: Fixing attribute record for %s.\n",
                mxmlGetOpaque(name_node));
        mxmlSetElement(name_node, "name");
        changed = 1;
      }
    }

    if (last_name)
    {
      if ((result = compare_strings(last_name, mxmlGetOpaque(name_node))) > 0)
      {
	fputs("register: Attribute record out-of-order in XML file.\n", stderr);
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }

      if (result)
        last_member = last_submember = NULL;
    }

    last_name = mxmlGetOpaque(name_node);
    if ((result = compare_strings(attrname, last_name)) > 0)
      continue;
    else if (result < 0)
      break;

    membername_node = mxmlFindElement(record_node, record_node, "member_attribute",
				      NULL, NULL, MXML_DESCEND_FIRST);
    if (last_member && membername_node && mxmlGetOpaque(membername_node))
    {
      if ((result = compare_strings(last_member, mxmlGetOpaque(membername_node))) > 0)
      {
	fputs("register: Attribute record out-of-order in XML file.\n", stderr);
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }

      if (result)
        last_submember = NULL;
    }

    last_member = mxmlGetOpaque(membername_node);
    if (membername && !last_member)
    {
      result = -1;
      continue;
    }
    else if (last_member && !membername)
    {
      result = 1;
      break;
    }
    else if (membername)
    {
      if ((result = compare_strings(membername, last_member)) > 0)
        continue;
      else if (result < 0)
        break;
    }

    submembername_node = mxmlFindElement(record_node, record_node, "sub-member_attribute",
					 NULL, NULL, MXML_DESCEND_FIRST);
    if (last_submember && submembername_node && mxmlGetOpaque(submembername_node))
    {
      if (compare_strings(last_submember, mxmlGetOpaque(submembername_node)) > 0)
      {
	fputs("register: Attribute record out-of-order in XML file.\n", stderr);
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }
    }

    last_submember = mxmlGetOpaque(submembername_node);
    if (submembername && !last_submember)
    {
      result = -1;
      continue;
    }
    else if (last_submember && !submembername)
    {
      result = 1;
      break;
    }
    else if (submembername)
    {
      if ((result = compare_strings(submembername, last_submember)) > 0)
        continue;
      else if (result < 0)
        break;
    }

   /*
    * OK, at this point the current record matches the attribute we are trying
    * to add.  If the syntax doesn't match, treat this as an extension to the
    * original definition and change the attribute or member name to
    * "name(extension)" and add it below...
    */

    syntax_node = mxmlFindElement(record_node, record_node, "syntax",
				  NULL, NULL, MXML_DESCEND_FIRST);
    if (!syntax_node)
    {
      fputs("register: Attribute record missing <syntax> in XML file.\n", stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if (compare_strings(mxmlGetOpaque(syntax_node), syntax))
    {
      fprintf(stderr, "register: Extending syntax for %s to '%s'.\n",
              submembername ? submembername : membername ? membername : attrname, syntax);

      if (submembername)
      {
        snprintf(extname, sizeof(extname), "%s(extension)", submembername);
        submembername = extname;
      }
      else if (membername)
      {
        snprintf(extname, sizeof(extname), "%s(extension)", membername);
        membername = extname;
      }
      else
      {
        snprintf(extname, sizeof(extname), "%s(extension)", attrname);
        attrname = extname;
      }
    }
    else
      return (changed);
  }

 /*
  * If we get here the attribute does not exist.  Build a new record and add it
  * to the registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  collection_node = mxmlNewElement(node, "collection");
  mxmlNewOpaque(collection_node, collection);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, attrname);

  if (membername)
  {
    membername_node = mxmlNewElement(node, "member_attribute");
    mxmlNewOpaque(membername_node, membername);

    if (submembername)
    {
      submembername_node = mxmlNewElement(node, "sub-member_attribute");
      mxmlNewOpaque(submembername_node, submembername);
    }
  }

  syntax_node = mxmlNewElement(node, "syntax");
  mxmlNewOpaque(syntax_node, syntax);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node,
          node);

  return (1);
}


/*
 * 'add_attr_group()' - Add an attribute group value to the registry.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_attr_group(mxml_node_t *xml,	/* I - XML registry */
               const char  *value,	/* I - Attribute group value */
               const char  *name,	/* I - Attribute group name */
               const char  *xref,	/* I - Standard URL/number */
               const char  *xrefname)	/* I - Standard name or NULL */
{
  int		changed = 0,		/* Did something change? */
		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*name_node,		/* <name> */
		*value_node,		/* <value> */
		*xref_node;		/* <xref> */
  char		*nodeval;		/* Pointer into node value */


 /*
  * Find any existing definition of the attribute...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
				       IPP_REGISTRY_ATTRIBUTE_GROUPS,
				       MXML_DESCEND)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_ATTRIBUTE_GROUPS
          "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL,
				 MXML_DESCEND_FIRST);
    if (!value_node)
    {
      fputs("register: Status code record missing <value> in XML file.\n",
            stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if ((result = strtol(value, NULL, 0) -
                  strtol(mxmlGetOpaque(value_node), &nodeval, 0)) > 0)
      continue;
    else if (result < 0)
      break;

    if (nodeval && *nodeval == '-')
    {
     /*
      * 0xXXXX-0xXXXX range; update the range to be 1 more than the current
      * value and then insert the new status code before this one...
      */

      char	newvalue[256];		/* New value */

      snprintf(newvalue, sizeof(newvalue), "0x%04x%s", (unsigned)strtol(value, NULL, 0) + 1, nodeval);
      mxmlSetOpaque(value_node, newvalue);
      changed = 1;
      break;
    }

   /*
    * OK, at this point the current record matches the value we are trying to add.
    */

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
                                MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fprintf(stderr, "register: Fixing record for attribute group '%s'.\n",
              value);
      name_node = mxmlNewElement(record_node, "name");
    }

    if (!mxmlGetOpaque(name_node))
    {
      mxmlNewOpaque(name_node, name);
      return (1);
    }
    else if (compare_strings(name, mxmlGetOpaque(name_node)))
    {
      fprintf(stderr, "register: Updating record for attribute group '%s'.\n",
              value);
      mxmlSetOpaque(name_node, name);
      return (1);
    }
    else
      return (0);
  }

 /*
  * If we get here the value does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, value);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node,
          node);

  return (1);
}


/*
 * 'add_valattr()' - Add a keyword/enum attribute definition.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_valattr(mxml_node_t *xml,		/* I - XML registry */
            const char  *registry,	/* I - Registry ID */
            const char  *attrname,	/* I - Attribute name */
            const char  *syntax,	/* I - Attribute syntax */
            const char  *xref,		/* I - Standard URL/number */
            const char  *xrefname)	/* I - Standard name or NULL */
{
  int		changed = 0,		/* Did something change? */
		compare_enums,		/* Compare enum values? */
		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*attr_node,		/* <attribute> */
		*syntax_node,		/* <syntax> */
		*xref_node;		/* <xref> */
  const char	*last_attr = NULL;	/* Last attribute name */


 /*
  * Find any existing definition of the attribute...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", registry,
                                       MXML_DESCEND)) == NULL)
  {
    fprintf(stderr, "register: Unable to find '%s' registry in XML file.\n",
            registry);
    exit(1);
  }

  compare_enums = !strcmp(registry, IPP_REGISTRY_ENUMS);

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    attr_node = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL,
				MXML_DESCEND_FIRST);
    if (!attr_node)
    {
      attr_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
				  MXML_DESCEND_FIRST);

      if (!attr_node)
      {
	fprintf(stderr, "register: %s record missing <attribute> in XML file.\n",
	        compare_enums ? "enum" : "keyword");
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }
      else
      {
        fprintf(stderr, "register: Fixing %s record for %s.\n",
                compare_enums ? "enum" : "keyword", mxmlGetOpaque(attr_node));
        mxmlSetElement(attr_node, "attribute");
        changed = 1;
      }
    }

    if (last_attr && mxmlGetOpaque(attr_node))
    {
      if (compare_strings(last_attr, mxmlGetOpaque(attr_node)) > 0)
      {
	fprintf(stderr, "register: %s record out-of-order in XML file.\n",
	        compare_enums ? "Enum" : "Keyword");
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }
    }

    if ((result = compare_strings(attrname, mxmlGetOpaque(attr_node))) > 0)
      continue;
    else if (result < 0)
      break;

    if (mxmlFindElement(record_node, record_node, "value", NULL, NULL,
			MXML_DESCEND_FIRST))
      break;

   /*
    * OK, at this point the current record matches the attribute we are trying to add.
    * If the syntax doesn't match issue a warning and reset the syntax, otherwise just
    * return with no changes...
    */

    syntax_node = mxmlFindElement(record_node, record_node, "syntax",
				  NULL, NULL, MXML_DESCEND_FIRST);
    if (!syntax_node)
    {
      fputs("register: Attribute record missing <syntax> in XML file.\n", stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if (compare_strings(mxmlGetOpaque(syntax_node), syntax))
    {
      fprintf(stderr, "register: Changing syntax for %s to '%s'.\n", attrname, syntax);
      mxmlSetOpaque(syntax_node, syntax);
      return (1);
    }
    else
      return (0);
  }

 /*
  * If we get here the attribute does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  attr_node = mxmlNewElement(node, "attribute");
  mxmlNewOpaque(attr_node, attrname);

  syntax_node = mxmlNewElement(node, "syntax");
  mxmlNewOpaque(syntax_node, syntax);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node,
          node);

  return (1);
}


/*
 * 'add_object()' - Add an object to the registry.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_object(mxml_node_t *xml,		/* I - XML registry */
           const char  *name,   	/* I - Object name */
           const char  *xref,   	/* I - Standard URL/number */
           const char  *xrefname)	/* I - Standard name or NULL */
{
  int		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*name_node,		/* <name> */
		*xref_node;		/* <xref> */


 /*
  * Find any existing definition of the operation...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_OBJECTS, MXML_DESCEND)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_OBJECTS "' registry in XML file.\n",
          stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
				MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fputs("register: Object record missing <name> in XML file.\n", stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if ((result = compare_strings(name, mxmlGetOpaque(name_node))) == 0)
      return (0);
    else if (result < 0)
      break;
  }

 /*
  * If we get here the value does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER,
          record_node, node);

  return (1);
}


/*
 * 'add_operation()' - Add an operation to the registry.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_operation(mxml_node_t *xml,		/* I - XML registry */
              const char  *name,	/* I - Operation name */
	      const char  *xref,	/* I - Standard URL/number */
	      const char  *xrefname)	/* I - Standard name or NULL */
{
  int		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*name_node,		/* <name> */
		*xref_node;		/* <xref> */


 /*
  * Find any existing definition of the operation...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_OPERATIONS, MXML_DESCEND)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_OPERATIONS "' registry in XML file.\n",
          stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
				MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fputs("register: Operation record missing <name> in XML file.\n",
            stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if ((result = compare_strings(name, mxmlGetOpaque(name_node))) == 0)
      return (0);
    else if (result < 0)
      break;
  }

 /*
  * If we get here the value does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER,
          record_node, node);

  return (1);
}


/*
 * 'add_out_of_band()' - Add an out-of-band value to the registry.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_out_of_band(mxml_node_t *xml,	/* I - XML registry */
                const char  *value,	/* I - Out-of-band value */
	        const char  *name,	/* I - Out-of-band name */
		const char  *xref,	/* I - Standard URL/number */
		const char  *xrefname)	/* I - Standard name or NULL */
{
  int		changed = 0,		/* Did something change? */
		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*name_node,		/* <name> */
		*value_node,		/* <value> */
		*xref_node;		/* <xref> */
  char		*nodeval;		/* Pointer into node value */


 /*
  * Find any existing definition of the attribute...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
				       IPP_REGISTRY_OUT_OF_BANDS,
				       MXML_DESCEND)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_OUT_OF_BANDS
          "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL,
				 MXML_DESCEND_FIRST);
    if (!value_node)
    {
      fputs("register: Out-of-band record missing <value> in XML file.\n",
            stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if ((result = strtol(value, NULL, 0) -
                  strtol(mxmlGetOpaque(value_node), &nodeval, 0)) > 0)
      continue;
    else if (result < 0)
      break;

    if (nodeval && *nodeval == '-')
    {
     /*
      * 0xXXXX-0xXXXX range; update the range to be 1 more than the current
      * value and then insert the new out-of-band value before this one...
      */

      char	newvalue[256];		/* New value */

      snprintf(newvalue, sizeof(newvalue), "0x%04x%s", (unsigned)strtol(value, NULL, 0) + 1, nodeval);
      mxmlSetOpaque(value_node, newvalue);
      changed = 1;
      break;
    }

   /*
    * OK, at this point the current record matches the value we are trying to add.
    */

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
                                MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fprintf(stderr, "register: Fixing record for out-of-band value '%s'.\n",
              value);
      name_node = mxmlNewElement(record_node, "name");
    }

    if (!mxmlGetOpaque(name_node))
    {
      mxmlNewOpaque(name_node, name);
      return (1);
    }
    else if (compare_strings(name, mxmlGetOpaque(name_node)))
    {
      fprintf(stderr, "register: Updating record for out-of-band value '%s'.\n",
              value);
      mxmlSetOpaque(name_node, name);
      return (1);
    }
    else
      return (0);
  }

 /*
  * If we get here the value does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, value);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node,
          node);

  return (1);
}


/*
 * 'add_status_code()' - Add a status code to the registry.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_status_code(mxml_node_t *xml,	/* I - XML registry */
                const char  *value,	/* I - Status code value */
	        const char  *name,	/* I - Status code name */
		const char  *xref,	/* I - Standard URL/number */
		const char  *xrefname)	/* I - Standard name or NULL */
{
  int		changed = 0,		/* Did something change? */
		result;			/* Result of comparison */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*name_node,		/* <name> */
		*value_node,		/* <value> */
		*xref_node;		/* <xref> */
  char		*nodeval;		/* Pointer into node value */


 /*
  * Find any existing definition of the attribute...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
				       IPP_REGISTRY_STATUS_CODES,
				       MXML_DESCEND)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_STATUS_CODES
          "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL,
				 MXML_DESCEND_FIRST);
    if (!value_node)
    {
      fputs("register: Status code record missing <value> in XML file.\n",
            stderr);
      mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
      exit(1);
    }

    if ((result = strtol(value, NULL, 0) -
                  strtol(mxmlGetOpaque(value_node), &nodeval, 0)) > 0)
      continue;
    else if (result < 0)
      break;

    if (nodeval && *nodeval == '-')
    {
     /*
      * 0xXXXX-0xXXXX range; update the range to be 1 more than the current
      * value and then insert the new status code before this one...
      */

      char	newvalue[256];		/* New value */

      snprintf(newvalue, sizeof(newvalue), "0x%04x%s", (unsigned)strtol(value, NULL, 0) + 1, nodeval);
      mxmlSetOpaque(value_node, newvalue);
      changed = 1;
      break;
    }

   /*
    * OK, at this point the current record matches the value we are trying to add.
    */

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
                                MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fprintf(stderr, "register: Fixing record for status code '%s'.\n",
              value);
      name_node = mxmlNewElement(record_node, "name");
    }

    if (!mxmlGetOpaque(name_node))
    {
      mxmlNewOpaque(name_node, name);
      return (1);
    }
    else if (compare_strings(name, mxmlGetOpaque(name_node)))
    {
      fprintf(stderr, "register: Updating record for status code '%s'.\n",
              value);
      mxmlSetOpaque(name_node, name);
      return (1);
    }
    else
      return (0);
  }

 /*
  * If we get here the value does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, value);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node,
          node);

  return (1);
}


/*
 * 'add_value()' - Add a keyword/enum value to the registry.
 */

static int				/* O - 1 if something changed, 0 otherwise */
add_value(mxml_node_t *xml,		/* I - XML registry */
          const char  *enumval,		/* I - Enumeration value or NULL */
          const char  *keyword,		/* I - Keyword */
	  const char  *attrname,	/* I - Attribute name */
	  const char  *syntax,		/* I - Attribute syntax */
	  const char  *xref,		/* I - Standard URL/number */
	  const char  *xrefname)	/* I - Standard name or NULL */
{
  int		changed = 0,		/* Did something change? */
		result;			/* Result of comparison */
  const char	*registry,		/* Registry */
		*name;			/* <name> value */
  mxml_node_t	*node,			/* New <registry> node */
		*registry_node,		/* <registry> */
		*record_node,		/* <record> */
		*attr_node,		/* <attribute> */
		*syntax_node,		/* <syntax> */
		*name_node,		/* <name> */
		*value_node,		/* <value> */
		*xref_node;		/* <xref> */


 /*
  * Find any existing definition of the attribute...
  */

  registry = enumval ? IPP_REGISTRY_ENUMS : IPP_REGISTRY_KEYWORDS;

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", registry,
				       MXML_DESCEND)) == NULL)
  {
    fprintf(stderr, "register: Unable to find '%s' registry in XML file.\n",
            registry);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST);
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND))
  {
    attr_node = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL,
				MXML_DESCEND_FIRST);
    if (!attr_node)
    {
      attr_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
				  MXML_DESCEND_FIRST);

      if (!attr_node)
      {
	fprintf(stderr, "register: %s record missing <attribute> in XML file.\n",
		enumval ? "enum" : "keyword");
	mxmlSaveFile(record_node, stderr, MXML_NO_CALLBACK);
	exit(1);
      }
      else
      {
        fprintf(stderr, "register: Fixing %s record for %s.\n",
                enumval ? "enum" : "keyword", mxmlGetOpaque(attr_node));
        mxmlSetElement(attr_node, "attribute");
        changed = 1;
      }
    }

    if ((result = compare_strings(attrname, mxmlGetOpaque(attr_node))) > 0)
      continue;
    else if (result < 0)
      break;

    if ((value_node = mxmlFindElement(record_node, record_node, "value", NULL,
                                      NULL, MXML_DESCEND_FIRST)) == NULL)
      continue;
    else if (enumval)
    {
      if ((result = compare_strings(enumval,
                                    mxmlGetOpaque(value_node))) > 0)
        continue;
      else if (result < 0)
        break;
      else if (result == 0 && strstr(keyword, "(deprecated)") != NULL)
      {
	record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_NO_DESCEND);
        break;
      }
    }
    else if ((result = compare_strings(keyword, mxmlGetOpaque(value_node))) > 0)
      continue;
    else if (result < 0)
      break;

   /*
    * OK, at this point the current record matches the value we are trying to add.
    */

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL,
                                MXML_DESCEND_FIRST);
    name      = mxmlGetOpaque(name_node);

    if (enumval)
    {
     /*
      * Replace the name with the new value...
      */

      if (!name_node)
      {
        fprintf(stderr, "register: Fixing bad enum record '%s' for attribute %s.\n",
                enumval, attrname);
        name_node = mxmlNewElement(record_node, "name");
      }

      if (!name)
        mxmlNewOpaque(name_node, keyword);
      else
      {
        fprintf(stderr, "register: Renaming keyword for enum record '%s' for attribute %s.\n", enumval, attrname);
        mxmlSetOpaque(name_node, keyword);
      }

      return (1);
    }

    fprintf(stderr, "register: Duplicate %s value '%s' for attribute %s.\n",
            enumval ? "enum" : "keyword", enumval ? enumval : keyword, attrname);
    return (changed);
  }

 /*
  * If we get here the value does not exist.  Build a new record and add it to the
  * registry in the right place.
  */

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, enumval ? enumval : keyword);

  if (enumval)
  {
    name_node = mxmlNewElement(node, "name");
    mxmlNewOpaque(name_node, keyword);
  }
  else if (!strcmp(attrname, "media"))
  {
   /*
    * All new media registrations are for size names now...
    */

    mxml_node_t *type_node = mxmlNewElement(node, "type");
    mxmlNewOpaque(type_node, "size name");
  }

  attr_node = mxmlNewElement(node, "attribute");
  mxmlNewOpaque(attr_node, attrname);

  syntax_node = mxmlNewElement(node, "syntax");
  mxmlNewOpaque(syntax_node, syntax);

  xref_node = mxmlNewElement(node, "xref");
  mxmlElementSetAttr(xref_node, "data", xref);
  if (xrefname)
  {
    mxmlElementSetAttr(xref_node, "type", "uri");
    mxmlNewOpaque(xref_node, xrefname);
  }
  else
    mxmlElementSetAttr(xref_node, "type", "rfc");

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node,
          node);

  return (1);
}


/*
 * 'compare_record()' - Compare two registry records...
 */

static int				/* O - Result of comparison */
compare_record(reg_record_t *a,		/* I - First record */
               reg_record_t *b)		/* I - Second record */
{
  int	i,				/* Looping var */
	result;				/* Result of comparison */


  for (i = 0; i < (int)(sizeof(a->keys) / sizeof(a->keys[0])); i ++)
    if ((result = compare_strings(a->keys[i], b->keys[i])) != 0)
      return (result);

  return (0);
}


/*
 * 'compare_strings()' - Compare two strings.
 *
 * This function basically does a strcmp() of the two strings, but is also
 * aware of numbers so that "a2" < "a100".
 */

static int				/* O - Result of comparison */
compare_strings(const char *s,		/* I - First string */
                const char *t)		/* I - Second string */
{
  int		diff;			/* Difference between digits */


  if (!s)
  {
    if (t)
      return (-1);
    else
      return (0);
  }
  else if (!t)
    return (1);

 /*
  * Loop through both names, returning only when a difference is
  * seen.  Also, compare whole numbers rather than just characters, too!
  */

  while (*s && *t)
  {
    if (isdigit(*s & 255) && isdigit(*t & 255))
    {
     /*
      * Got numbers; scan the numbers and compare...
      */

      int sn = (int)strtol(s, (char **)&s, 0);
      int tn = (int)strtol(t, (char **)&t, 0);

      if ((diff = sn - tn) != 0)
        return (diff);
    }
    else if ((diff = *s - *t) != 0)
      return (diff);
    else
    {
      s ++;
      t ++;
    }
  }

 /*
  * Return the results of the final comparison...
  */

  if (*s)
    return (1);
  else if (*t)
    return (-1);
  else
    return (0);
}


/*
 * 'read_text()' - Read text registration data from the file.
 */

static int				/* O - 1 if something was added, 0 if not */
read_text(mxml_node_t *xml,		/* I - XML registration document */
          FILE        *textfile,	/* I - Text registration file */
          const char  *title,		/* I - Standard title */
          const char  *xref)		/* I - Standard URL */
{
  int		i,			/* Looping var */
                blanks = 0,             /* Number of blank lines */
		changed = 0,		/* Was something changed? */
		state = IN_NOTHING,	/* Current state */
		linenum = 0,		/* Current line number */
		paren = 0;		/* Parenthesis level */
  char		line[1024],		/* Line from text registration file */
		*ptr,			/* Pointer into line */
		*first,			/* First value */
		*second,		/* Second value */
		attrname[128],		/* Current attribute name */
		membername[128],	/* Current member attribute name */
		syntax[128];		/* Current syntax */
  const char	*xrefname;		/* Standard number */
  static struct
  {
    const char	*header;		/* Header string */
    size_t	headerlen;		/* Header length */
  }	states[] =	/* Headers for each group */
  {
    { NULL, 0 },
    { "Operation attributes:", 21 },
    { "Printer Description attributes:", 31 },
    { "Printer Status attributes:", 26 },
    { "Job Description attributes:", 27 },
    { "Job Status attributes:", 22 },
    { "Job Template attributes:", 24 },
    { "Document Description attributes:", 32 },
    { "Document Status attributes:", 27 },
    { "Document Template attributes:", 29 },
    { "Resource Description attributes:", 32 },
    { "Resource Status attributes:", 27 },
    { "Subscription Description attributes:", 36 },
    { "Subscription Status attributes:", 31 },
    { "Subscription Template attributes:", 33 },
    { "System Description attributes:", 30 },
    { "System Status attributes:", 15 },
    { "Event Notification attributes:", 30 },
    { "Keyword Attribute Value", 23 },
    { "Enum Value", 10 },
    { "Operation Name", 14 },
    { "Value    Status Code Name", 25 },
    { "Object Name", 11 },
    { "Attribute Group Value", 21 },
    { "Out-of-Band Value", 17 }
  };
  static const char * const collections[] =
  {					/* Collection names for each state */
    NULL,
    "Operation",
    "Printer Description",
    "Printer Status",
    "Job Description",
    "Job Status",
    "Job Template",
    "Document Description",
    "Document Status",
    "Document Template",
    "Resource Description",
    "Resource Status",
    "Subscription Description",
    "Subscription Status",
    "Subscription Template",
    "System Description",
    "System Status",
    "Event Notification",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  };


 /*
  * Read lines from the text registration file and make changes as needed...
  */

  memset(attrname, 0, sizeof(attrname));
  memset(membername, 0, sizeof(membername));
  memset(syntax, 0, sizeof(syntax));

  xrefname = xref_name(xref, title);

  while (fgets(line, sizeof(line), textfile))
  {
    linenum ++;

   /*
    * Strip leading and trailing whitespace...
    */

    ptr = line + strlen(line) - 1;
    while (ptr >= line)
      if (isspace(*ptr & 255))
        *ptr-- = '\0';
      else
        break;

    for (ptr = line; *ptr && isspace(*ptr & 255); ptr ++);

   /*
    * Skip blank lines and lines that start with "-"...
    */

    if (!*ptr)
    {
      blanks ++;
      if (blanks > 1)
        state = IN_NOTHING;

      continue;
    }
    else if (*ptr == '-')
      continue;

    blanks = 0;

   /*
    * Look for section headings...
    */

    for (i = IN_OPER_ATTRS; i < IN_MAX; i ++)
      if (!strncmp(ptr, states[i].header, states[i].headerlen))
      {
	state         = i;
	attrname[0]   = '\0';
	membername[0] = '\0';
	syntax[0]     = '\0';
	break;
      }

    if (i >= IN_MAX)
    {
      switch (state)
      {
        case IN_OPER_ATTRS :
        case IN_PDESC_ATTRS :
        case IN_PSTAT_ATTRS :
        case IN_JDESC_ATTRS :
        case IN_JSTAT_ATTRS :
        case IN_JTMPL_ATTRS :
        case IN_DDESC_ATTRS :
        case IN_DSTAT_ATTRS :
        case IN_DTMPL_ATTRS :
        case IN_RDESC_ATTRS :
        case IN_RSTAT_ATTRS :
        case IN_SDESC_ATTRS :
        case IN_SSTAT_ATTRS :
        case IN_STMPL_ATTRS :
        case IN_SYSDESC_ATTRS :
        case IN_SYSSTAT_ATTRS :
        case IN_EVENT_ATTRS :
            if (!islower(*ptr & 255))
              continue;

           /*
            * Attributes are not indented, member attributes *are*.
            */

            first = ptr;
            while (*ptr && !isspace(*ptr & 255))
              ptr ++;
            if (!*ptr)
            {
              fprintf(stderr, "register: Bad attribute definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (isspace(*ptr & 255))
              *ptr++ = '\0';

            if (*ptr != '(')
            {
              fprintf(stderr, "register: Bad attribute definition on line %d.\n",
                      linenum);
              exit(1);
            }

            ptr ++;
            second = ptr;
            for (paren = 1; *ptr && paren > 0; ptr ++)
            {
              if (*ptr == '(')
                paren ++;
              else if (*ptr == ')')
              {
                paren --;
                if (paren == 0)
                  *ptr = '\0';
              }
            }

            if (first == line)
            {
             /*
              * attribute syntax
              */

              fprintf(stderr, "Attribute \"%s (%s)\"\n", first, second);
              changed |= add_attr(xml, collections[state], first, NULL, NULL, second,
                                  xref, xrefname);

              strncpy(attrname, first, sizeof(attrname));
            }
            else if ((first - 2) == line)
            {
             /*
              * member syntax
              */

              if (!attrname[0])
              {
		fprintf(stderr, "register: Bad member attribute definition on line %d.\n",
			linenum);
		exit(1);
              }

              fprintf(stderr, "Member attribute \"%s.%s (%s)\"\n", attrname, first,
                      second);

              changed |= add_attr(xml, collections[state], attrname, first, NULL,
                                  second, xref, xrefname);

              strncpy(membername, first, sizeof(membername));
            }
            else
            {
             /*
              * sub-member syntax
              */

              if (!attrname[0] || !membername[0])
              {
		fprintf(stderr,
		        "register: Bad sub-memeber attribute definition on line %d.\n",
			linenum);
		exit(1);
              }

              fprintf(stderr, "Sub-member attribute \"%s.%s.%s (%s)\"\n", attrname,
                      membername, first, second);

              changed |= add_attr(xml, collections[state], attrname, membername, first,
                                  second, xref, xrefname);
            }
            break;

        case IN_KEYWORD_VALS :
            if (*ptr != '<' && !islower(*ptr & 255))
              continue;

           /*
            * Attributes are not indented, keywords *are*.
            */

            first = ptr;
            if (*ptr == '<')
            {
              while (*ptr && *ptr != '>')
                ptr ++;

              if (*ptr == '>')
                ptr ++;
            }
            else
            {
	      while (*ptr && !isspace(*ptr & 255))
		ptr ++;
            }

            if (!*ptr && first == line)
            {
              fprintf(stderr, "register: Bad attribute/keyword definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (isspace(*ptr & 255))
              *ptr++ = '\0';

            if (first == line)
            {
	      if (*ptr != '(')
	      {
		fprintf(stderr, "register: Bad attribute definition on line %d.\n",
			linenum);
		exit(1);
	      }

	      ptr ++;
	      second = ptr;
	      for (paren = 1; *ptr && paren > 0; ptr ++)
	      {
		if (*ptr == '(')
		  paren ++;
		else if (*ptr == ')')
		{
		  paren --;
		  if (paren == 0)
		    *ptr = '\0';
		}
	      }

             /*
              * attribute syntax
              */

              fprintf(stderr, "Attribute \"%s (%s)\"\n", first, second);
              changed |= add_valattr(xml, IPP_REGISTRY_KEYWORDS, first, second, xref, xrefname);

              strncpy(attrname, first, sizeof(attrname));
              strncpy(syntax, second, sizeof(syntax));
            }
            else if (!attrname[0])
            {
              fprintf(stderr, "Missing attribute before keyword value on line %d.\n",
                      linenum);
              exit(1);
            }
            else
            {
             /*
              * keyword
              */

              fprintf(stderr, "Keyword '%s' \"%s (%s)\"\n", first, attrname, syntax);

              changed |= add_value(xml, NULL, first, attrname, syntax, xref, xrefname);
            }
            break;

        case IN_ENUM_VALS :
            if (*ptr != '<' && !islower(line[0] & 255) && !isdigit(*ptr & 255))
              continue;

           /*
            * Attributes are not indented, enums *are*.
            */

            first = ptr;
            if (*ptr == '<')
            {
              while (*ptr && *ptr != '>')
                ptr ++;

              if (*ptr == '>')
                ptr ++;
            }
            else
            {
	      while (*ptr && !isspace(*ptr & 255))
		ptr ++;
            }

            if (!*ptr && first == line)
            {
              fprintf(stderr, "register: Bad attribute/enum definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (isspace(*ptr & 255))
              *ptr++ = '\0';

            if (first == line)
            {
	      if (*ptr != '(')
	      {
		fprintf(stderr, "register: Bad attribute definition on line %d.\n",
			linenum);
		exit(1);
	      }

	      ptr ++;
	      second = ptr;
	      for (paren = 1; *ptr && paren > 0; ptr ++)
	      {
		if (*ptr == '(')
		  paren ++;
		else if (*ptr == ')')
		{
		  paren --;
		  if (paren == 0)
		    *ptr = '\0';
		}
	      }

             /*
              * attribute syntax
              */

              fprintf(stderr, "Attribute \"%s (%s)\"\n", first, second);
              changed |= add_valattr(xml, IPP_REGISTRY_ENUMS, first, second, xref, xrefname);

              strncpy(attrname, first, sizeof(attrname));
              strncpy(syntax, second, sizeof(syntax));
            }
            else if (!attrname[0])
            {
              fprintf(stderr, "Missing attribute before enum value on line %d.\n",
                      linenum);
              exit(1);
            }
            else if (!*ptr && first[0] != '<')
            {
              fprintf(stderr, "register: Bad enum definition on line %d.\n",
                      linenum);
              exit(1);
            }
            else
            {
             /*
              * enum keyword
              */

	      for (second = ptr, ptr ++; *ptr && !isspace(*ptr); ptr ++);

	      *ptr = '\0';
	      if (first[0] == '<')
	        second = NULL;

              fprintf(stderr, "Enum %s '%s' \"%s (%s)\"\n", first, second, attrname,
                      syntax);

              changed |= add_value(xml, first, second, attrname, syntax, xref, xrefname);
            }
            break;

        case IN_OPERATIONS :
            if (!isupper(*ptr & 255))
              continue;

            first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;
	    if (!strncmp(ptr, " (extension)", 12))
	      ptr += 12;
            *ptr = '\0';

	    fprintf(stderr, "Operation '%s'\n", first);
            changed |= add_operation(xml, first, xref, xrefname);
            break;

        case IN_STATUS_CODES :
            if (strncmp(ptr, "0x", 2) || strchr(ptr, ':'))
              continue;

            first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

            if (!*ptr)
            {
              fprintf(stderr,
                      "register: Bad status code definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (isspace(*ptr & 255))
              *ptr++ = '\0';

            second = ptr;
            if (!islower(*ptr & 255))
            {
              fprintf(stderr,
                      "register: Bad status code definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (*ptr && (islower(*ptr & 255) || *ptr == '-'))
              ptr ++;
            *ptr = '\0';

	    fprintf(stderr, "Status Code %s '%s'\n", first, second);
            changed |= add_status_code(xml, first, second, xref, xrefname);
            break;

        case IN_OBJECTS :
            first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

            if (!*ptr)
            {
              fprintf(stderr,
                      "register: Bad object definition on line %d.\n",
                      linenum);
              exit(1);
            }

            *ptr = '\0';

	    fprintf(stderr, "Object %s\n", first);
            changed |= add_object(xml, first, xref, xrefname);
            break;

        case IN_ATTR_GROUP_VALS :
            if (strncmp(ptr, "0x", 2) || strchr(ptr, ':'))
              continue;

            first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

            if (!*ptr)
            {
              fprintf(stderr,
                      "register: Bad attribute group value definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (isspace(*ptr & 255))
              *ptr++ = '\0';

            second = ptr;
            if (!islower(*ptr & 255))
            {
              fprintf(stderr,
                      "register: Bad attribute group value definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (*ptr && (islower(*ptr & 255) || *ptr == '-'))
              ptr ++;
            *ptr = '\0';

	    fprintf(stderr, "Attribute Group %s '%s'\n", first, second);
            changed |= add_attr_group(xml, first, second, xref, xrefname);
            break;

        case IN_OUT_OF_BAND_VALS :
            if (strncmp(ptr, "0x", 2) || strchr(ptr, ':'))
              continue;

            first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

            if (!*ptr)
            {
              fprintf(stderr,
                      "register: Bad out-of-band value definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (isspace(*ptr & 255))
              *ptr++ = '\0';

            second = ptr;
            if (!islower(*ptr & 255))
            {
              fprintf(stderr,
                      "register: Bad out-of-band value definition on line %d.\n",
                      linenum);
              exit(1);
            }

            while (*ptr && (islower(*ptr & 255) || *ptr == '-'))
              ptr ++;
            *ptr = '\0';

	    fprintf(stderr, "Out-of-Band Value %s '%s'\n", first, second);
            changed |= add_out_of_band(xml, first, second, xref, xrefname);
            break;

        default :
            break;
      }
    }
  }

  return (changed);
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


  if (node->parent && !strcmp(node->parent->value.element.name, "note"))
    return (NULL);

  if (where == MXML_WS_AFTER_OPEN)
  {
    const char *name = mxmlGetElement(node);

    if (name[0] == '!' || name[0] == '?' ||
        !strcmp(name, "registry") || !strcmp(name, "record") || !strcmp(name, "range") ||
        (!node->child && strcmp(node->parent->value.element.name, "note")))
      return ("\n");
    else
      return (NULL);
  }
  else if (where == MXML_WS_BEFORE_CLOSE)
  {
    const char *name = mxmlGetElement(node);

    if (strcmp(name, "registry") && strcmp(name, "record") && strcmp(name, "range"))
      return (NULL);
  }
  else if (where == MXML_WS_AFTER_CLOSE)
    return ("\n");

  for (level = -4; node; node = node->parent, level += 2);

  if (level <= 0)
    return (NULL);
  else if (level > 40)
    return (spaces);
  else
    return (spaces + 40 - level);
}


/*
 * 'usage()' - Show program usage.
 */

static int				/* O - Exit status */
usage(const char *opt)			/* I - Pointer to option or NULL */
{
  if (opt)
    fprintf(stderr, "Unknown option '-%c'.\n", *opt);

  puts("\nUsage: ./register options filename.xml [filename.txt]\n");
  puts("Options:");
  puts("  -o newfile.xml            New XML file instead of replacing filename.xml");
  puts("  -t Title                  Title of standard");
  puts("  -x {rfcNNNN|URL}          URL or RFC for standard");
  return (1);
}


/*
 * 'validate_registry()' - Validate the sorting of a registry.
 */

static int				/* O - 1 if changed, 0 otherwise... */
validate_registry(
    mxml_node_t         *xml,		/* I - XML registry */
    const char          *registry,	/* I - Registry to validate */
    const char          *regname,	/* I - Name for message output */
    int                 num_keys,	/* I - Number of sorting keys */
    const char * const *keys)		/* I - Sorting keys */
{
  int		i,			/* Looping var */
		changed = 0,		/* Did something change? */
		num_records;		/* Number of records */
  reg_record_t	*records,		/* Records */
		*record;		/* Current record */
  mxml_node_t	*registry_node,		/* <registry> node */
		*record_node,		/* Current <record> node */
		*key_node,		/* Current key node */
		*last_node,		/* Last node in sequence */
		*range_node,		/* <range> node */
		*value_node,		/* <value> node */
		*syntax_node;		/* <syntax> node */
  const char	*value,			/* Value string */
		*syntax;		/* Syntax string */
  char		*type3;			/* Pointer to a "type3" definition */


 /*
  * Find the registry...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", registry,
                                       MXML_DESCEND)) == NULL)
  {
    fprintf(stderr, "register: Unable to find registry '%s' (%s) in XML file.\n",
            registry, regname);
    exit(1);
  }

 /*
  * Scan for "type3" registration rules...
  */

  for (range_node = mxmlFindElement(registry_node, registry_node, "range", NULL, NULL, MXML_DESCEND);
       range_node;
       range_node = mxmlFindElement(range_node, registry_node, "range", NULL, NULL, MXML_DESCEND))
  {
    value_node = mxmlFindElement(range_node, range_node, "value", NULL, NULL, MXML_DESCEND);
    value      = mxmlGetOpaque(value_node);

    if (value && !strcmp(value, "type3"))
    {
      fputs("Removing type3 registration rule.\n", stderr);
      mxmlDelete(range_node);
      range_node = registry_node;
      changed = 1;
    }
  }

 /*
  * Scan for "type3" registration syntaxes...
  */

  for (syntax_node = mxmlFindElement(registry_node, registry_node, "syntax", NULL, NULL, MXML_DESCEND);
       syntax_node;
       syntax_node = mxmlFindElement(syntax_node, registry_node, "syntax", NULL, NULL, MXML_DESCEND))
  {
    syntax = mxmlGetOpaque(syntax_node);
    while (syntax && (type3 = strstr(syntax, "type3")) != NULL)
    {
     /*
      * Change type3 to type2...
      */

      type3[4] = '2';
      changed  = 1;
    }
  }

 /*
  * Figure out the number of records...
  */

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST),
           num_records = 0;
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND),
           num_records ++);

  fprintf(stderr, "register: Registry '%s' (%s) has %d record%s.\n", registry,
	  regname, num_records, num_records != 1 ? "s" : "");

  if (num_records == 0)
    return (0);

  records = calloc(num_records, sizeof(reg_record_t));

 /*
  * Get the records...
  */

  for (record_node = mxmlFindElement(registry_node, registry_node, "record",
                                     NULL, NULL, MXML_DESCEND_FIRST),
           record = records;
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record",
                                     NULL, NULL, MXML_NO_DESCEND),
           record ++)
  {
    record->index = (int)(record - records);
    record->node  = record_node;

    for (i = 0; i < num_keys; i ++)
    {
      key_node = mxmlFindElement(record_node, record_node, keys[i], NULL, NULL,
                                 MXML_DESCEND_FIRST);
      if (!key_node && !strcmp(keys[i], "name"))
      {
        key_node = mxmlFindElement(record_node, record_node, "attribute",
                                   NULL, NULL, MXML_DESCEND_FIRST);

        if (key_node)
        {
          fprintf(stderr, "register: Fixing record for '%s' to use <name>.\n",
                  mxmlGetOpaque(key_node));
          mxmlSetElement(key_node, "name");
          changed = 1;
        }
      }
      else if (!key_node && !strcmp(keys[i], "attribute"))
      {
        key_node = mxmlFindElement(record_node, record_node, "name",
                                   NULL, NULL, MXML_DESCEND_FIRST);

        if (key_node)
        {
          fprintf(stderr,
                  "register: Fixing record for '%s' to use <attribute>.\n",
                  mxmlGetOpaque(key_node));
          mxmlSetElement(key_node, "attribute");
          changed = 1;
        }
      }

      record->keys[i] = mxmlGetOpaque(key_node);
    }
  }

 /*
  * Sort the records and see if anything changed...
  */

  qsort(records, num_records, sizeof(reg_record_t),
        (int (*)(const void *, const void *))compare_record);

  for (i = 0, record = records; i < num_records; i ++, record ++)
    if (record->index != i)
      break;

  if (i < num_records)
  {
    fprintf(stderr, "register: Fixing sorting of records in '%s' (%s).\n",
            registry, regname);
    changed = 1;

    last_node = mxmlFindElement(registry_node, registry_node, "note",
                                NULL, NULL, MXML_DESCEND_FIRST);

    for (i = 0, record = records; i < num_records; i ++, record ++)
    {
      if (last_node)
        mxmlAdd(registry_node, MXML_ADD_AFTER, last_node, record->node);
      else
        mxmlAdd(registry_node, MXML_ADD_BEFORE, MXML_ADD_TO_PARENT,
                record->node);

      last_node = record->node;
    }
  }

  free(records);

  return (changed);
}


/*
 * 'xref_name()' - Return the name to show for the referenced standard.
 */

static const char *			/* O - Name or NULL */
xref_name(const char *xref,		/* I - URL or "rfcNNNN" reference */
          const char *title)		/* I - Title */
{
  char	*ptr;				/* Pointer into URL/name */
  static char	name[128];		/* Name */


  if (!strncmp(xref, "rfc", 3))
    return (NULL);
  else if ((ptr = (char *)strstr(xref, "-51")) == NULL)
    return (title);

  snprintf(name, sizeof(name), "PWG%s", ptr + 1);
  if ((ptr = strchr(name, '-')) == NULL)
    ptr = strrchr(name, '.');

  if (*ptr)
    *ptr = '\0';

  return (name);
}
