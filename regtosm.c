/*
 * Program to convert the IPP registrations into a PWG Semantic Model schema.
 *
 * Usage:
 *
 *    ./regtosm [-a attribute-list] [-n namespace-url] [-s service-name]
 *              [-v version] filename.xml output-directory
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
#include <stdarg.h>


/* Common IPP registry stuff */
#include "ipp-registry.h"


/*
 * Local variables...
 */

typedef struct
{
  const char    *name,                  /* Name */
                *value;                 /* Value/Substitution */
} ipp_map_t;

static size_t	alloc_include = 0,	/* Attributes and values to include */
		num_include = 0;
static ipp_map_t *include_attributes = NULL;
static ipp_map_t exclude_attributes[] =	/* Attributes to exclude */
{
  { "cover-back-supported", NULL },
  { "cover-front-supported", NULL },
  { "destination-accesses-supported", NULL },
  { "destination-attributes-supported", NULL },
  { "destination-uris-supported", NULL },
  { "document-access-supported", NULL },
  { "document-creation-attributes-supported", NULL },
  { "document-format-details-supported", NULL },
  { "document-format-varying-attributes", NULL },
  { "fetch-document-attributes-supported", NULL },
  { "finishings-col-supported", NULL },
  { "input-attributes-supported", NULL },
  { "insert-sheet-supported", NULL },
  { "ipp-features-supported", NULL },
  { "ipp-versions-supported", NULL },
  { "job-accounting-sheets-supported", NULL },
  { "job-cover-back-supported", NULL },
  { "job-cover-front-supported", NULL },
  { "job-creation-attributes-supported", NULL },
  { "job-error-sheet-supported", NULL },
  { "job-finishings-col-supported", NULL },
  { "job-save-disposition-supported", NULL },
  { "job-settable-attributes-supported", NULL },
  { "job-sheets-col-supported", NULL },
  { "media-col-supported", NULL },
  { "media-key-supported", NULL },
  { "notify-attributes", NULL },
  { "notify-attributes-supported", NULL },
  { "operations-supported", NULL },
  { "output-attributes-supported", NULL },
  { "overrides-supported", NULL },
  { "pdl-init-file-supported", NULL },
  { "pdl-override-guaranteed-supported", NULL },
  { "preferred-attributes-supported", NULL },
  { "printer-get-attributes-supported", NULL },
  { "printer-kind", NULL }, /* FIXME - missing registrations for values */
  { "printer-settable-attributes-supported", NULL },
  { "proof-print-supported", NULL },
  { "repertoire-supported", NULL },
  { "save-info-supported", NULL },
  { "separator-sheets-supported", NULL },
  { "stitching-supported", NULL },
  { "user-defined-values-supported", NULL }
};
static ipp_map_t alt_attributes[] =     /* Capability/Configuration attributes */
{
  { "finishings-col-database", NULL },
  { "finishings-col-ready", NULL },
  { "finishings-ready", NULL },
  { "materials-col-database", NULL },
  { "materials-col-ready", NULL },
  { "media-col-database", NULL },
  { "media-col-ready", NULL },
  { "printer-alert", NULL },
  { "printer-alert-description", NULL },
  { "printer-input-tray", NULL },
  { "printer-finisher", NULL },
  { "printer-finisher-description", NULL },
  { "printer-finisher-supply", NULL },
  { "printer-finisher-supply-description", NULL },
  { "printer-output-tray", NULL },
  { "printer-supply", NULL },
  { "printer-supply-description", NULL }
};
static ipp_map_t map_types[] =          /* Overrides of type names */
{
  { "binding-reference-edge", "ReferenceEdgeWKV" },
  { "binding-reference-edge-supported", "ReferenceEdgeWKV" },
  { "current-page-order", "PageOrderWKV" },
  { "destination-attributes", "JobTicketType" },
  { "destination-mandatory-access-attributes", "ElementWKV" },
  { "fetch-status-code", "StatusCodeWKV" },
  { "folding-reference-edge", "ReferenceEdgeWKV" },
  { "folding-reference-edge-supported", "ReferenceEdgeWKV" },
  { "input-orientation-requested", "OrientationRequestedWKV" },
  { "input-orientation-requested-supported", "OrientationRequestedWKV" },
  { "input-quality", "PrintQualityWKV" },
  { "input-quality-supported", "PrintQualityWKV" },
  { "input-sides", "SidesWKV" },
  { "input-sides-supported", "SidesWKV" },
  { "job-accounting-output-bin", "OutputBinWKV" },
  { "job-mandatory-attributes", "ElementWKV" },
  { "material-color", "MediaColorWKV" },
  { "media-back-coating", "MediaCoatingWKV" },
  { "media-back-coating-supported", "MediaCoatingWKV" },
  { "media-front-coating", "MediaCoatingWKV" },
  { "media-front-coating-supported", "MediaCoatingWKV" },
  { "media-input-tray-check", "MediaWKV" },
  { "media-key", "NameType" },
  { "media-size-name", "MediaWKV" },
  { "media-source-feed-orientation", "OrientationRequestedWKV" },
  { "notify-status-code", "StatusCodeWKV" },
  { "notify-subscribed-event", "NotifyEventsWKV" },
  { "output-device-document-state", "DocumentStateWKV" },
  { "output-device-document-state-reasons", "DocumentStateReasonsWKV" },
  { "output-device-job-state", "JobStateWKV" },
  { "output-device-job-state-reasons", "JobStateReasonsWKV" },
  { "output-device-job-states", "JobStateWKV" },
  { "page-order-received", "PageOrderWKV" },
  { "page-order-received-supported", "PageOrderWKV" },
  { "preferred-attributes", "JobTicketType" },
  { "printer-mandatory-job-attributes", "ElementWKV" },
  { "punching-reference-edge", "ReferenceEdgeWKV" },
  { "punching-reference-edge-supported", "ReferenceEdgeWKV" },
  { "stitching-reference-edge", "ReferenceEdgeWKV" },
  { "stitching-reference-edge-supported", "ReferenceEdgeWKV" },
  { "trimming-reference-edge", "ReferenceEdgeWKV" },
  { "trimming-reference-edge-supported", "ReferenceEdgeWKV" }
};


/*
 * Local functions...
 */

static int              compare_map_both(ipp_map_t *a, ipp_map_t *b);
static int              compare_map_name(ipp_map_t *a, ipp_map_t *b);
static void             create_elements(mxml_node_t *xsdnode, mxml_node_t *registry_node, const char *service);
static void             create_collection(mxml_node_t *xsdnode, mxml_node_t *record_node, const char *smtype);
static void             create_service(mxml_node_t *xsdnode, mxml_node_t *commonnode, mxml_node_t *registry_node, const char *service);
static void		create_status_codes(mxml_node_t *xsdnode, mxml_node_t *registry_node);
static void             create_types(mxml_node_t *xsdnode);
static void		create_well_known_values(mxml_node_t *xsdnode, mxml_node_t *registry_node);
static FILE		*create_xsd_file(const char *directory, const char *name);
static mxml_node_t	*create_xsd_root(const char *nsurl, const char *version, const char *annotation, ...);
static ipp_map_t        *find_map_both(const char *name, const char *value, ipp_map_t *map, size_t mapsize);
static ipp_map_t        *find_map_name(const char *name, ipp_map_t *map, size_t mapsize);
static int		get_current_date(int *month);
static char		*get_sm_element(const char *ipp, char *sm, size_t smsize);
static char		*get_sm_name(const char *ipp, char *sm, size_t smsize);
static char		*get_sm_type(const char *ipp, int collection, char *sm, size_t smsize);
static int		load_attributes(const char *filename);
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
		*directory = NULL,	/* Output directory */
		*service = "Print";	/* Service name */
  mxml_node_t	*xml,			/* XML registration file top node */
		*registry_node;		/* Current registry node */
  FILE		*xmlfile;		/* XML registration file pointer */
  char		nsurl[1024] = "",	/* Namespace URL */
		version[256] = "",	/* Schema version */
		temp[1024];		/* Temporary string */
  mxml_node_t	*xsdnode,		/* XSD node */
                *commonnode;            /* PwgCommon.xsd node */
  FILE		*xsdfile;		/* XSD file pointer */


 /*
  * Map sure map arrays are properly sorted...
  */

  qsort(exclude_attributes, sizeof(exclude_attributes) / sizeof(exclude_attributes[0]), sizeof(ipp_map_t), (int (*)(const void *, const void *))compare_map_name);
  qsort(alt_attributes, sizeof(alt_attributes) / sizeof(alt_attributes[0]), sizeof(ipp_map_t), (int (*)(const void *, const void *))compare_map_name);
  qsort(map_types, sizeof(map_types) / sizeof(map_types[0]), sizeof(ipp_map_t), (int (*)(const void *, const void *))compare_map_name);

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
          case 'a' : /* -a attributes-file */
              i ++;
              if (i < argc)
              {
                load_attributes(argv[i]);
	      }
	      else
	      {
	        fputs("regtosm: Missing attributes list filename after '-a'.\n", stderr);
	        return (usage());
	      }
	      break;

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

          case 's' : /* -s service-name */
              i ++;
              if (i < argc)
              {
                service = argv[i];
	      }
	      else
	      {
	        fputs("regtosm: Missing service name after '-s'.\n", stderr);
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
  * Output common element definitions...
  */

  puts("Generating PwgCommon.xsd...");

  xsdnode = create_xsd_root(nsurl, version, "Semantic elements used in more than one sub-schema.", "PwgTypes.xsd", NULL);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) != NULL)
    create_elements(xsdnode, registry_node, service);

  if ((xsdfile = create_xsd_file(directory, "PwgCommon")) != NULL)
  {
    mxmlSaveFile(mxmlGetParent(xsdnode), xsdfile, save_cb);
    fclose(xsdfile);
  }
  else
    return (1);

  commonnode = xsdnode;

 /*
  * Output common type definitions...
  */

  puts("Generating PwgTypes.xsd...");

  xsdnode = create_xsd_root(nsurl, version, "Semantic types used for all elements.", "PwgWellKnownValues.xsd", NULL);

  create_types(xsdnode);

  if ((xsdfile = create_xsd_file(directory, "PwgTypes")) != NULL)
  {
    mxmlSaveFile(mxmlGetParent(xsdnode), xsdfile, save_cb);
    fclose(xsdfile);
  }
  else
    return (1);

  mxmlDelete(mxmlGetParent(xsdnode));

 /*
  * Output well-known value strings strings...
  */

  puts("Generating PwgWellKnownValues.xsd...");

  xsdnode = create_xsd_root(nsurl, version, "Well known values (i.e. keywords) used by semantic elements.", NULL);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ENUMS, MXML_DESCEND)) != NULL)
    create_well_known_values(xsdnode, registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       IPP_REGISTRY_KEYWORDS, MXML_DESCEND)) != NULL)
    create_well_known_values(xsdnode, registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_STATUS_CODES, MXML_DESCEND)) != NULL)
    create_status_codes(xsdnode, registry_node);

  if ((xsdfile = create_xsd_file(directory, "PwgWellKnownValues")) != NULL)
  {
    mxmlSaveFile(mxmlGetParent(xsdnode), xsdfile, save_cb);
    fclose(xsdfile);
  }
  else
    return (1);

  mxmlDelete(mxmlGetParent(xsdnode));

 /*
  * Output print service definitions...
  */

  printf("Generating %sService.xsd...\n", service);

  snprintf(temp, sizeof(temp), "%s Service Definition.", service);
  xsdnode = create_xsd_root(nsurl, version, temp, "PwgCommon.xsd", NULL);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id", IPP_REGISTRY_ATTRIBUTES, MXML_DESCEND)) != NULL)
    create_service(xsdnode, commonnode, registry_node, service);

  snprintf(temp, sizeof(temp), "%sService", service);

  if ((xsdfile = create_xsd_file(directory, temp)) != NULL)
  {
    mxmlSaveFile(mxmlGetParent(xsdnode), xsdfile, save_cb);
    fclose(xsdfile);
  }
  else
    return (1);

  mxmlDelete(mxmlGetParent(xsdnode));
  mxmlDelete(mxmlGetParent(commonnode));

  return (0);
}


/*
 * 'compare_map_name()' - Compare two map items by name.
 */

static int                              /* O - Result of comparison */
compare_map_name(ipp_map_t *a,          /* I - First item */
		 ipp_map_t *b)          /* I - Second item */
{
  return (strcmp(a->name, b->name));
}


/*
 * 'compare_map_both()' - Compare two map items by name and value.
 */

static int                              /* O - Result of comparison */
compare_map_both(ipp_map_t *a,          /* I - First item */
                 ipp_map_t *b)          /* I - Second item */
{
  int	result = strcmp(a->name, b->name);

  if (result)
    return (result);
  else
    return (strcmp(a->value, b->value));
}


/*
 * 'create_collection()' - Create a complex type representing a collection.
 */

static void
create_collection(
    mxml_node_t *xsdnode,               /* I - xs:schema node */
    mxml_node_t *record_node,           /* I - First record in collection */
    const char  *smtype)                /* I - Semantic model type name */
{
  mxml_node_t   *name_node,             /* name node */
                *syntax_node,           /* syntax node */
                *member_node,           /* membername node */
                *submember_node,        /* submembername node */
                *xs_element,            /* xs:element node */
                *xs_type = NULL,        /* xs:complexType or xs:simpleType node */
                *xs_sequence = NULL,    /* xs:sequence node */
                *xs_sequence2,          /* xs:sequence node */
                *xs_temp;               /* xs:element, xs:maxLength, or xs:whiteSpace node */
  const char    *name1,                 /* First name value */
                *name,                 /* name value */
                *syntax,                /* syntax value */
                *membername1,           /* First membername value */
                *membername,            /* membername value */
                *submembername;         /* submembername value */
  char          smelement[1024],        /* Semantic model element name */
                smtemp[1024];           /* Temporary names */
  const char    *smeltype;              /* Element type */


  name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
  name1     = mxmlGetOpaque(name_node);

  member_node = mxmlFindElement(record_node, record_node, "member_attribute", NULL, NULL, MXML_DESCEND_FIRST);
  membername1 = mxmlGetOpaque(member_node);

  while ((record_node = mxmlGetNextSibling(record_node)) != NULL)
  {
   /*
    * Return if the attribute name or the member attribute name changes...
    */

    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    name      = mxmlGetOpaque(name_node);

    if (!name || strcmp(name1, name))
      return;

    member_node = mxmlFindElement(record_node, record_node, "member_attribute", NULL, NULL, MXML_DESCEND_FIRST);
    membername  = mxmlGetOpaque(member_node);

    if (!membername || (membername1 && strcmp(membername1, membername)))
      return;

    submember_node = mxmlFindElement(record_node, record_node, "sub-member_attribute", NULL, NULL, MXML_DESCEND_FIRST);
    submembername  = mxmlGetOpaque(submember_node);

    if (!membername1 && submembername)
      continue;
    else if (membername1 && !submembername)
      continue;

    if (strstr(membername, "(extension)") || strstr(membername, "(deprecated)") || strstr(membername, "(obsolete)") || strstr(membername, "(under review)"))
      continue;

    if (find_map_name(membername, exclude_attributes, sizeof(exclude_attributes) / sizeof(exclude_attributes[0])))
      continue;

    if (submembername && find_map_name(submembername, exclude_attributes, sizeof(exclude_attributes) / sizeof(exclude_attributes[0])))
        continue;

    syntax_node = mxmlFindElement(record_node, record_node, "syntax", NULL, NULL, MXML_DESCEND_FIRST);
    syntax      = mxmlGetOpaque(syntax_node);

    if (!syntax)
      continue;

    if (membername1)
      get_sm_element(submembername, smelement, sizeof(smelement));
    else
      get_sm_element(membername, smelement, sizeof(smelement));

    if (strstr(syntax, "boolean"))
      smeltype = "xs:boolean";
    else if (strstr(syntax, "charset"))
      smeltype = "CharsetType";
    else if (strstr(syntax, "collection"))
    {
      if (membername1)
        get_sm_type(submembername, 1, smtemp, sizeof(smtemp));
      else
        get_sm_type(membername, 1, smtemp, sizeof(smtemp));

      smeltype = smtemp;

      if (!mxmlFindElement(xsdnode, xsdnode, "xs:complexType", "name", smeltype, MXML_DESCEND_FIRST))
        create_collection(xsdnode, record_node, smeltype);
    }
    else if (strstr(syntax, "dateTime"))
      smeltype = "xs:dateTime";
    else if (strstr(syntax, "enum") != NULL || strstr(syntax, "type2 keyword") != NULL)
    {
      if (membername1)
        get_sm_type(submembername, 0, smtemp, sizeof(smtemp));
      else
        get_sm_type(membername, 0, smtemp, sizeof(smtemp));

      smeltype = smtemp;
    }
    else if (strstr(syntax, "keyword"))
      smeltype = "xs:NMTOKEN";
    else if (strstr(syntax, "rangeOfInteger"))
      smeltype = "RangeOfIntType";
    else if (strstr(syntax, "integer"))
      smeltype = "xs:int";
    else if (strstr(syntax, "mimeMediaType"))
      smeltype = "MimeMediaType";
    else if (strstr(syntax, "name"))
      smeltype = "NameType";
    else if (strstr(syntax, "naturalLanguage"))
      smeltype = "NaturalLanguageType";
    else if (strstr(syntax, "octetString"))
      smeltype = "OctetStringType";
    else if (strstr(syntax, "resolution"))
      smeltype = "ResolutionType";
    else if (strstr(syntax, "text"))
      smeltype = "TextType";
    else if (strstr(syntax, "uri"))
      smeltype = "xs:anyURI";
    else
      return;

    if (!xs_sequence)
    {
     /*
      * Create top-level type...
      */

      xs_type = mxmlNewElement(xsdnode, "xs:complexType");
      mxmlElementSetAttr(xs_type, "name", smtype);

      xs_sequence = mxmlNewElement(xs_type, "xs:sequence");
    }

    xs_element = mxmlNewElement(xs_sequence, "xs:element");
    mxmlElementSetAttr(xs_element, "name", smelement);

    if (!strncmp(syntax, "1setOf ", 7))
    {
     /*
      * A sequence of 0 or more element values...
      */

      xs_type      = mxmlNewElement(xs_element, "xs:complexType");
      xs_sequence2 = mxmlNewElement(xs_type, "xs:sequence");
      xs_temp      = mxmlNewElement(xs_sequence2, "xs:element");
      mxmlElementSetAttrf(xs_temp, "name", "%sValue", smelement);
      mxmlElementSetAttr(xs_temp, "type", smeltype);
      mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");
    }
    else
    {
     /*
      * Just a single element value.
      */

      mxmlElementSetAttr(xs_element, "type", smeltype);
    }
  }
}


/*
 * 'create_element()' - Create a single element.
 */

static void
create_element(
    mxml_node_t *xsdnode,               /* I - xs:schema node */
    mxml_node_t *record_node)           /* I - Attribute record */
{
  mxml_node_t   *name_node,             /* name node */
                *syntax_node,           /* syntax node */
                *member_node,           /* member_attribute node */
                *xs_element,            /* xs:element node */
                *xs_type,               /* xs:complexType or xs:simpleType node */
                *xs_sequence,           /* xs:sequence node */
                *xs_temp;               /* xs:element, xs:maxLength, or xs:whiteSpace node */
  const char    *name,                  /* name value */
                *syntax;                /* syntax value */
  char          smname[1024],           /* Semantic model element name */
                smtemp[1024];           /* Semantic model name */
  const char    *smtype;                /* Semantic model type name */


 /*
  * IPP data types map to the following XML schema data types:
  *
  *   boolean         -> xs:boolean
  *   charset         -> CharsetType
  *   collection      -> xs:complexType
  *   enum            -> AttributeNameWKV
  *   integer         -> xs:int
  *   keyword         -> AttributeNameWKV
  *   mimeMediaType   -> MimeMediaType
  *   name            -> NameType
  *   naturalLanguage -> NaturalLanguageType
  *   octetString     -> OctetStringType
  *   rangeOfInteger  -> RangeOfIntType
  *   resolution      -> ResolutionType
  *   text            -> TextType
  *   uri             -> xs:anyURI
  */

  name_node   = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
  name        = mxmlGetOpaque(name_node);
  member_node = mxmlFindElement(record_node, record_node, "member_attribute", NULL, NULL, MXML_DESCEND_FIRST);
  syntax_node = mxmlFindElement(record_node, record_node, "syntax", NULL, NULL, MXML_DESCEND_FIRST);
  syntax      = mxmlGetOpaque(syntax_node);

  if (!name || !syntax)
    return;

  if (member_node)
    return;

  if (strstr(name, "-actual") || strstr(name, "-completed") || strstr(name, "-default") || strstr(name, "(extension)") || strstr(name, "(deprecated)") || strstr(name, "(obsolete)") || strstr(name, "(under review)"))
    return;

  if (num_include && !find_map_both(name, "", include_attributes, num_include))
    return;				/* Only include listed attributes */

  if (find_map_name(name, exclude_attributes, sizeof(exclude_attributes) / sizeof(exclude_attributes[0])))
    return;				/* Skip excluded attributes */

  get_sm_element(name, smname, sizeof(smname));

  if (strstr(syntax, "boolean"))
    smtype = "xs:boolean";
  else if (strstr(syntax, "charset"))
    smtype = "CharsetType";
  else if (strstr(syntax, "collection"))
  {
    get_sm_type(name, 1, smtemp, sizeof(smtemp));

    smtype = smtemp;

    if (!mxmlFindElement(xsdnode, xsdnode, "xs:complexType", "name", smtype, MXML_DESCEND_FIRST))
      create_collection(xsdnode, record_node, smtype);
  }
  else if (strstr(syntax, "dateTime"))
    smtype = "xs:dateTime";
  else if (strstr(syntax, "enum") != NULL || strstr(syntax, "type2 keyword") != NULL)
  {
    get_sm_type(name, 0, smtemp, sizeof(smtemp));

    smtype = smtemp;
  }
  else if (strstr(syntax, "keyword"))
    smtype = "xs:NMTOKEN";
  else if (strstr(syntax, "rangeOfInteger"))
    smtype = "RangeOfIntType";
  else if (strstr(syntax, "integer"))
    smtype = "xs:int";
  else if (strstr(syntax, "mimeMediaType"))
    smtype = "MimeMediaType";
  else if (strstr(syntax, "name"))
    smtype = "NameType";
  else if (strstr(syntax, "naturalLanguage"))
    smtype = "NaturalLanguageType";
  else if (strstr(syntax, "octetString"))
    smtype = "OctetStringType";
  else if (strstr(syntax, "resolution"))
    smtype = "ResolutionType";
  else if (strstr(syntax, "text"))
    smtype = "TextType";
  else if (strstr(syntax, "uri"))
    smtype = "xs:anyURI";
  else
    return;

  if (mxmlFindElement(xsdnode, xsdnode, "xs:element", "name", smname, MXML_DESCEND_FIRST) != NULL)
    return;

  xs_element = mxmlNewElement(xsdnode, "xs:element");
  mxmlElementSetAttr(xs_element, "name", smname);

  if (!strncmp(syntax, "1setOf ", 7))
  {
   /*
    * A sequence of 0 or more element values...
    */

    xs_type     = mxmlNewElement(xs_element, "xs:complexType");
    xs_sequence = mxmlNewElement(xs_type, "xs:sequence");
    xs_temp     = mxmlNewElement(xs_sequence, "xs:element");
    mxmlElementSetAttrf(xs_temp, "name", "%sValue", smname);
    mxmlElementSetAttr(xs_temp, "type", smtype);
    mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");
  }
  else
  {
   /*
    * Just a single element value.
    */

    mxmlElementSetAttr(xs_element, "type", smtype);
  }
}


/*
 * 'create_elements()' - Create all of the common elements and their types.
 */

static void
create_elements(
    mxml_node_t *xsdnode,               /* I - xs:schema node */
    mxml_node_t *registry_node,         /* I - Attribute registry */
    const char  *service)               /* I - Service name */
{
  mxml_node_t   *record_node;           /* Current attribute record */


 /*
  * Loop through all attributes to define them and any types...
  */

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_NO_DESCEND))
    create_element(xsdnode, record_node);
}


/*
 * 'create_service()' - Create a service definition from the common types.
 */

static void
create_service(
    mxml_node_t *xsdnode,               /* I - xs:schema node */
    mxml_node_t *commonnode,            /* I - PwgCommon.xsd node */
    mxml_node_t *registry_node,         /* I - Attribute registry */
    const char  *service)               /* I - Service name */
{
  int           i;                      /* Looping var */
  mxml_node_t   *xs_type,               /* xs:complexType node */
                *xs_type2,              /* Secondary xs:complexType node */
                *xs_sequence,           /* xs:sequence node */
                *xs_sequence2,          /* Secondary xs:sequence node */
                *xs_temp;               /* Other xs:... nodes */
  mxml_node_t   *record_node;           /* Current attribute record */
  mxml_node_t   *collection_node;       /* Current collection node */
  const char    *collection;            /* Current collection value */
  ipp_map_t     *type;                  /* Current type */
  static ipp_map_t types[] =            /* Map collections to types */
  {
    { "Document Description", "DocumentDescriptionType" },
    { "Document Status", "DocumentStatusType" },
    { "Document Template", "DocumentProcessingType" },
    { "Job Description", "JobDescriptionType" },
    { "Job Status", "JobStatusType" },
    { "Job Template", "JobProcessingType" },
    { "Printer Description", "ServiceDescriptionType" }, /* Also ServiceCapabilitiesType */
    { "Printer Status", "ServiceStatusType" }           /* Also ServiceConfigurationType */
  };


 /*
  * Create the core types for each attribute group...
  */

  for (i = (int)(sizeof(types) / sizeof(types[0])), type = types; i > 0; i --, type ++)
  {
    mxml_node_t *xs_altsequence;
    mxml_node_t *name_node;
    const char *name;
    char smelement[1024];

    xs_type = mxmlNewElement(xsdnode, "xs:complexType");
    mxmlElementSetAttrf(xs_type, "name", "%s%s", service, type->value);

    xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

    if (!strcmp(type->name, "Printer Description"))
    {
      xs_type = mxmlNewElement(xsdnode, "xs:complexType");
      mxmlElementSetAttrf(xs_type, "name", "%sServiceCapabilitiesType", service);

      xs_altsequence = mxmlNewElement(xs_type, "xs:sequence");
    }
    else if (!strcmp(type->name, "Printer Status"))
    {
      xs_type = mxmlNewElement(xsdnode, "xs:complexType");
      mxmlElementSetAttrf(xs_type, "name", "%sServiceConfigurationType", service);

      xs_altsequence = mxmlNewElement(xs_type, "xs:sequence");
    }
    else
      xs_altsequence = NULL;

    for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_NO_DESCEND))
    {
      collection_node = mxmlFindElement(record_node, record_node, "collection", NULL, NULL, MXML_DESCEND_FIRST);
      collection      = mxmlGetOpaque(collection_node);

      if (strcmp(collection, type->name))
        continue;

      if (mxmlFindElement(record_node, record_node, "member_attribute", NULL, NULL, MXML_DESCEND_FIRST))
        continue;

      name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
      name      = mxmlGetOpaque(name_node);

      if (!name)
        continue;

      if (strstr(name, "-actual") || strstr(name, "-completed") || strstr(name, "-default") || strstr(name, "(extension)") || strstr(name, "(deprecated)") || strstr(name, "(obsolete)") || strstr(name, "(under review)"))
        continue;

      if (num_include && !find_map_both(name, "", include_attributes, num_include))
	continue;			/* Only include listed attributes */

      if (find_map_name(name, exclude_attributes, sizeof(exclude_attributes) / sizeof(exclude_attributes[0])))
        continue;

      if (xs_altsequence && strstr(name, "-supported"))
      {
       /*
        * See if this is a -supported attribute for a Job Template attribute...
        */

        char name2[1024], *nameptr;
        mxml_node_t *job_template_node = NULL;

        if (!strncmp(name, "max-", 4))
          strncpy(name2, name + 4, sizeof(name2) - 1);
        else
          strncpy(name2, name, sizeof(name2) - 1);
        name2[sizeof(name2) - 1] = '\0';

        if ((nameptr = strstr(name2, "-supported")) != NULL)
        {
          *nameptr = '\0';
          job_template_node = mxmlFindElement(commonnode, commonnode, "xs:element", "name", get_sm_element(name2, smelement, sizeof(smelement)), MXML_DESCEND);
        }
        if (job_template_node)
          xs_temp = mxmlNewElement(xs_altsequence, "xs:element");
        else
          xs_temp = mxmlNewElement(xs_sequence, "xs:element");
      }
      else if (xs_altsequence && find_map_name(name, alt_attributes, sizeof(alt_attributes) / sizeof(alt_attributes[0])))
        xs_temp = mxmlNewElement(xs_altsequence, "xs:element");
      else
        xs_temp = mxmlNewElement(xs_sequence, "xs:element");

      mxmlElementSetAttr(xs_temp, "ref", get_sm_element(name, smelement, sizeof(smelement)));
    }

   /*
    * Extension points...
    */

    xs_temp = mxmlNewElement(xs_sequence, "xs:any");
    mxmlElementSetAttr(xs_temp, "namespace", "##other");
    mxmlElementSetAttr(xs_temp, "minOccurs", "0");
    mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

    if (xs_altsequence)
    {
      xs_temp = mxmlNewElement(xs_altsequence, "xs:any");
      mxmlElementSetAttr(xs_temp, "namespace", "##other");
      mxmlElementSetAttr(xs_temp, "minOccurs", "0");
      mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");
    }
  }

 /*
  * <service>DocumentTicketType...
  */

  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttrf(xs_type, "name", "%sDocumentTicketType", service);

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocumentDescription", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentDescriptionType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocumentProcessing", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentProcessingType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

 /*
  * <service>JobTicketType...
  */

  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttrf(xs_type, "name", "%sJobTicketType", service);

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJobDescription", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobDescriptionType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJobProcessing", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobProcessingType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");  /* <service>DocumentType */
  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttrf(xs_type, "name", "%sDocumentType", service);

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocumentReceipt", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentTicketType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocumentStatus", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentStatusType", service);

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocumentTicket", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentTicketType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  /* <service>JobType */
  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttrf(xs_type, "name", "%sJobType", service);

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJobReceipt", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobTicketType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJobStatus", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobStatusType", service);

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJobTicket", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobTicketType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocuments", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_type2     = mxmlNewElement(xs_temp, "xs:complexType");
  xs_sequence2 = mxmlNewElement(xs_type2, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence2, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sDocument", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  xs_temp = mxmlNewElement(xs_sequence, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  /* <service>JobsType */
  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttrf(xs_type, "name", "%sJobsType", service);

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJob", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  /* <service>ServiceType */
  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttrf(xs_type, "name", "%sServiceType", service);

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sServiceCapabilities", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sServiceCapabilitiesType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sServiceConfiguration", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sServiceConfigurationType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sServiceDefaults", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_type2     = mxmlNewElement(xs_temp, "xs:complexType");
  xs_sequence2 = mxmlNewElement(xs_type2, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence2, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "Default%sDocumentTicket", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sDocumentTicketType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence2, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "Default%sJobTicket", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sJobTicketType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence2, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sServiceDescription", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sServiceDescriptionType", service);

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sServiceStatus", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sServiceStatusType", service);

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sJobTable", service);

  xs_type2     = mxmlNewElement(xs_temp, "xs:complexType");
  xs_sequence2 = mxmlNewElement(xs_type2, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence2, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "ActiveJobs");
  mxmlElementSetAttrf(xs_temp, "type", "%sJobsType", service);

  xs_temp = mxmlNewElement(xs_sequence2, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "JobHistory");
  mxmlElementSetAttrf(xs_temp, "type", "%sJobsType", service);
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");

  xs_temp = mxmlNewElement(xs_sequence2, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  xs_temp = mxmlNewElement(xs_sequence, "xs:any");
  mxmlElementSetAttr(xs_temp, "namespace", "##other");
  mxmlElementSetAttr(xs_temp, "minOccurs", "0");
  mxmlElementSetAttr(xs_temp, "maxOccurs", "unbounded");

  /* <service>Service element */
  xs_temp = mxmlNewElement(xsdnode, "xs:element");
  mxmlElementSetAttrf(xs_temp, "name", "%sService", service);
  mxmlElementSetAttrf(xs_temp, "type", "%sServiceType", service);
}


/*
 * 'create_status_codes()' - Create the well-known values nodes for status codes.
 */

static void
create_status_codes(
    mxml_node_t *xsdnode,		/* I - xs:schema node */
    mxml_node_t *registry_node)		/* I - IPP registry node */
{
  mxml_node_t	*record_node,		/* Current record node */
		*name_node;		/* Name node */
  const char    *name;                  /* Name string */
  char		smname[1024];		/* SM name */
  mxml_node_t	*xs_simpleType,		/* Simple type for well-known value */
		*xs_restriction,	/* Restrictions on simpleType */
		*xs_maxLength,		/* Maximum length of value */
		*xs_enumeration;	/* Enumerated value */



  xs_simpleType = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_simpleType, "name", "StatusCodeWKV");

  xs_restriction = mxmlNewElement(xs_simpleType, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

  xs_maxLength = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_maxLength, "value", "255");

  for (record_node = mxmlFindElement(registry_node, registry_node, "record", NULL, NULL, MXML_DESCEND_FIRST); record_node; record_node = mxmlFindElement(record_node, registry_node, "record", NULL, NULL, MXML_NO_DESCEND))
  {
    name_node = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    name      = mxmlGetOpaque(name_node);

    if (name && islower(*name & 255))
    {
      xs_enumeration = mxmlNewElement(xs_restriction, "xs:enumeration");
      mxmlElementSetAttr(xs_enumeration, "value", get_sm_name(name, smname, sizeof(smname)));
    }
  }
}


/*
 * 'create_types()' - Create all of the common types.
 */

static void
create_types(mxml_node_t *xsdnode)      /* I - xs:schema node */
{
  mxml_node_t   *xs_type,               /* xs:complexType or xs:simpleType node */
                *xs_restriction,        /* xs:restriction node */
                *xs_sequence,           /* xs:sequence node */
                *xs_temp;               /* xs:element, xs:maxLength, or xs:whiteSpace node */


  /* ElementWKV */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "ElementWKV");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "255");

  /* CharsetType */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "CharsetType");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "63");

  /* MimeMediaType */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "MimeMediaType");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "255");

  /* NameType */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "NameType");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:string");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "255");

  /* NaturalLanguageType */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "NaturalLanguageType");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "63");

  /* OctetStringType */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "OctetStringType");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:base64Binary");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "43690");

  /* RangeOfIntType */
  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttr(xs_type, "name", "RangeOfIntType");

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "LowerBound");
  mxmlElementSetAttr(xs_temp, "type", "xs:int");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "UpperBound");
  mxmlElementSetAttr(xs_temp, "type", "xs:int");

  /* ResolutionType */
  xs_type = mxmlNewElement(xsdnode, "xs:complexType");
  mxmlElementSetAttr(xs_type, "name", "ResolutionType");

  xs_sequence = mxmlNewElement(xs_type, "xs:sequence");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "CrossFeedDir");
  mxmlElementSetAttr(xs_temp, "type", "xs:int");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "FeedDir");
  mxmlElementSetAttr(xs_temp, "type", "xs:int");

  xs_temp = mxmlNewElement(xs_sequence, "xs:element");
  mxmlElementSetAttr(xs_temp, "name", "Units");
  mxmlElementSetAttr(xs_temp, "type", "UnitsWKV");

  /* UnitsWKV */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "UnitsWKV");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "255");

  xs_temp = mxmlNewElement(xs_restriction, "xs:enumeration");
  mxmlElementSetAttr(xs_temp, "value", "Dpi");

  xs_temp = mxmlNewElement(xs_restriction, "xs:enumeration");
  mxmlElementSetAttr(xs_temp, "value", "Dpcm");

  /* TextType */
  xs_type = mxmlNewElement(xsdnode, "xs:simpleType");
  mxmlElementSetAttr(xs_type, "name", "TextType");

  xs_restriction = mxmlNewElement(xs_type, "xs:restriction");
  mxmlElementSetAttr(xs_restriction, "base", "xs:string");

  xs_temp = mxmlNewElement(xs_restriction, "xs:maxLength");
  mxmlElementSetAttr(xs_temp, "value", "1023");
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
    name_node       = mxmlFindElement(record_node, record_node, "name", NULL, NULL, MXML_DESCEND_FIRST);
    value_node      = mxmlFindElement(record_node, record_node, "value", NULL, NULL, MXML_DESCEND_FIRST);

    if (attribute_node && value_node)
    {
      const char *attribute = mxmlGetOpaque(attribute_node),
                 *value = mxmlGetOpaque(name_node);

      if (!value)
        value = mxmlGetOpaque(value_node);

      if (num_include > 0 && !find_map_both(attribute, value, include_attributes, num_include) && !find_map_both(attribute, "*", include_attributes, num_include))
        continue;			/* Skip values not explicitly listed */

      if (value[0] == '<' || strstr(value, "(deprecated)") != NULL || strstr(value,
      "(obsolete)") != NULL)
        continue;                       /* Skip "< ... >" values */

      if (strstr(attribute, "-default") != NULL || strstr(attribute, "-ready") != NULL)
        continue;                       /* Skip -default and -ready values */

      if (find_map_name(attribute, exclude_attributes, sizeof(exclude_attributes) / sizeof(exclude_attributes[0])))
        continue;                       /* Skip excluded attributes */

      if (!last_attribute || strcmp(last_attribute, attribute))
      {
       /* Start a new xs:simpleType */
        last_attribute = attribute;

        get_sm_type(attribute, 0, smname, sizeof(smname));

        xs_simpleType = mxmlNewElement(xsdnode, "xs:simpleType");
        mxmlElementSetAttr(xs_simpleType, "name", smname);

	xs_restriction = mxmlNewElement(xs_simpleType, "xs:restriction");
	mxmlElementSetAttr(xs_restriction, "base", "xs:NMTOKEN");

	xs_maxLength = mxmlNewElement(xs_restriction, "xs:maxLength");
	mxmlElementSetAttr(xs_maxLength, "value", "255");
      }

      xs_enumeration = mxmlNewElement(xs_restriction, "xs:enumeration");
      if (!strcmp(attribute, "media") && strchr(value, '_') != NULL)
        mxmlElementSetAttr(xs_enumeration, "value", value);
      else
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
                const char *annotation, /* I - Comment for header */
                ...)                    /* I - Additional include paths */
{
  mxml_node_t	*root,			/* Root node */
		*xs_schema,		/* xs:schema node */
		*xs_include,            /* xs:include node */
		*xs_documentation,	/* xs:documentation node */
		*xs_annotation;		/* xs:annotation node */
  char		header[2048];		/* Standard header text */
  va_list       ap;                     /* Additional arguments */
  const char    *include;               /* Include file */


  root      = mxmlNewXML("1.0");
  xs_schema = mxmlNewElement(root, "xs:schema");

  mxmlElementSetAttr(xs_schema, "xmlns", nsurl);
  mxmlElementSetAttr(xs_schema, "xmlns:xs", "http://www.w3.org/2001/XMLSchema");
  mxmlElementSetAttr(xs_schema, "xmlns:pwg", nsurl);
  mxmlElementSetAttr(xs_schema, "targetNamespace", nsurl);
  mxmlElementSetAttr(xs_schema, "elementFormDefault", "qualified");
  mxmlElementSetAttr(xs_schema, "attributeFormDefault", "qualified");
  mxmlElementSetAttr(xs_schema, "version", version);

  va_start(ap, annotation);
  while ((include = va_arg(ap, const char *)) != NULL)
  {
    xs_include = mxmlNewElement(xs_schema, "xs:include");
    mxmlElementSetAttr(xs_include, "schemaLocation", include);
  }
  va_end(ap);

  xs_annotation    = mxmlNewElement(xs_schema, "xs:annotation");
  xs_documentation = mxmlNewElement(xs_annotation, "xs:documentation");

  snprintf(header, sizeof(header),
           "PWG Semantic Model v%s\n"
           "Copyright &copy; 2002-%d The IEEE-ISTO Printer Working Group.\n\n"
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

  mxmlNewOpaque(xs_documentation, header);

  if (annotation)
  {
    xs_documentation = mxmlNewElement(xs_annotation, "xs:documentation");

    mxmlNewOpaque(xs_documentation, annotation);
  }

  return (xs_schema);
}


/*
 * 'find_map_both()' - Find a map item using a name and value.
 */

static ipp_map_t *                      /* O - Matching element or NULL */
find_map_both(const char *name,         /* I - Name to lookup */
              const char *value,	/* I - Value to lookup */
	      ipp_map_t  *map,          /* I - Map */
	      size_t     mapsize)       /* I - Number of elements in map */
{
  ipp_map_t     key;                    /* Search key */


  key.name  = name;
  key.value = value ? value : "";

  return ((ipp_map_t *)bsearch(&key, map, mapsize, sizeof(ipp_map_t), (int (*)(const void *, const void *))compare_map_both));
}


/*
 * 'find_map_name()' - Find a map item using a name.
 */

static ipp_map_t *                      /* O - Matching element or NULL */
find_map_name(const char *name,         /* I - Name to lookup */
	      ipp_map_t  *map,          /* I - Map */
	      size_t     mapsize)       /* I - Number of elements in map */
{
  ipp_map_t     key;                    /* Search key */


  key.name = name;

  return ((ipp_map_t *)bsearch(&key, map, mapsize, sizeof(ipp_map_t), (int (*)(const void *, const void *))compare_map_name));
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
 * 'get_sm_element()' - Convert an IPP attribute name into a Semantic Model element name.
 *
 * Follows the rules from PWG PJT 1.0.
 */

static char *				/* O - SM name */
get_sm_element(const char *ipp,		/* I - IPP keyword/name */
               char       *sm,		/* I - SM name buffer */
               size_t     smsize)	/* I - Size of name buffer */
{
  char	*smptr = sm, *smend = sm + smsize - 1;
					/* Pointer into SM name buffer and end */


 /*
  * Rule 2: Strip leading "ipp-"...
  * Rule 6: Remove "job-", "document-", and "printer-" prefix from common Job, Document, and Printer attributes.
  */

  if (!strncmp(ipp, "ipp-", 4))
    ipp += 4;
  else if (!strncmp(ipp, "job-k-octets", 12) || !strncmp(ipp, "job-media-sheets", 16) || !strncmp(ipp, "job-impressions", 15))
    ipp += 4;
  else if (!strncmp(ipp, "printer-resolution", 18))
    ipp += 8;
  else if (!strncmp(ipp, "printer-", 8))
  {
   /*
    * Rule 6: Replace "printer-" prefix with "Service".
    */

    strncpy(smptr, "Service", smend - smptr);
    *smend = '\0';
    smptr += strlen(smptr);
    ipp += 8;
  }

 /*
  * Rule 4: Convert "foo-bar-bla" into "FooBarBla".
  */

  *smptr++ = toupper(*ipp++);

  while (*ipp && smptr < smend)
  {
    if (!strncmp(ipp, "-attribute", 10))
    {
     /*
      * Rule 3: Replace "-attribute" with "Element"...
      */

      strncpy(smptr, "Element", smend - smptr);
      *smend = '\0';
      smptr += strlen(smptr);
      ipp += 10;
    }
    else if (*ipp == '-' && ipp[1])
    {
      if (smptr > sm && isdigit(smptr[-1]) && isdigit(ipp[1]))
        *smptr++ = '_';

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
 * 'get_sm_name()' - Convert an IPP keyword/enum into a Semantic Model token name.
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
      if (smptr > sm && isdigit(smptr[-1]) && isdigit(ipp[1]))
        *smptr++ = '_';

      ipp ++;
      *smptr++ = toupper(*ipp++);
    }
    else if (isspace(*ipp & 255))
    {
      ipp ++;
      *smptr++ = '_';
    }
    else
      *smptr++ = *ipp++;
  }

  *smptr = '\0';

  return (sm);
}


/*
 * 'get_sm_type()' - Convert an IPP keyword/enum into a Semantic Model type name.
 */

static char *				/* O - SM name */
get_sm_type(const char *ipp,		/* I - IPP keyword/name */
            int        collection,      /* I - Collection attribute? */
            char       *sm,		/* I - SM name buffer */
            size_t     smsize)		/* I - Size of name buffer */
{
  ipp_map_t     *type;                  /* Mapped type, if any */
  char	*smptr = sm, *smend = sm + smsize - 1;
					/* Pointer into SM name buffer and end */


 /*
  * Map types for attributes that don't follow a simple rule...
  */

  if ((type = find_map_name(ipp, map_types, sizeof(map_types) / sizeof(map_types[0]))) != NULL)
  {
    strncpy(sm, type->value, smsize - 1);
    *smend = '\0';

    return (sm);
  }

 /*
  * Rule 2: Strip leading "ipp-"...
  * Rule 6: Remove "job-", "document-", and "printer-" prefix from common Job, Document, and Printer attributes.
  */

  if (!strncmp(ipp, "ipp-", 4))
    ipp += 4;
  else if (!strncmp(ipp, "job-cover-", 10) || !strncmp(ipp, "job-finishings", 14) || !strncmp(ipp, "job-k-octets", 12) || !strncmp(ipp, "job-media-sheets", 16) || !strncmp(ipp, "job-impressions", 15))
    ipp += 4;
  else if (!strncmp(ipp, "printer-resolution", 18))
    ipp += 8;

 /*
  * Rule 4: Convert "foo-bar-bla" into "FooBarBla".
  */

  *smptr++ = toupper(*ipp++);

  while (*ipp && smptr < smend)
  {
    if (!strncmp(ipp, "printer-", 8))
    {
     /*
      * Rule 6: Replace "printer-" prefix with "Service".
      */

      strncpy(smptr, "Service", smend - smptr);
      *smend = '\0';
      smptr += strlen(smptr);
      ipp += 8;
    }
    else if (!strncmp(ipp, "-attribute", 10))
    {
     /*
      * Rule 3: Replace "-attribute" with "Element"...
      */

      strncpy(smptr, "Element", smend - smptr);
      *smend = '\0';
      smptr += strlen(smptr);
      ipp += 10;
    }
    else if (!strcmp(ipp, "-accepted") || !strcmp(ipp, "-actual") || !strcmp(ipp, "-completed") || !strcmp(ipp, "-configured") || !strcmp(ipp, "-database") || !strcmp(ipp, "-default") || !strcmp(ipp, "-detected") || !strcmp(ipp, "-ready") || !strcmp(ipp, "-supplied") || (!strcmp(ipp, "-supported") && !collection))
    {
     /*
      * Type name doesn't include attribute suffix...
      */

      break;
    }
    else if (*ipp == '-' && ipp[1])
    {
      if (smptr > sm && isdigit(smptr[-1]) && isdigit(ipp[1]))
        *smptr++ = '_';

      ipp ++;
      *smptr++ = toupper(*ipp++);
    }
    else
      *smptr++ = *ipp++;
  }

  if (collection)
    strncpy(smptr, "Type", smend - smptr);
  else
    strncpy(smptr, "WKV", smend - smptr);

  *smend = '\0';

  return (sm);
}


/*
 * 'load_attributes()' - Load the attributes description file.
 */

static int				/* O - 1 on failure, 0 on success */
load_attributes(const char *filename)	/* I - File to load */
{
  FILE		*fp;			/* File pointer */
  int		linenum = 0;		/* Line number */
  char		line[1024],		/* Line */
		*ptr,			/* Pointer into line */
		*name = NULL,		/* Current attribute name */
		*value = NULL;		/* Current attribute value */
  ipp_map_t	*temp;			/* Temporary map pointer */


  if ((fp = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "regtosm: Unable to open attribute list file \"%s\": %s\n", filename, strerror(errno));
    return (1);
  }

  while (fgets(line, sizeof(line), fp))
  {
    linenum ++;

    if ((ptr = line + strlen(line) - 1) >= line && *ptr == '\n')
      *ptr = '\0';

    if (line[0] == '#' || line[0] == '\0')
      continue;
    else if (line[0] == '=')
    {
     /*
      * Value...
      */

      for (value = line + 1; isspace(*value & 255); value ++);
    }
    else if (!isalpha(line[0] & 255))
    {
      fprintf(stderr, "regtosm: Bad attribute on line %d of \"%s\".\n", linenum, filename);
      fclose(fp);
      return (1);
    }
    else if ((name = strdup(line)) == NULL)
    {
      fputs("regtosm: Unable to allocate memory for attribute list.\n", stderr);
      fclose(fp);
      return (1);
    }
    else
      value = NULL;

    if (num_include >= alloc_include)
    {
      alloc_include += 100;
      if (!include_attributes)
        temp = malloc(alloc_include * sizeof(ipp_map_t));
      else
        temp = realloc(include_attributes, alloc_include * sizeof(ipp_map_t));

      if (!temp)
      {
        fputs("regtosm: Unable to allocate memory for attribute list.\n", stderr);
        fclose(fp);
        return (1);
      }

      include_attributes = temp;
    }

    temp = include_attributes + num_include;
    num_include ++;

    temp->name = name;
    if (value)
      temp->value = strdup(value);
    else
      temp->value = "";
  }

  fclose(fp);

  if (num_include > 0)
    qsort(include_attributes, num_include, sizeof(ipp_map_t), (int (*)(const void *, const void *))compare_map_both);

  return (0);
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
        if (!strcmp(mxmlGetElement(node), "xs:documentation"))
          return (NULL);
    case MXML_WS_AFTER_CLOSE :
        return ("\n");

    case MXML_WS_BEFORE_CLOSE :
        if (!strcmp(mxmlGetElement(node), "xs:documentation"))
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
  puts("\nUsage: ./regtosm [options] filename.xml output-directory\n");
  puts("Options:");
  puts("  -a attribute-list");
  puts("  -n namespace-url");
  puts("  -s service-name");
  puts("  -v version");

  return (1);
}
