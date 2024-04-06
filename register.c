//
// Program to add registration info to the IANA IPP registry.
//
// Copyright © 2018-2024 by The IEEE-ISTO Printer Working Group.
// Copyright © 2008-2017 by Michael R Sweet
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//
// Usage:
//
//    ./register options filename.xml [regtext.txt]
//
// If no regtext.txt file is supplied, reads registration from stdin.
//
// Options:
//
//    -f			Force writing of new file
//    -o newfile.xml		New XML file (instead of replacing filename.xml)
//    -t "Standard Title"	Title of standard
//    -v			Be verbose
//    -x "Standard URL"		URL for standard (or rfcNNNN)
//

#include "ipp-registry.h"


// Windows case insensitive string compare */
#if _WIN32
#  define strcasecmp _stricmp
#endif // _WIN32


//
// Constants...
//

enum
{
  IN_NOTHING,				// Nothing to process
  IN_OPER_ATTRS,			// Operation attributes
  IN_PDESC_ATTRS,			// Printer Description attributes
  IN_PSTAT_ATTRS,			// Printer Status attributes
  IN_JDESC_ATTRS,			// Job Description attributes
  IN_JSTAT_ATTRS,			// Job Status attributes
  IN_JTMPL_ATTRS,			// Job Template attributes
  IN_DDESC_ATTRS,			// Document Description attributes
  IN_DSTAT_ATTRS,			// Document Status attributes
  IN_DTMPL_ATTRS,			// Document Template attributes
  IN_RDESC_ATTRS,			// Resource Description attributes
  IN_RSTAT_ATTRS,			// Resource Status attributes
  IN_SDESC_ATTRS,			// Subscription Description attributes
  IN_SSTAT_ATTRS,			// Subscription Status attributes
  IN_STMPL_ATTRS,			// Subscription Template attributes
  IN_SYSDESC_ATTRS,			// System Description attributes
  IN_SYSSTAT_ATTRS,			// System Status attributes
  IN_EVENT_ATTRS,			// Event Notification attributes
  IN_KEYWORD_VALS,			// Keyword attribute values
  IN_ENUM_VALS,				// Enum Attribute values
  IN_OPERATIONS,			// Operations
  IN_STATUS_CODES,			// Status codes
  IN_OBJECTS,				// Objects
  IN_ATTR_GROUP_VALS,			// Attribute Group Values
  IN_OUT_OF_BAND_VALS,			// Out-of-Band Values
  IN_MAX
};


//
// Structures...
//

typedef struct				// Record for sorting
{
  size_t	index;			// Index (to detect changes)
  mxml_node_t	*node;			// Node for this record
  const char	*keys[4];		// Sorting keys
} reg_record_t;

typedef struct				// Work counters
{
  size_t	added,			// Items added
		updated,		// Items updated/extended
		ignored;		// Items ignored
} reg_counter_t;


//
// Local globals...
//

static reg_counter_t	Attributes = { 0, 0, 0 };
static reg_counter_t	Enums = { 0, 0, 0 };
static reg_counter_t	Keywords = { 0, 0, 0 };
static reg_counter_t	Operations = { 0, 0, 0 };
static reg_counter_t	StatusCodes = { 0, 0, 0 };
static int		Verbosity = 0;


//
// Local functions...
//

static bool		add_attr(mxml_node_t *xml, FILE *logfile, const char *collection, const char *attrname, const char *membername, const char *submembername, const char *syntax, const char *xref, const char *xrefname);
static bool		add_attr_group(mxml_node_t *xml, FILE *logfile, const char *value, const char *name, const char *xref, const char *xrefname);
static bool		add_object(mxml_node_t *xml, FILE *logfile, const char *name, const char *xref, const char *xrefname);
static bool		add_operation(mxml_node_t *xml, FILE *logfile, const char *name, const char *xref, const char *xrefname);
static bool		add_out_of_band(mxml_node_t *xml, FILE *logfile, const char *value, const char *name, const char *xref, const char *xrefname);
static bool		add_status_code(mxml_node_t *xml, FILE *logfile, const char *value, const char *name, const char *xref, const char *xrefname);
static bool		add_value(mxml_node_t *xml, FILE *logfile, const char *enumval, const char *keyword, const char *attrname, const char *syntax, const char *xref, const char *xrefname);
static bool		add_valattr(mxml_node_t *xml, FILE *logfile, const char *registry, const char *attrname, const char *syntax, const char *xref, const char *xrefname);
static bool		add_value(mxml_node_t *xml, FILE *logfile, const char *enumval, const char *keyword, const char *attrname, const char *syntax, const char *xref, const char *xrefname);
static int		compare_record(reg_record_t *a, reg_record_t *b);
static int		compare_strings(const char *s, const char *t);
static const char	*node_name(mxml_node_t *node);
static bool		read_text(mxml_node_t *xml, FILE *textfile, FILE *logfile, const char *title, const char *xref);
static bool		update_xref(mxml_node_t *record, FILE *logfile, const char *xref, const char *xrefname);
static int		usage(const char *opt);
static bool		validate_registry(mxml_node_t *xml, const char *registry, const char *regname, size_t num_keys, const char * const *keys);
static const char	*ws_cb(void *data, mxml_node_t *node, mxml_ws_t ws);
static const char	*xref_name(const char *xref, const char *title);


//
// 'main()' - Handle additions to the IANA registry.
//

int					// O - Exit status
main(int  argc,				// I - Number of command-line args
     char *argv[])			// I - Command-line arguments
{
  int		i;			// Looping var
  const char	*xmlin = NULL,		// XML registration input file
		*xmlout = NULL,		// XML registration output file
		*textin = NULL,		// Text registration input file
		*title = NULL,		// Title for standard
		*xref = NULL,		// URL for standard (or rfcNNNN)
		*opt;			// Current option character
  mxml_node_t	*xml;			// XML registration file top node
  mxml_options_t *xmloptions;		// XML load/save options
  char		xmlbackup[1024];	// Backup file
  FILE		*textfile,		// Text registration file
		*logfile;		// Log file
  bool		changed = false;	// Was the registration data changed?
  static const char * const attribute_keys[] = { "collection", "name", "member_attribute", "sub-member_attribute" };
  static const char * const attribute_group_keys[] = { "value" };
  static const char * const keyword_keys[] = { "attribute", "value" };
  static const char * const enum_keys[] = { "attribute", "value" };
  static const char * const object_keys[] = { "name" };
  static const char * const operation_keys[] = { "name" };
  static const char * const out_of_band_keys[] = { "value" };
  static const char * const status_code_keys[] = { "value" };
					// Sorting keys for the different registries


  // Parse command-line arguments...
  for (i = 1; i < argc; i ++)
  {
    if (argv[i][0] == '-')
    {
      for (opt = argv[i] + 1; *opt; opt ++)
      {
	switch (*opt)
	{
	  case 'f' : // Force output
	      changed = true;
	      break;

	  case 'o' : // Output file
	      if (xmlout)
	      {
		fputs("register: Output file may only be specified once.\n", stderr);
		return (usage(NULL));
	      }

	      i ++;
	      if (i >= argc)
	      {
		fputs("register: Expected filename after '-o'.\n", stderr);
		return (usage(NULL));
	      }

	      xmlout = argv[i];
	      break;

	  case 't' : // Standard title
	      if (title)
	      {
		fputs("register: Title may only be specified once.\n", stderr);
		return (usage(NULL));
	      }

	      i ++;
	      if (i >= argc)
	      {
		fputs("register: Expected title after '-t'.\n", stderr);
		return (usage(NULL));
	      }

	      title = argv[i];
	      break;

	  case 'v' :	// Be verbose
	      Verbosity ++;
	      break;

	  case 'x' :	// Standard URL or RFC number
	      if (xref)
	      {
		fputs("register: URL may only be specified once.\n", stderr);
		return (usage(NULL));
	      }

	      i ++;
	      if (i >= argc)
	      {
		fputs("register: Expected URL or RFC number after '-x'.\n", stderr);
		return (usage(NULL));
	      }

	      if (strncmp(argv[i], "rfc", 3) && strncmp(argv[i], "http://", 7) && strncmp(argv[i], "https://", 8))
	      {
		fputs("register: Standard URL must be 'rfcNNNN', 'http://...', or 'https://...'.\n", stderr);
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
    {
      xmlin = argv[i];
    }
    else if (!textin)
    {
      textin = argv[i];
    }
    else
    {
      fprintf(stderr, "register: Unknown argument '%s'.\n", argv[i]);
      return (usage(NULL));
    }
  }

  if (!title)
    title = "TEST";

  if (!xref)
    xref = "test";

  if (!xmlin)
  {
    fputs("register: Missing required input file.\n", stderr);
    return (usage(NULL));
  }

  if (!xmlout)
    xmlout = xmlin;

  // Load the XML registration file...
  xmloptions = mxmlOptionsNew();
  mxmlOptionsSetErrorCallback(xmloptions, (mxml_error_cb_t)ipp_error_cb, (void *)"register");
  mxmlOptionsSetTypeCallback(xmloptions, ipp_type_cb, /*cbdata*/NULL);
  mxmlOptionsSetWhitespaceCallback(xmloptions, ws_cb, /*cbdata*/NULL);
  mxmlOptionsSetWrapMargin(xmloptions, 0);

  if ((xml = mxmlLoadFilename(/*top*/NULL, xmloptions, xmlin)) == NULL)
    return (1);

  // Verify that the XML file is well-formed...
  changed |= validate_registry(xml, IPP_REGISTRY_OBJECTS, "Objects", sizeof(object_keys) / sizeof(object_keys[0]), object_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_ATTRIBUTES, "Attributes", sizeof(attribute_keys) / sizeof(attribute_keys[0]), attribute_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_KEYWORDS, "Keyword Values", sizeof(keyword_keys) / sizeof(keyword_keys[0]), keyword_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_ENUMS, "Enum Values", sizeof(enum_keys) / sizeof(enum_keys[0]), enum_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_ATTRIBUTE_GROUPS, "Attribute Groups", sizeof(attribute_group_keys) / sizeof(attribute_group_keys[0]), attribute_group_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_OUT_OF_BANDS, "Out-of-Band Values", sizeof(out_of_band_keys) / sizeof(out_of_band_keys[0]), out_of_band_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_OPERATIONS, "Operations", sizeof(operation_keys) / sizeof(operation_keys[0]), operation_keys);
  changed |= validate_registry(xml, IPP_REGISTRY_STATUS_CODES, "Status Codes", sizeof(status_code_keys) / sizeof(status_code_keys[0]), status_code_keys);

  // Read the text registrations...
  if (textin)
  {
    char	logname[1024],		// Log filename
		*logext;		// Extension

    if ((textfile = fopen(textin, "rb")) == NULL)
    {
      fprintf(stderr, "register: Unable to open '%s': %s\n", textin, strerror(errno));
      return (1);
    }

    ipp_copy_string(logname, textin, sizeof(logname) - 1);
    logname[sizeof(logname) - 1] = '\0';
    if ((logext = strrchr(logname, '.')) == NULL || strcasecmp(logext, ".txt"))
      logext = logname + strlen(logname);

    ipp_copy_string(logext, ".log", sizeof(logname) - 1 - (size_t)(logext - logname));
    if ((logfile = fopen(logname, "w")) == NULL)
    {
      fprintf(stderr, "Unable to create '%s': %s\n", logname, strerror(errno));
      logfile = stdout;
    }

    setbuf(logfile, NULL);
  }
  else
  {
    textfile = stdin;
    logfile  = stdout;
  }

  changed |= read_text(xml, textfile, logfile, title, xref);

  if (textfile != stdin)
    fclose(textfile);

  if (logfile != stdout)
    fclose(logfile);

  // Save the XML registration file if there were changes...
  if (changed || Verbosity)
  {
    if (Attributes.added || Attributes.updated || Attributes.ignored)
      printf("Attributes: %lu added, %lu updated, %lu ignored\n", (unsigned long)Attributes.added, (unsigned long)Attributes.updated, (unsigned long)Attributes.ignored);

    if (Enums.added || Enums.updated || Enums.ignored)
      printf("Enums: %lu added, %lu updated, %lu ignored\n", (unsigned long)Enums.added, (unsigned long)Enums.updated, (unsigned long)Enums.ignored);

    if (Keywords.added || Keywords.updated || Keywords.ignored)
      printf("Keywords: %lu added, %lu updated, %lu ignored\n", (unsigned long)Keywords.added, (unsigned long)Keywords.updated, (unsigned long)Keywords.ignored);

    if (Operations.added || Operations.updated || Operations.ignored)
      printf("Operations: %lu added, %lu updated, %lu ignored\n", (unsigned long)Operations.added, (unsigned long)Operations.updated, (unsigned long)Operations.ignored);

    if (StatusCodes.added || StatusCodes.updated || StatusCodes.ignored)
      printf("Status Codes: %lu added, %lu updated, %lu ignored\n", (unsigned long)StatusCodes.added, (unsigned long)StatusCodes.updated, (unsigned long)StatusCodes.ignored);
  }

  if (changed)
  {
    mxml_node_t *updated = mxmlFindElement(xml, xml, "updated", NULL, NULL, MXML_DESCEND_ALL);

    if (updated)
    {
      time_t curtime = time(NULL);	// Current time in seconds
      struct tm *curdate = gmtime(&curtime);
					// Current date in UTC
      char datestr[64];			// Date string YYYY-MM-DD

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

    if (!mxmlSaveFilename(xml, xmloptions, xmlout))
    {
      if (xmlin == xmlout)
	rename(xmlbackup, xmlin);

      return (1);
    }
    else
    {
      printf("Wrote '%s'.\n", xmlout);
    }
  }
  else
  {
    puts("No changes written.");
  }

  return (!changed);
}


//
// 'add_attr()' - Add a (member) attribute in the right position of the XML registry.
//

static bool				// O - `true` if something was added, `false` if not
add_attr(mxml_node_t *xml,		// I - XML registry
         FILE        *logfile,		// I - Log file
	 const char  *collection,	// I - Collection within the attributes registry
	 const char  *attrname,		// I - Attribute name
	 const char  *membername,	// I - Member attribute name or NULL
	 const char  *submembername,	// I - Sub-member attribute name or NULL
	 const char  *syntax,		// I - Syntax string
	 const char  *xref,		// I - Standard URL/number
	 const char  *xrefname)		// I - Standard name or NULL
{
  bool		changed = false;	// Did something change?
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*collection_node,	// <collection>
		*name_node,		// <name>
		*membername_node,	// <member_attribute>
		*submembername_node,	// <sub-member_attribute>
		*syntax_node;		// <syntax>
  const char	*last_collection = NULL,// Last collection
		*last_name = NULL,	// Last attribute name
		*last_member = NULL,	// Last member attribute
		*last_submember = NULL; // Last sub-member attribute
  char		extname[256];		// attribute-name(extension)


  // Find any existing definition of the attribute...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_ATTRIBUTES "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    collection_node = mxmlFindElement(record_node, record_node, "collection", NULL, NULL, MXML_DESCEND_FIRST);
    if (!collection_node || !mxmlGetOpaque(collection_node))
    {
      fputs("register: Attribute record missing <collection> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    collection_node = mxmlFindElement(record_node, record_node, "collection", NULL, NULL, MXML_DESCEND_FIRST);
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

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (!name_node)
    {
      name_node = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL, MXML_DESCEND_FIRST);

      if (!name_node)
      {
	fputs("register: Attribute record missing <name> in XML file.\n", stderr);
	mxmlSaveFile(record_node, NULL, stderr);
	exit(1);
      }
      else
      {
	fprintf(logfile, "Fixing attribute record for %s.\n", mxmlGetOpaque(name_node));
	mxmlSetElement(name_node, "name");
	changed = true;
	Attributes.updated ++;
      }
    }

    if (last_name)
    {
      if ((result = compare_strings(last_name, mxmlGetOpaque(name_node))) > 0)
      {
	fputs("register: Attribute record out-of-order in XML file.\n", stderr);
	mxmlSaveFile(record_node, NULL, stderr);
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

    membername_node = mxmlFindElement(record_node, record_node, "member_attribute", NULL, NULL, MXML_DESCEND_FIRST);
    if (last_member && membername_node && mxmlGetOpaque(membername_node))
    {
      if ((result = compare_strings(last_member, mxmlGetOpaque(membername_node))) > 0)
      {
	fputs("register: Attribute record out-of-order in XML file.\n", stderr);
	mxmlSaveFile(record_node, NULL, stderr);
	exit(1);
      }

      if (result)
	last_submember = NULL;
    }

    last_member = mxmlGetOpaque(membername_node);
    if (membername && !last_member)
    {
      continue;
    }
    else if (last_member && !membername)
    {
      break;
    }
    else if (membername)
    {
      if ((result = compare_strings(membername, last_member)) > 0)
	continue;
      else if (result < 0)
	break;
    }

    submembername_node = mxmlFindElement(record_node, record_node, "sub-member_attribute", NULL, NULL, MXML_DESCEND_FIRST);
    if (last_submember && submembername_node && mxmlGetOpaque(submembername_node))
    {
      if (compare_strings(last_submember, mxmlGetOpaque(submembername_node)) > 0)
      {
	fputs("register: Attribute record out-of-order in XML file.\n", stderr);
	mxmlSaveFile(record_node, NULL, stderr);
	exit(1);
      }
    }

    last_submember = mxmlGetOpaque(submembername_node);
    if (submembername && !last_submember)
    {
      continue;
    }
    else if (last_submember && !submembername)
    {
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

    syntax_node = mxmlFindElement(record_node, record_node, "syntax", NULL, NULL, MXML_DESCEND_FIRST);
    if (!syntax_node)
    {
      fputs("register: Attribute record missing <syntax> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if (update_xref(record_node, logfile, xref, xrefname))
      changed = true;

    if (compare_strings(mxmlGetOpaque(syntax_node), syntax))
    {
      // Extend definition...
      fprintf(logfile, "Extending syntax for %s to '%s'.\n", submembername ? submembername : membername ? membername : attrname, syntax);

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

      Attributes.updated ++;
    }
    else
    {
      // No change to base IETF definition...
      fprintf(logfile, "Ignoring definition of %s.\n", submembername ? submembername : membername ? membername : attrname);
      return (changed);
    }
  }

 /*
  * If we get here the attribute does not exist.  Build a new record and add it
  * to the registry in the right place.
  */

  if (submembername)
    fprintf(logfile, "Adding member attribute %s/%s/%s (%s)...\n", attrname, membername, submembername, syntax);
  else if (membername)
    fprintf(logfile, "Adding member attribute %s/%s (%s)...\n", attrname, membername, syntax);
  else
    fprintf(logfile, "Adding attribute %s (%s)...\n", attrname, syntax);

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

  update_xref(node, logfile, xref, xrefname);

  Attributes.added ++;

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_attr_group()' - Add an attribute group value to the registry.
//

static bool				// O - `true` if something was added, `false` if not
add_attr_group(mxml_node_t *xml,	// I - XML registry
	       FILE        *logfile,	// I - Log file
	       const char  *value,	// I - Attribute group value
	       const char  *name,	// I - Attribute group name
	       const char  *xref,	// I - Standard URL/number
	       const char  *xrefname)	// I - Standard name or NULL
{
  bool		changed = false;	// Did something change?
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*name_node,		// <name>
		*value_node;		// <value>
  char		*nodeval;		// Pointer into node value


 /*
  * Find any existing definition of the attribute...
  */

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTE_GROUPS, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_ATTRIBUTE_GROUPS "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST);
    if (!value_node)
    {
      fputs("register: Status code record missing <value> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if ((result = strtol(value, NULL, 0) - strtol(mxmlGetOpaque(value_node), &nodeval, 0)) > 0)
      continue;
    else if (result < 0)
      break;

    if (nodeval && *nodeval == '-')
    {
      // 0xXXXX-0xXXXX range; update the range to be 1 more than the current
      // value and then insert the new status code before this one...
      char	newvalue[256];		// New value

      snprintf(newvalue, sizeof(newvalue), "0x%04x%s", (unsigned)strtol(value, NULL, 0) + 1, nodeval);
      mxmlSetOpaque(value_node, newvalue);
      break;
    }

    // OK, at this point the current record matches the value we are trying to add.
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fprintf(logfile, "Fixing record for attribute group '%s'.\n", value);
      name_node = mxmlNewElement(record_node, "name");
    }

    if (update_xref(record_node, logfile, xref, xrefname))
      changed = true;

    if (!mxmlGetOpaque(name_node))
    {
      fprintf(logfile, "Updating record for attribute group '%s'.\n", value);
      mxmlNewOpaque(name_node, name);
      return (true);
    }
    else if (compare_strings(name, mxmlGetOpaque(name_node)))
    {
      fprintf(logfile, "Updating record for attribute group '%s'.\n", value);
      mxmlSetOpaque(name_node, name);
      return (true);
    }
    else
      return (changed);
  }

  // If we get here the value does not exist.  Build a new record and add it to
  // the registry in the right place.
  fprintf(logfile, "Adding attribute group %s '%s'...\n", value, name);

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, value);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_valattr()' - Add a keyword/enum attribute definition.
//

static bool				// O - `true` if something was added, `false` if not
add_valattr(mxml_node_t *xml,		// I - XML registry
	    FILE        *logfile,	// I - Log file
	    const char	*registry,	// I - Registry ID
	    const char	*attrname,	// I - Attribute name
	    const char	*syntax,	// I - Attribute syntax
	    const char	*xref,		// I - Standard URL/number
	    const char	*xrefname)	// I - Standard name or NULL
{
  bool		changed = false,	// Did something change?
		compare_enums;		// Compare enum values?
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*attr_node,		// <attribute>
		*syntax_node;		// <syntax>
  const char	*last_attr = NULL;	// Last attribute name


  // Find any existing definition of the attribute...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", registry, MXML_DESCEND_ALL)) == NULL)
  {
    fprintf(stderr, "register: Unable to find '%s' registry in XML file.\n", registry);
    exit(1);
  }

  compare_enums = !strcmp(registry, IPP_REGISTRY_ENUMS);

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    attr_node = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL, MXML_DESCEND_FIRST);
    if (!attr_node)
    {
      attr_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);

      if (!attr_node)
      {
	fprintf(stderr, "register: %s record missing <attribute> in XML file.\n", compare_enums ? "enum" : "keyword");
	mxmlSaveFile(record_node, NULL, stderr);
	exit(1);
      }
      else
      {
	if (compare_enums)
	  Enums.updated ++;
	else
	  Keywords.updated ++;

	fprintf(logfile, "Fixing %s record for '%s'.\n", compare_enums ? "enum" : "keyword", mxmlGetOpaque(attr_node));
	mxmlSetElement(attr_node, "attribute");
	changed = true;
      }
    }

    if (last_attr && mxmlGetOpaque(attr_node))
    {
      if (compare_strings(last_attr, mxmlGetOpaque(attr_node)) > 0)
      {
	fprintf(stderr, "register: %s record out-of-order in XML file.\n", compare_enums ? "Enum" : "Keyword");
	mxmlSaveFile(record_node, NULL, stderr);
	exit(1);
      }
    }

    if ((result = compare_strings(attrname, mxmlGetOpaque(attr_node))) > 0)
      continue;
    else if (result < 0)
      break;

    if (mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST))
      break;

    // OK, at this point the current record matches the attribute we are trying
    // to add.  If the syntax doesn't match issue a warning and reset the
    // syntax, otherwise just return with no changes...
    syntax_node = mxmlFindElement(record_node, record_node, "syntax", NULL, NULL, MXML_DESCEND_FIRST);
    if (!syntax_node)
    {
      fputs("register: Attribute record missing <syntax> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if (update_xref(record_node, logfile, xref, xrefname))
      changed = true;

    if (compare_strings(mxmlGetOpaque(syntax_node), syntax))
    {
      if (compare_enums)
        Enums.updated ++;
      else
        Keywords.updated ++;

      fprintf(logfile, "Changing syntax for '%s' to '%s'.\n", attrname, syntax);
      mxmlSetOpaque(syntax_node, syntax);

      return (true);
    }
    else
      return (changed);
  }

  // If we get here the attribute does not exist.  Build a new record and add it
  // to the registry in the right place.
  if (compare_enums)
  {
    Enums.added ++;
    fprintf(logfile, "Adding enum attribute '%s'...\n", attrname);
  }
  else
  {
    Keywords.added ++;
    fprintf(logfile, "Adding keyword attribute '%s'...\n", attrname);
  }

  node = mxmlNewElement(NULL, "record");

  attr_node = mxmlNewElement(node, "attribute");
  mxmlNewOpaque(attr_node, attrname);

  syntax_node = mxmlNewElement(node, "syntax");
  mxmlNewOpaque(syntax_node, syntax);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_object()' - Add an object to the registry.
//

static bool				// O - `true` if something was added, `false` if not
add_object(mxml_node_t *xml,		// I - XML registry
	   FILE        *logfile,	// I - Log file
	   const char  *name,		// I - Object name
	   const char  *xref,		// I - Standard URL/number
	   const char  *xrefname)	// I - Standard name or NULL
{
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*name_node;		// <name>


  // Find any existing definition of the operation...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_OBJECTS, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_OBJECTS "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fputs("register: Object record missing <name> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if ((result = compare_strings(name, mxmlGetOpaque(name_node))) == 0)
    {
      fprintf(logfile, "Duplicate object '%s'.\n", name);
      return (update_xref(record_node, logfile, xref, xrefname));
    }
    else if (result < 0)
    {
      break;
    }
  }

  // If we get here the value does not exist.  Build a new record and add it to
  // the registry in the right place.
  fprintf(logfile, "Adding object '%s'...\n", name);

  node = mxmlNewElement(NULL, "record");

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_operation()' - Add an operation to the registry.
//

static bool				// O - `true` if something was added, `false` if not
add_operation(mxml_node_t *xml,		// I - XML registry
	      FILE        *logfile,	// I - Log file
	      const char  *name,	// I - Operation name
	      const char  *xref,	// I - Standard URL/number
	      const char  *xrefname)	// I - Standard name or NULL
{
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*name_node;		// <name>


  // Find any existing definition of the operation...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_OPERATIONS, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_OPERATIONS "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fputs("register: Operation record missing <name> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if ((result = compare_strings(name, mxmlGetOpaque(name_node))) == 0)
    {
      Operations.ignored ++;
      fprintf(logfile, "Duplicate operation '%s'.\n", name);
      return (update_xref(record_node, logfile, xref, xrefname));
    }
    else if (result < 0)
    {
      break;
    }
  }

  // If we get here the value does not exist.  Build a new record and add it to
  // the registry in the right place.
  Operations.added ++;
  fprintf(logfile, "Adding operation '%s'...\n", name);

  node = mxmlNewElement(NULL, "record");

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_out_of_band()' - Add an out-of-band value to the registry.
//

static bool				// O - `true` if something was added, `false` if not
add_out_of_band(mxml_node_t *xml,	// I - XML registry
		FILE        *logfile,	// I - Log file
		const char  *value,	// I - Out-of-band value
		const char  *name,	// I - Out-of-band name
		const char  *xref,	// I - Standard URL/number
		const char  *xrefname)	// I - Standard name or NULL
{
  bool		changed = false;	// Did something change?
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*name_node,		// <name>
		*value_node;		// <value>
  char		*nodeval;		// Pointer into node value


  // Find any existing definition of the attribute...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_OUT_OF_BANDS, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_OUT_OF_BANDS "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST);
    if (!value_node)
    {
      fputs("register: Out-of-band record missing <value> in XML file.\n",
	    stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if ((result = strtol(value, NULL, 0) - strtol(mxmlGetOpaque(value_node), &nodeval, 0)) > 0)
      continue;
    else if (result < 0)
      break;

    if (nodeval && *nodeval == '-')
    {
      // 0xXXXX-0xXXXX range; update the range to be 1 more than the current
      // value and then insert the new out-of-band value before this one...
      char	newvalue[256];		// New value

      snprintf(newvalue, sizeof(newvalue), "0x%04x%s", (unsigned)strtol(value, NULL, 0) + 1, nodeval);
      mxmlSetOpaque(value_node, newvalue);
      break;
    }

    // OK, at this point the current record matches the value we are trying to add.
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fprintf(logfile, "Fixing record for out-of-band value %s.\n", value);
      name_node = mxmlNewElement(record_node, "name");
    }

    if (update_xref(record_node, logfile, xref, xrefname))
      changed = true;

    if (!mxmlGetOpaque(name_node))
    {
      fprintf(logfile, "Updating record for out-of-band value %s.\n", value);
      mxmlNewOpaque(name_node, name);
      return (true);
    }
    else if (compare_strings(name, mxmlGetOpaque(name_node)))
    {
      fprintf(logfile, "Updating record for out-of-band value %s.\n", value);
      mxmlSetOpaque(name_node, name);
      return (true);
    }
    else
    {
      return (changed);
    }
  }

  // If we get here the value does not exist.  Build a new record and add it to
  // the registry in the right place.
  fprintf(logfile, "Adding out-of-band value %s '%s'\n", value, name);

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, value);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_status_code()' - Add a status code to the registry.
//

static bool				// O - `true` if something was added, `false` if not
add_status_code(mxml_node_t *xml,	// I - XML registry
		FILE        *logfile,	// I - Log file
		const char  *value,	// I - Status code value
		const char  *name,	// I - Status code name
		const char  *xref,	// I - Standard URL/number
		const char  *xrefname)	// I - Standard name or NULL
{
  bool		changed = false;	// Did something change?
  int		result;			// Result of comparison
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*name_node,		// <name>
		*value_node;		// <value>
  char		*nodeval;		// Pointer into node value


  // Find any existing definition of the attribute...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_STATUS_CODES, MXML_DESCEND_ALL)) == NULL)
  {
    fputs("register: Unable to find '" IPP_REGISTRY_STATUS_CODES "' registry in XML file.\n", stderr);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST);
    if (!value_node)
    {
      fputs("register: Status code record missing <value> in XML file.\n", stderr);
      mxmlSaveFile(record_node, NULL, stderr);
      exit(1);
    }

    if ((result = strtol(value, NULL, 0) - strtol(mxmlGetOpaque(value_node), &nodeval, 0)) > 0)
      continue;
    else if (result < 0)
      break;

    if (nodeval && *nodeval == '-')
    {
      // 0xXXXX-0xXXXX range; update the range to be 1 more than the current
      // value and then insert the new status code before this one...
      char	newvalue[256];		// New value

      snprintf(newvalue, sizeof(newvalue), "0x%04x%s", (unsigned)strtol(value, NULL, 0) + 1, nodeval);
      mxmlSetOpaque(value_node, newvalue);
      break;
    }

    // OK, at this point the current record matches the value we are trying to add.
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (!name_node)
    {
      fprintf(logfile, "Fixing record for status code '%s'.\n", value);
      name_node = mxmlNewElement(record_node, "name");
    }

    if (update_xref(record_node, logfile, xref, xrefname))
      changed = true;

    if (!mxmlGetOpaque(name_node) || compare_strings(name, mxmlGetOpaque(name_node)))
    {
      StatusCodes.updated ++;
      fprintf(logfile, "Updating record for status code '%s'.\n", value);

      if (!mxmlGetOpaque(name_node))
        mxmlNewOpaque(name_node, name);
      else
        mxmlSetOpaque(name_node, name);

      return (true);
    }
    else
    {
      fprintf(logfile, "Ignoring record for status code '%s'.\n", value);
      return (changed);
    }
  }

  // If we get here the value does not exist.  Build a new record and add it to
  // the registry in the right place.
  StatusCodes.added ++;

  fprintf(logfile, "Adding status code %s '%s'...\n", value, name);

  node = mxmlNewElement(NULL, "record");

  value_node = mxmlNewElement(node, "value");
  mxmlNewOpaque(value_node, value);

  name_node = mxmlNewElement(node, "name");
  mxmlNewOpaque(name_node, name);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'add_value()' - Add a keyword/enum value to the registry.
//

static bool				// O - `true` if something was added, `false` if not
add_value(mxml_node_t *xml,		// I - XML registry
	  FILE        *logfile,		// I - Log file
	  const char  *enumval,		// I - Enumeration value or NULL
	  const char  *keyword,		// I - Keyword
	  const char  *attrname,	// I - Attribute name
	  const char  *syntax,		// I - Attribute syntax
	  const char  *xref,		// I - Standard URL/number
	  const char  *xrefname)	// I - Standard name or NULL
{
  bool		changed = false;	// Did something change?
  int		result;			// Result of comparison
  const char	*registry,		// Registry
		*name;			// <name> value
  mxml_node_t	*node,			// New <registry> node
		*registry_node,		// <registry>
		*record_node,		// <record>
		*attr_node,		// <attribute>
		*syntax_node,		// <syntax>
		*name_node,		// <name>
		*value_node;		// <value>


  // Find any existing definition of the attribute...
  registry = enumval ? IPP_REGISTRY_ENUMS : IPP_REGISTRY_KEYWORDS;

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", registry, MXML_DESCEND_ALL)) == NULL)
  {
    fprintf(stderr, "register: Unable to find '%s' registry in XML file.\n", registry);
    exit(1);
  }

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
  {
    attr_node = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL, MXML_DESCEND_FIRST);
    if (!attr_node)
    {
      attr_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);

      if (!attr_node)
      {
	fprintf(stderr, "register: %s record missing <attribute> in XML file.\n", enumval ? "enum" : "keyword");
	mxmlSaveFile(record_node, NULL, stderr);
	exit(1);
      }
      else
      {
        if (enumval)
          Enums.updated ++;
        else
          Keywords.updated ++;

	fprintf(logfile, "Fixing %s record for %s.\n", enumval ? "enum" : "keyword", mxmlGetOpaque(attr_node));
	mxmlSetElement(attr_node, "attribute");
	changed = true;
      }
    }

    if ((result = compare_strings(attrname, mxmlGetOpaque(attr_node))) > 0)
      continue;
    else if (result < 0)
      break;

    if ((value_node = mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST)) == NULL)
      continue;

    if (enumval)
    {
      if ((result = compare_strings(enumval, mxmlGetOpaque(value_node))) > 0)
      {
        continue;
      }
      else if (result < 0)
      {
	break;
      }
      else if (result == 0 && keyword && strstr(keyword, "(deprecated)") != NULL)
      {
	record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE);
	break;
      }
    }
    else if ((result = compare_strings(keyword, mxmlGetOpaque(value_node))) > 0)
    {
      continue;
    }
    else if (result < 0)
    {
      break;
    }

    // OK, at this point the current record matches the value we are trying to
    // add.
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    name      = mxmlGetOpaque(name_node);

    if (update_xref(record_node, logfile, xref, xrefname))
      changed = true;

    if (enumval)
    {
      // Replace the name with the new value...
      if (!name_node)
      {
        Enums.updated ++;
	fprintf(logfile, "Fixing bad enum record %s for attribute '%s'.\n", enumval, attrname);
	name_node = mxmlNewElement(record_node, "name");
      }

      if (!name)
      {
	mxmlNewOpaque(name_node, keyword);
      }
      else
      {
        Enums.updated ++;
	fprintf(logfile, "Renaming keyword for enum record %s for attribute '%s'.\n", enumval, attrname);
	mxmlSetOpaque(name_node, keyword);
      }

      return (true);
    }

    if (enumval)
    {
      Enums.ignored ++;
      fprintf(logfile, "Duplicate enum value %s for attribute '%s'.\n", enumval, attrname);
    }
    else
    {
      Keywords.ignored ++;
      fprintf(logfile, "Duplicate keyword value '%s' for attribute '%s'.\n", keyword, attrname);
    }

    return (changed);
  }

  // If we get here the value does not exist.  Build a new record and add it to
  // the registry in the right place.
  if (enumval)
  {
    Enums.added ++;
    fprintf(logfile, "Adding enum %s '%s' for '%s (%s)'...\n", enumval, keyword, attrname, syntax);
  }
  else
  {
    Keywords.added ++;
    fprintf(logfile, "Adding keyword '%s' for '%s (%s)'...\n", keyword, attrname, syntax);
  }

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
    // All new media registrations are for size names now...
    mxml_node_t *type_node = mxmlNewElement(node, "type");
    mxmlNewOpaque(type_node, "size name");
  }

  attr_node = mxmlNewElement(node, "attribute");
  mxmlNewOpaque(attr_node, attrname);

  syntax_node = mxmlNewElement(node, "syntax");
  mxmlNewOpaque(syntax_node, syntax);

  update_xref(node, logfile, xref, xrefname);

  mxmlAdd(registry_node, record_node ? MXML_ADD_BEFORE : MXML_ADD_AFTER, record_node, node);

  return (true);
}


//
// 'compare_record()' - Compare two registry records...
//

static int				// O - Result of comparison
compare_record(reg_record_t *a,		// I - First record
	       reg_record_t *b)		// I - Second record
{
  size_t	i;			// Looping var
  int		result;			// Result of comparison


  for (i = 0; i < (sizeof(a->keys) / sizeof(a->keys[0])); i ++)
  {
    if ((result = compare_strings(a->keys[i], b->keys[i])) != 0)
      return (result);
  }

  return (0);
}


//
// 'compare_strings()' - Compare two strings.
//
// This function basically does a strcmp() of the two strings, but is also
// aware of numbers so that "a2" < "a100".
//

static int				// O - Result of comparison
compare_strings(const char *s,		// I - First string
		const char *t)		// I - Second string
{
  int		diff;			// Difference between digits


  if (!s)
  {
    if (t)
      return (-1);
    else
      return (0);
  }
  else if (!t)
    return (1);

  // Loop through both names, returning only when a difference is seen.  Also,
  // compare whole numbers rather than just characters, too!
  while (*s && *t)
  {
    if (isdigit(*s & 255) && isdigit(*t & 255))
    {
      // Got numbers; scan the numbers and compare...
      long sn = strtol(s, (char **)&s, 0);
      long tn = strtol(t, (char **)&t, 0);

      if (sn < tn)
	return (-1);
      else if (sn > tn)
        return (1);
    }
    else if ((diff = *s - *t) != 0)
    {
      return (diff);
    }
    else
    {
      s ++;
      t ++;
    }
  }

  // Return the results of the final comparison...
  if (*s)
    return (1);
  else if (*t)
    return (-1);
  else
    return (0);
}


//
// 'node_name()' - Return the name associated with a registration node.
//

static const char *			// O - Node name
node_name(mxml_node_t *node)		// I - Node
{
  mxml_node_t	*name_node;		// Name node, if any
  const char	*name;			// Name string, if any


  name_node = mxmlFindElement(node, node, "name", NULL, NULL, MXML_DESCEND_FIRST);
  name      = mxmlGetOpaque(name_node);

  return (name ? name : "unknown");
}


//
// 'read_text()' - Read text registration data from the file.
//

static bool				// O - `true` if something was added, `false` if not
read_text(mxml_node_t *xml,		// I - XML registration document
	  FILE	      *textfile,	// I - Text registration file
	  FILE        *logfile,		// I - Log file
	  const char  *title,		// I - Standard title
	  const char  *xref)		// I - Standard URL
{
  bool		changed = false;	// Was something changed?
  int		i,			// Looping var
		blanks = 0,		// Number of blank lines
		state = IN_NOTHING,	// Current state
		linenum = 0,		// Current line number
		paren = 0;		// Parenthesis level
  char		line[1024],		// Line from text registration file
		*ptr,			// Pointer into line
		*first,			// First value
		*second,		// Second value
		attrname[128],		// Current attribute name
		membername[128],	// Current member attribute name
		syntax[128];		// Current syntax
  const char	*xrefname;		// Standard number
  static struct
  {
    const char	*header;		// Header string
    size_t	headerlen;		// Header length
  }	states[] =	// Headers for each group
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
    { "Value	Status Code Name", 25 },
    { "Object Name", 11 },
    { "Attribute Group Value", 21 },
    { "Out-of-Band Value", 17 }
  };
  static const char * const collections[] =
  {					// Collection names for each state
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


  // Read lines from the text registration file and make changes as needed...
  memset(attrname, 0, sizeof(attrname));
  memset(membername, 0, sizeof(membername));
  memset(syntax, 0, sizeof(syntax));

  xrefname = xref_name(xref, title);

  while (fgets(line, sizeof(line), textfile))
  {
    linenum ++;

    // Strip "[something]"...
    if ((ptr = strchr(line, '[')) != NULL)
      *ptr = '\0';

    // Strip leading and trailing whitespace...
    ptr = line + strlen(line) - 1;
    while (ptr >= line)
    {
      if (isspace(*ptr & 255))
	*ptr-- = '\0';
      else
	break;
    }

    for (ptr = line; *ptr && isspace(*ptr & 255); ptr ++)
      ;

    // Skip blank lines and lines that start with "-"...
    if (!*ptr)
    {
      blanks ++;
      if (blanks > 1)
	state = IN_NOTHING;

      continue;
    }
    else if (*ptr == '-')
    {
      continue;
    }

    blanks = 0;

    // Look for section headings...
    for (i = IN_OPER_ATTRS; i < IN_MAX; i ++)
    {
      if (!strncmp(ptr, states[i].header, states[i].headerlen))
      {
	state	      = i;
	attrname[0]   = '\0';
	membername[0] = '\0';
	syntax[0]     = '\0';
	break;
      }
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

	    // Attributes are not indented, member attributes *are*.
	    first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;
	    if (!*ptr)
	    {
	      fprintf(stderr, "register: Bad attribute definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (isspace(*ptr & 255))
	      *ptr++ = '\0';

	    if (*ptr != '(')
	    {
	      fprintf(stderr, "register: Bad attribute definition on line %d.\n", linenum);
	      exit(1);
	    }

	    ptr ++;
	    second = ptr;
	    for (paren = 1; *ptr && paren > 0; ptr ++)
	    {
	      if (*ptr == '(')
	      {
		paren ++;
	      }
	      else if (*ptr == ')')
	      {
		paren --;
		if (paren == 0)
		  *ptr = '\0';
	      }
	    }

	    if (first == line)
	    {
	      // attribute syntax
	      changed |= add_attr(xml, logfile, collections[state], first, NULL, NULL, second, xref, xrefname);

	      ipp_copy_string(attrname, first, sizeof(attrname));
	    }
	    else if ((first - 2) == line)
	    {
	      // member syntax
	      if (!attrname[0])
	      {
		fprintf(stderr, "register: Bad member attribute definition on line %d.\n", linenum);
		exit(1);
	      }

	      changed |= add_attr(xml, logfile, collections[state], attrname, first, NULL, second, xref, xrefname);

	      ipp_copy_string(membername, first, sizeof(membername));
	    }
	    else
	    {
	      // sub-member syntax
	      if (!attrname[0] || !membername[0])
	      {
		fprintf(stderr, "register: Bad sub-member attribute definition on line %d.\n", linenum);
		exit(1);
	      }

	      changed |= add_attr(xml, logfile, collections[state], attrname, membername, first, second, xref, xrefname);
	    }
	    break;

	case IN_KEYWORD_VALS :
	    if (*ptr != '<' && !islower(*ptr & 255))
	      continue;

	    // Attributes are not indented, keywords *are*.
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
	      fprintf(stderr, "register: Bad attribute/keyword definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (isspace(*ptr & 255))
	      *ptr++ = '\0';

	    if (first == line)
	    {
	      if (*ptr != '(')
	      {
		fprintf(stderr, "register: Bad attribute definition on line %d.\n", linenum);
		exit(1);
	      }

	      ptr ++;
	      second = ptr;
	      for (paren = 1; *ptr && paren > 0; ptr ++)
	      {
		if (*ptr == '(')
		{
		  paren ++;
		}
		else if (*ptr == ')')
		{
		  paren --;
		  if (paren == 0)
		    *ptr = '\0';
		}
	      }

	      // attribute syntax
	      changed |= add_valattr(xml, logfile, IPP_REGISTRY_KEYWORDS, first, second, xref, xrefname);

	      ipp_copy_string(attrname, first, sizeof(attrname));
	      ipp_copy_string(syntax, second, sizeof(syntax));
	    }
	    else if (!attrname[0])
	    {
	      fprintf(stderr, "register: Missing attribute before keyword value on line %d.\n", linenum);
	      exit(1);
	    }
	    else
	    {
	      // keyword
	      changed |= add_value(xml, logfile, NULL, first, attrname, syntax, xref, xrefname);
	    }
	    break;

	case IN_ENUM_VALS :
	    if (*ptr != '<' && !islower(line[0] & 255) && !isdigit(*ptr & 255))
	      continue;

	    // Attributes are not indented, enums *are*.
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
	      fprintf(stderr, "register: Bad attribute/enum definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (isspace(*ptr & 255))
	      *ptr++ = '\0';

	    if (first == line)
	    {
	      if (*ptr != '(')
	      {
		fprintf(stderr, "register: Bad attribute definition on line %d.\n", linenum);
		exit(1);
	      }

	      ptr ++;
	      second = ptr;
	      for (paren = 1; *ptr && paren > 0; ptr ++)
	      {
		if (*ptr == '(')
		{
		  paren ++;
		}
		else if (*ptr == ')')
		{
		  paren --;
		  if (paren == 0)
		    *ptr = '\0';
		}
	      }

	      // attribute syntax
	      changed |= add_valattr(xml, logfile, IPP_REGISTRY_ENUMS, first, second, xref, xrefname);

	      ipp_copy_string(attrname, first, sizeof(attrname));
	      ipp_copy_string(syntax, second, sizeof(syntax));
	    }
	    else if (!attrname[0])
	    {
	      fprintf(stderr, "register: Missing attribute before enum value on line %d.\n", linenum);
	      exit(1);
	    }
	    else if (!*ptr && first[0] != '<')
	    {
	      fprintf(stderr, "register: Bad enum definition on line %d.\n", linenum);
	      exit(1);
	    }
	    else
	    {
	      // enum keyword
	      for (second = ptr, ptr ++; *ptr && !isspace(*ptr); ptr ++);

	      *ptr = '\0';
	      if (first[0] == '<')
		second = NULL;

	      changed |= add_value(xml, logfile, first, second, attrname, syntax, xref, xrefname);
	    }
	    break;

	case IN_OPERATIONS :
	    if (!isupper(*ptr & 255))
	      continue;

	    first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;
	    if (!strncmp(ptr, "(extension)", 12))
	      ptr += 12;
	    *ptr = '\0';

	    changed |= add_operation(xml, logfile, first, xref, xrefname);
	    break;

	case IN_STATUS_CODES :
	    if (strncmp(ptr, "0x", 2) || strchr(ptr, ':'))
	      continue;

	    first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

	    if (!*ptr)
	    {
	      fprintf(stderr, "register: Bad status code definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (isspace(*ptr & 255))
	      *ptr++ = '\0';

	    second = ptr;
	    if (!islower(*ptr & 255))
	    {
	      fprintf(stderr, "register: Bad status code definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (*ptr && (islower(*ptr & 255) || *ptr == '-'))
	      ptr ++;
	    *ptr = '\0';

	    changed |= add_status_code(xml, logfile, first, second, xref, xrefname);
	    break;

	case IN_OBJECTS :
	    first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

	    *ptr = '\0';

	    changed |= add_object(xml, logfile, first, xref, xrefname);
	    break;

	case IN_ATTR_GROUP_VALS :
	    if (strncmp(ptr, "0x", 2) || strchr(ptr, ':'))
	      continue;

	    first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

	    if (!*ptr)
	    {
	      fprintf(stderr, "register: Bad attribute group value definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (isspace(*ptr & 255))
	      *ptr++ = '\0';

	    second = ptr;
	    if (!islower(*ptr & 255))
	    {
	      fprintf(stderr, "register: Bad attribute group value definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (*ptr && (islower(*ptr & 255) || *ptr == '-'))
	      ptr ++;
	    *ptr = '\0';

	    changed |= add_attr_group(xml, logfile, first, second, xref, xrefname);
	    break;

	case IN_OUT_OF_BAND_VALS :
	    if (strncmp(ptr, "0x", 2) || strchr(ptr, ':'))
	      continue;

	    first = ptr;
	    while (*ptr && !isspace(*ptr & 255))
	      ptr ++;

	    if (!*ptr)
	    {
	      fprintf(stderr, "register: Bad out-of-band value definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (isspace(*ptr & 255))
	      *ptr++ = '\0';

	    second = ptr;
	    if (!islower(*ptr & 255))
	    {
	      fprintf(stderr, "register: Bad out-of-band value definition on line %d.\n", linenum);
	      exit(1);
	    }

	    while (*ptr && (islower(*ptr & 255) || *ptr == '-'))
	      ptr ++;
	    *ptr = '\0';

	    changed |= add_out_of_band(xml, logfile, first, second, xref, xrefname);
	    break;

	default :
	    break;
      }
    }
  }

  return (changed);
}


//
// 'update_xref()' - Add or update the xref for a node.
//

static bool				// O - `true` if something was added, `false` if not
update_xref(mxml_node_t *record_node,	// I - XML record
            FILE        *logfile,	// I - Log file
            const char  *xref,		// I - <xref> link
            const char  *xrefname)	// I - <xref> name
{
  bool		changed = false;	// Did something change?
  mxml_node_t	*xref_node;		// <xref> node
  const char	*xref_type;		// <xref> type


  // Create the <xref> node as needed...
  if ((xref_node = mxmlFindElement(record_node, record_node, "xref", NULL, NULL, MXML_DESCEND_FIRST)) == NULL)
    xref_node = mxmlNewElement(record_node, "xref");

  xref_type = mxmlElementGetAttr(xref_node, "type");

  if (!xref_type || strcmp(xref_type, "rfc"))
  {
    // Replace reference...
    changed = true;

    fprintf(logfile, "Updating reference for %s.\n", node_name(record_node));

    mxmlElementSetAttr(xref_node, "type", xrefname ? "uri" : "rfc");
    mxmlElementSetAttr(xref_node, "data", xref);

    if (xrefname)
    {
      // Set/add name
      if (mxmlGetFirstChild(xref_node))
        mxmlSetOpaque(xref_node, xrefname);
      else
	mxmlNewOpaque(xref_node, xrefname);
    }
    else if (mxmlGetFirstChild(xref_node))
    {
      // Clear name...
      mxmlDelete(mxmlGetFirstChild(xref_node));
    }
  }

  return (changed);
}


//
// 'usage()' - Show program usage.
//

static int				// O - Exit status
usage(const char *opt)			// I - Pointer to option or NULL
{
  if (opt)
    fprintf(stderr, "Unknown option '-%c'.\n", *opt);

  puts("\nUsage: ./register options filename.xml [filename.txt]\n");
  puts("Options:");
  puts("  -o newfile.xml	    New XML file instead of replacing filename.xml");
  puts("  -t Title		    Title of standard");
  puts("  -x {rfcNNNN|URL}	    URL or RFC for standard");
  return (1);
}


//
// 'validate_registry()' - Validate the sorting of a registry.
//

static bool				// O - `true` if changed, `false` otherwise...
validate_registry(
    mxml_node_t        *xml,		// I - XML registry
    const char         *registry,	// I - Registry to validate
    const char         *regname,	// I - Name for message output
    size_t             num_keys,	// I - Number of sorting keys
    const char * const *keys)		// I - Sorting keys
{
  size_t	i,			// Looping var
		num_records;		// Number of records
  bool		changed = false;	// Did something change?
  reg_record_t	*records,		// Records
		*record;		// Current record
  mxml_node_t	*registry_node,		// <registry> node
		*record_node,		// Current <record> node
		*key_node,		// Current key node
		*last_node,		// Last node in sequence
		*range_node,		// <range> node
		*value_node,		// <value> node
		*syntax_node;		// <syntax> node
  const char	*value,			// Value string
		*syntax;		// Syntax string
  char		*type3;			// Pointer to a "type3" definition


  // Find the registry...
  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", registry, MXML_DESCEND_ALL)) == NULL)
  {
    fprintf(stderr, "register: Unable to find registry '%s' (%s) in XML file.\n", registry, regname);
    exit(1);
  }

  // Scan for "type3" registration rules...
  for (range_node = mxmlFindElement(registry_node, registry_node, "range", NULL, NULL, MXML_DESCEND_ALL); range_node; range_node = mxmlFindElement(range_node, registry_node, "range", NULL, NULL, MXML_DESCEND_ALL))
  {
    value_node = mxmlFindElement(range_node, range_node, "value", NULL, NULL, MXML_DESCEND_ALL);
    value      = mxmlGetOpaque(value_node);

    if (value && !strcmp(value, "type3"))
    {
      fputs("Removing type3 registration rule.\n", stderr);
      mxmlDelete(range_node);
      range_node = registry_node;
      changed    = true;
    }
  }

  // Scan for "type3" registration syntaxes...
  for (syntax_node = mxmlFindElement(registry_node, registry_node, "syntax", NULL, NULL, MXML_DESCEND_ALL); syntax_node; syntax_node = mxmlFindElement(syntax_node, registry_node, "syntax", NULL, NULL, MXML_DESCEND_ALL))
  {
    syntax = mxmlGetOpaque(syntax_node);
    while (syntax && (type3 = strstr(syntax, "type3")) != NULL)
    {
      // Change type3 to type2...
      type3[4] = '2';
      changed  = true;
    }
  }

  // Figure out the number of records...
  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST), num_records = 0;
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE))
    num_records ++;

  if (Verbosity > 1)
    printf("Registry '%s' (%s) has %lu records.\n", registry, regname, (unsigned long)num_records);

  if (num_records == 0)
    return (0);

  records = calloc(num_records, sizeof(reg_record_t));

  // Get the records...
  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST), record = records;
       record_node;
       record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_DESCEND_NONE), record ++)
  {
    record->index = (size_t)(record - records);
    record->node  = record_node;

    for (i = 0; i < num_keys; i ++)
    {
      key_node = mxmlFindElement(record_node, record_node, keys[i], NULL, NULL, MXML_DESCEND_FIRST);
      if (!key_node && !strcmp(keys[i], "name"))
      {
	key_node = mxmlFindElement(record_node, record_node, "attribute", NULL, NULL, MXML_DESCEND_FIRST);

	if (key_node)
	{
	  fprintf(stderr, "Fixing record for '%s' to use <name>.\n", mxmlGetOpaque(key_node));
	  mxmlSetElement(key_node, "name");
	  changed = true;
	}
      }
      else if (!key_node && !strcmp(keys[i], "attribute"))
      {
	key_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);

	if (key_node)
	{
	  fprintf(stderr, "Fixing record for '%s' to use <attribute>.\n", mxmlGetOpaque(key_node));
	  mxmlSetElement(key_node, "attribute");
	  changed = true;
	}
      }

      record->keys[i] = mxmlGetOpaque(key_node);
    }
  }

  // Sort the records and see if anything changed...
  qsort(records, num_records, sizeof(reg_record_t), (int (*)(const void *, const void *))compare_record);

  for (i = 0, record = records; i < num_records; i ++, record ++)
  {
    if (record->index != i)
      break;
  }

  if (i < num_records)
  {
    fprintf(stderr, "Fixing sorting of records in '%s' (%s).\n", registry, regname);
    changed = true;

    last_node = mxmlFindElement(registry_node, registry_node, "note", NULL, NULL, MXML_DESCEND_FIRST);

    for (i = 0, record = records; i < num_records; i ++, record ++)
    {
      if (last_node)
	mxmlAdd(registry_node, MXML_ADD_AFTER, last_node, record->node);
      else
	mxmlAdd(registry_node, MXML_ADD_BEFORE, NULL, record->node);

      last_node = record->node;
    }
  }

  free(records);

  return (changed);
}


//
// 'ws_cb()' - Whitespace (save) callback...
//

static const char *			// O - Whitespace to output
ws_cb(void        *data,		// I - Callback data (not used)
      mxml_node_t *node,		// I - Current node
      mxml_ws_t	  ws)			// I - Where we are
{
  int		level;			// Indentation level
  const char	*name = mxmlGetElement(node);
					// Element name
  mxml_type_t	type = mxmlGetType(node);
					// Node type
  static const char *spaces = "                                        ";
					// 40 spaces


  (void)data;

  if (!name)
    name = "unknown";

  if (mxmlGetParent(node) && !strcmp(name, "note"))
    return (NULL);

  if (ws == MXML_WS_AFTER_OPEN)
  {
    if (type == MXML_TYPE_DECLARATION || type == MXML_TYPE_DIRECTIVE || !strcmp(name, "registry") || !strcmp(name, "record") || !strcmp(name, "range") || (!mxmlGetFirstChild(node) && strcmp(mxmlGetElement(mxmlGetParent(node)), "note")))
      return ("\n");
    else
      return (NULL);
  }
  else if (ws == MXML_WS_BEFORE_CLOSE)
  {
    if (strcmp(name, "registry") && strcmp(name, "record") && strcmp(name, "range"))
      return (NULL);
  }
  else if (ws == MXML_WS_AFTER_CLOSE)
  {
    return ("\n");
  }

  for (level = -4; node; node = mxmlGetParent(node))
    level += 2;

  if (level <= 0)
    return (NULL);
  else if (level > 40)
    return (spaces);
  else
    return (spaces + 40 - level);
}


//
// 'xref_name()' - Return the name to show for the referenced standard.
//

static const char *			// O - Name or NULL
xref_name(const char *xref,		// I - URL or "rfcNNNN" reference
	  const char *title)		// I - Title
{
  char	*ptr;				// Pointer into URL/name
  static char	name[128];		// Name


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
