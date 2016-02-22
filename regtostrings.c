/*
 * Program to convert keyword and enum registrations to strings.
 *
 * Usage:
 *
 *    ./reg2strings filename.xml
 *
 * Copyright (c) 2008-2016 by Michael R Sweet
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


/* <registry id="foo"> values */
#define REG_OBJECTS		"ipp-registrations-1"
#define REG_ATTRIBUTES		"ipp-registrations-2"
#define REG_KEYWORDS		"ipp-registrations-4"
#define REG_ENUMS		"ipp-registrations-6"
#define REG_OPERATIONS		"ipp-registrations-10"
#define REG_STATUS_CODES	"ipp-registrations-11"


/*
 * "Canned" localizations for media sizes and types...
 */

typedef struct
{
  const char	*id,			/* Keyword.value */
		*str;			/* Localized string */
} ipp_loc_t;

static ipp_loc_t	strings[] =
{
  { "document-state-reasons", "Detailed Document State" },
  { "font-name-requested", "Font Name" },
  { "font-size-requested", "Font Size" },
  { "input-color-mode.bi-level", "Bi-Level" },
  { "ipp-attribute-fidelity", "Attribute Fidelity" },
  { "job-hold-until", "Hold Until" },
  { "job-hold-until.indefinite", "Released" },
  { "job-sheets", "Banner Page" },
  { "job-sheets-col", "Banner Page" },
  { "job-sheets.first-print-stream-page", "First Print Stream Page" },
  { "job-sheets.job-both-sheet", "Start and End Sheets" },
  { "job-sheets.job-end-sheet", "End Sheet" },
  { "job-sheets.job-start-sheet", "Start Sheet" },
  { "job-state-reasons", "Detailed Job State" },
  { "media-grain.x-direction", "Cross-Feed Direction" },
  { "media-grain.y-direction", "Feed Direction" },
  { "media-pre-printed.blank", "Blank" },
  { "media-pre-printed.letter-head", "Letterhead" },
  { "media-pre-printed.pre-printed", "Preprinted" },
  { "media-pre-printed", "Media Preprinted" },
  { "media-type.cd", "CD" },
  { "media-type.dvd", "DVD" },
  { "media-type.photographic-film", "Photographic (Film)" },
  { "media-type.photographic-glossy", "Photographic (Glossy)" },
  { "media-type.photographic-high-gloss", "Photographic (High Gloss)" },
  { "media-type.photographic-matte", "Photographic (Matte)" },
  { "media-type.photographic-satin", "Photographic (Satin)" },
  { "media-type.photographic-semi-gloss", "Photographic (Semi-Gloss)" },
  { "media-type.stationery-coated", "Stationery Coated" },
  { "media-type.stationery-fine", "Stationery Fine" },
  { "media-type.stationery-heavyweight", "Stationery Heavyweight" },
  { "media-type.stationery-inkjet", "Stationery Inkjet" },
  { "media-type.stationery-letterhead", "Stationery Letterhead" },
  { "media-type.stationery-lightweight", "Stationery Lightweight" },
  { "media-type.stationery-preprinted", "Stationery Preprinted" },
  { "media-type.stationery-prepunched", "Stationery Prepunched" },
  { "media.choice_iso_a4_210x297mm_na_letter_8.5x11in", "A4 or US Letter" },
  { "media.iso_b5_176x250mm", "B5 Envelope" },
  { "media.iso_b6c4_125x324mm", "B6/C4 Envelope" },
  { "media.iso_dl_110x220mm", "DL Envelope" },
  { "media.jis_b0_1030x1456mm", "JIS B0" },
  { "media.jis_b1_728x1030mm", "JIS B1" },
  { "media.jis_b10_32x45mm", "JIS B10" },
  { "media.jis_b2_515x728mm", "JIS B2" },
  { "media.jis_b3_364x515mm", "JIS B3" },
  { "media.jis_b4_257x364mm", "JIS B4" },
  { "media.jis_b5_182x257mm", "JIS B5" },
  { "media.jis_b6_128x182mm", "JIS B6" },
  { "media.jis_b7_91x128mm", "JIS B7" },
  { "media.jis_b8_64x91mm", "JIS B8" },
  { "media.jis_b9_45x64mm", "JIS B9" },
  { "media.jis_exec_216x330mm", "JIS Executive" },
  { "media.jpn_chou2_111.1x146mm", "Chou 2 Envelope" },
  { "media.jpn_chou3_120x235mm", "Chou 3 Envelope" },
  { "media.jpn_chou4_90x205mm", "Chou 4 Envelope" },
  { "media.jpn_hagaki_100x148mm", "Hagaki" },
  { "media.jpn_kahu_240x322.1mm", "Kahu Envelope" },
  { "media.jpn_kaku2_240x332mm", "Kahu 2 Envelope" },
  { "media.jpn_oufuku_148x200mm", "Oufuku Reply Postcard" },
  { "media.jpn_you4_105x235mm", "You 4 Envelope" },
  { "media.na_a2_4.375x5.75in", "A2 Envelope" },
  { "media.na_c5_6.5x9.5in", "C5 Envelope" },
  { "media.na_executive_7.25x10.5in", "Executive" },
  { "media.na_fanfold-eur_8.5x12in", "European Fanfold" },
  { "media.na_fanfold-us_11x14.875in", "US Fanfold" },
  { "media.na_foolscap_8.5x13in", "Foolscap" },
  { "media.na_invoice_5.5x8.5in", "Statement" },
  { "media.na_legal_8.5x14in", "US Legal" },
  { "media.na_legal-extra_9.5x15in", "US Legal (Extra)" },
  { "media.na_letter_8.5x11in", "US Letter" },
  { "media.na_letter-extra_9.5x12in", "US Letter (Extra)" },
  { "media.na_letter-plus_8.5x12.69in", "US Letter (Plus)" },
  { "media.na_monarch_3.875x7.5in", "Monarch Envelope" },
  { "media.na_number-10_4.125x9.5in", "#10 Envelope" },
  { "media.na_number-11_4.5x10.375in", "#11 Envelope" },
  { "media.na_number-12_4.75x11in", "#12 Envelope" },
  { "media.na_number-14_5x11.5in", "#14 Envelope" },
  { "media.na_number-9_3.875x8.875in", "#9 Envelope" },
  { "media.na_personal_3.625x6.5in", "Personal Envelope" },
  { "media.na_quarto_8.5x10.83in", "Quarto" },
  { "media.om_folio_210x330mm", "Folio (Special)" },
  { "media.om_folio-sp_215x315mm", "Folio" },
  { "media.om_invite_220x220mm", "Invitation Envelope" },
  { "media.om_italian_110x230mm", "Italian Envelope" },
  { "media.om_postfix_114x229mm", "Postfix Envelope" },
  { "media.prc_1_102x165mm", "Chinese #1 Envelope" },
  { "media.prc_10_324x458mm", "Chinese #10 Envelope" },
  { "media.prc_16k_146x215mm", "Chinese 16k" },
  { "media.prc_2_102x176mm", "Chinese #2 Envelope" },
  { "media.prc_3_125x176mm", "Chinese #3 Envelope" },
  { "media.prc_32k_97x151mm", "Chinese 32k" },
  { "media.prc_4_110x208mm", "Chinese #4 Envelope" },
  { "media.prc_5_110x220mm", "Chinese #5 Envelope" },
  { "media.prc_6_120x320mm", "Chinese #6 Envelope" },
  { "media.prc_7_160x230mm", "Chinese #7 Envelope" },
  { "media.prc_8_120x309mm", "Chinese #8 Envelope" },
  { "media.roc_16k_7.75x10.75in", "ROC 16k" },
  { "media.roc_8k_10.75x15.5in", "ROC 8k" },
  { "notify-recipient-uri", "Notify Recipient" },
  { "notify-subscribed-event", "Notify Event" },
  { "number-up", "Number-Up" },
  { "orientation-requested", "Orientation" },
  { "pdf-versions-supported.iso-15930-1_2001", "ISO 15930-1:2001" },
  { "pdf-versions-supported.iso-15930-3_2002", "ISO 15930-3:2002" },
  { "pdf-versions-supported.iso-15930-4_2003", "ISO 15930-4:2003" },
  { "pdf-versions-supported.iso-15930-6_2003", "ISO 15930-6:2003" },
  { "pdf-versions-supported.iso-15930-7_2010", "ISO 15930-7:2010" },
  { "pdf-versions-supported.iso-15930-8_2010", "ISO 15930-8:2010" },
  { "pdf-versions-supported.iso-16612-2_2010", "ISO 16612-2:2010" },
  { "pdf-versions-supported.iso-19005-1_2005", "ISO 19005-1:2005" },
  { "pdf-versions-supported.iso-19005-2_2011", "ISO 19005-2:2011" },
  { "pdf-versions-supported.iso-19005-3_2012", "ISO 19005-3:2012" },
  { "pdf-versions-supported.iso-32000-1_2008", "ISO 32000-1:2008" },
  { "pdf-versions-supported.pwg-5102.3", "PWG 5102.3" },
  { "presentation-direction-number-up", "Number-Up Layout" },
  { "presentation-direction-number-up.tobottom-toleft", "Top-Bottom, Right-Left" },
  { "presentation-direction-number-up.tobottom-toright", "Top-Bottom, Left-Right" },
  { "presentation-direction-number-up.toleft-tobottom", "Right-Left, Top-Bottom" },
  { "presentation-direction-number-up.toleft-totop", "Right-Left, Bottom-Top" },
  { "presentation-direction-number-up.toright-tobottom", "Left-Right, Top-Bottom" },
  { "presentation-direction-number-up.toright-totop", "Left-Right, Bottom-Top" },
  { "presentation-direction-number-up.totop-toleft", "Bottom-Top, Right-Left" },
  { "presentation-direction-number-up.totop-toright", "Bottom-Top, Left-Right" },
  { "print-color-mode.bi-level", "Bi-Level" },
  { "print-color-mode.process-bi-level", "Process Bi-Level" },
  { "print-content-optimize", "Print Optimization" },
  { "print-content-optimize.graphic", "Graphics" },
  { "print-content-optimize.text-and-graphic", "Text And Graphics" },
  { "print-rendering-intent.relative-bpc", "Relative w/Black Point Compensation" },
  { "printer-state-reasons", "Detailed Printer State" },
  { "printer-up-time", "Printer Uptime" },
  { "profile-uri-actual", "Actual Profile URI" },
  { "sheet-collate", "Collate Copies" },
  { "sheet-collate.collated", "Yes" },
  { "sheet-collate.uncollated", "No" },
  { "sides", "2-Sided Printing" },
  { "sides.one-sided", "Off" },
  { "sides.two-sided-long-edge", "On (Portrait)" },
  { "sides.two-sided-short-edge", "On (Landscape)" },
  { "x-side1-image-shift", "X Front Side Image Shift" },
  { "x-side2-image-shift", "X Back Side Image Shift" },
  { "y-side1-image-shift", "Y Front Side Image Shift" },
  { "y-side2-image-shift", "Y Back Side Image Shift" }
};


/*
 * Local functions...
 */

static int		compare_loc(ipp_loc_t *a, ipp_loc_t *b);
static const char	*get_localized(const char *attribute,
			               const char *name, const char *value,
			               char *buffer, size_t bufsize);
static int		load_cb(mxml_node_t *node);
static int		usage(void);
static void		write_strings(mxml_node_t *registry_node);


/*
 * 'main()' - Handle additions to the IANA registry.
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
                                       REG_ATTRIBUTES, MXML_DESCEND)) != NULL)
    write_strings(registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       REG_ENUMS, MXML_DESCEND)) != NULL)
    write_strings(registry_node);

  if ((registry_node = mxmlFindElement(xml, xml, "registry", "id",
                                       REG_KEYWORDS, MXML_DESCEND)) != NULL)
    write_strings(registry_node);

  return (0);
}


/*
 * 'compare_loc()' - Compare two localizations.
 */

static int				/* O - Result of comparison */
compare_loc(ipp_loc_t *a,		/* I - First localization */
            ipp_loc_t *b)		/* I - Second localization */
{
  return (strcmp(a->id, b->id));
}


/*
 * 'get_localized()' - Get the localized string for a value.
 *
 * Converts keywords of the form "some-name" to "Some Name".
 */

static const char *			/* O - Localized string */
get_localized(const char *attribute,	/* I - Attribute */
              const char *name,		/* I - Name string */
              const char *value,	/* I - Value */
              char       *buffer,	/* I - Buffer */
              size_t     bufsize)	/* I - Size of buffer */
{
  char		*bufptr,		/* Pointer into buffer */
		*bufend,		/* End of buffer */
		id[256];		/* Localization identifier */
  ipp_loc_t	key,			/* Search key */
		*match;			/* Matching localization */
  const char	*size;			/* Size string */


 /*
  * See if we have a canned localization...
  */

  if (*attribute)
    snprintf(id, sizeof(id), "%s.%s", attribute, value);
  else
    strlcpy(id, value, sizeof(id));

  key.id = id;
  if ((match = (ipp_loc_t *)bsearch(&key, strings,
                                    sizeof(strings) / sizeof(strings[0]),
                                    sizeof(ipp_loc_t),
                                    (int (*)(const void *,
                                             const void *))compare_loc)) != NULL)
    return (match->str);

 /*
  * Not a canned localization, is this a media size name?
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  if (!strcmp(attribute, "media") && (size = strrchr(value, '_')) != NULL)
  {
   /*
    * Yes, use the ISO/US name or the size from the end of the self-describing
    * name...
    */

    if (!strncmp(value, "iso_", 4))
    {
     /*
      * Use ISO name...
      */

      value += 4;

      *bufptr++ = toupper(*value++);

      while (*value && *value != '_' && bufptr < bufend)
      {
        if (!strncmp(value, "-extra", 6))
        {
          value += 6;
          strlcpy(bufptr, " (Extra)", bufend - bufptr + 1);
          bufptr += strlen(bufptr);
        }
        else if (!strncmp(value, "-tab", 4))
        {
          value += 4;
          strlcpy(bufptr, " (Tab)", bufend - bufptr + 1);
          bufptr += strlen(bufptr);
        }
        else
	  *bufptr++ = *value++;
      }

      *bufptr = '\0';

      if (buffer[0] == 'C')
        strlcpy(bufptr, " Envelope", bufend - bufptr + 1);
    }
    else
    {
     /*
      * Use dimensional name...
      */

      size ++;
      while (*size && bufptr < bufend)
      {
        if (*size == 'x')
        {
          size ++;
          strlcpy(bufptr, " x ", bufend - bufptr + 1);
          bufptr += strlen(bufptr);
        }
        else if (!strcmp(size, "in"))
        {
          size += 2;
	  strlcpy(bufptr, "\\\"", bufend - bufptr + 1);
	}
	else
	  *bufptr++ = *size++;
      }
    }

    return (buffer);
  }

 /*
  * Common words?
  */

  if (!strcmp(name, "auto"))
    return ("Automatic");
  else if (!strcmp(name, "semi-gloss"))
    return ("Semi-Gloss");
  else if (!strcmp(name, "multi-color"))
    return ("Multi-Color");

 /*
  * Nope, make one from the keyword/name...
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  while (*name && bufptr < bufend)
  {
    if (!strncmp(name, "jdf-", 4))
    {
      name += 4;
      strlcpy(bufptr, "JDF ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "rgb_", 4))
    {
      name += 4;
      strlcpy(bufptr, "RGB ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "rgba_", 5))
    {
      name += 5;
      strlcpy(bufptr, "RGBA ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "cmyk_", 5))
    {
      name += 5;
      strlcpy(bufptr, "CMYK ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }

    *bufptr++ = toupper(*name++ & 255);

    while (*name && bufptr < bufend)
    {
      if (!strcmp(name, "-col"))
      {
        name += 4;
        break;
      }
      else if (!strcmp(name, "-id"))
      {
	name += 3;
	strlcpy(bufptr, " ID", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
	break;
      }
      else if (!strcmp(name, "-requested"))
      {
        name += 10;
        break;
      }
      else if (!strcmp(name, "-uri"))
      {
	name += 4;
	strlcpy(bufptr, " URI", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
	break;
      }
      else if (*name == '-' || *name == '_')
      {
        *bufptr++ = ' ';
        name ++;
        break;
      }

      *bufptr++ = *name++;
    }
  }

  *bufptr = '\0';

  return (buffer);
}


/*
 * 'load_cb()' - Load callback.
 */

static int				/* O - Type of child node */
load_cb(mxml_node_t *node)		/* I - Parent node */
{
  const char *name = mxmlGetElement(node);
					/* Name of parent element */


  if (!strcmp(name, "attribute") ||
      !strcmp(name, "collection") ||
      !strcmp(name, "created") ||
      !strcmp(name, "member_attribute") ||
      !strcmp(name, "name") ||
      !strcmp(name, "note") ||
      !strcmp(name, "registration_rule") ||
      !strcmp(name, "section") ||
      !strcmp(name, "sub-member_attribute") ||
      !strcmp(name, "syntax") ||
      !strcmp(name, "title") ||
      !strcmp(name, "type") ||
      !strcmp(name, "updated") ||
      !strcmp(name, "value") ||
      !strcmp(name, "xref"))
    return (MXML_OPAQUE);
  else
    return (MXML_IGNORE);
}


/*
 * 'usage()' - Show program usage.
 */

static int				/* O - Exit status */
usage(void)
{
  puts("\nUsage: ./regtostrings filename.xml >filename.strings\n");
  return (1);
}


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
