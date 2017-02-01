/*
 * Standard localizations of IPP keywords and enums.
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

#ifndef __IPP_STRINGS_H__
#  define __IPP_STRINGS_H__


/*
 * "Canned" localizations for media sizes and types...
 */

typedef struct
{
  const char	*id,			/* Keyword.value */
		*str;			/* Localized string */
} ipp_loc_t;

static int		ipp_strings_sorted = 0;
static ipp_loc_t	ipp_strings[] =
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
  { "media-type.disc", "Optical Disc" },
  { "media-type.disc-glossy", "Glossy Optical Disc" },
  { "media-type.disc-high-gloss", "High Gloss Optical Disc" },
  { "media-type.disc-matte", "Matte Optical Disc" },
  { "media-type.disc-satin", "Satin Optical Disc" },
  { "media-type.disc-semi-gloss", "Semi-Gloss Optical Disc" },
  { "media-type.dvd", "DVD" },
  { "media-type.envelope-archival", "Archival Envelope" },
  { "media-type.envelope-bond", "Bond Envelope" },
  { "media-type.envelope-coated", "Coated Envelope" },
  { "media-type.envelope-colored", "Colored Envelope" },
  { "media-type.envelope-cotton", "Cotton Envelope" },
  { "media-type.envelope-fine", "Fine Envelope" },
  { "media-type.envelope-heavyweight", "Heavyweight Envelope" },
  { "media-type.envelope-inkjet", "Inkjet Envelope" },
  { "media-type.envelope-lightweight", "Lightweight Envelope" },
  { "media-type.envelope-plain", "Plain Envelope" },
  { "media-type.envelope-preprinted", "Preprinted Envelope" },
  { "media-type.envelope-window", "Windowed Envelope" },
  { "media-type.fabric", "Fabric" },
  { "media-type.fabric-archival", "Archival Fabric" },
  { "media-type.fabric-glossy", "Glossy Fabric" },
  { "media-type.fabric-high-gloss", "High Gloss Fabric" },
  { "media-type.fabric-matte", "Matte Fabric" },
  { "media-type.fabric-semi-gloss", "Semi-Gloss Fabric" },
  { "media-type.fabric-waterproof", "Waterproof Fabric" },
  { "media-type.labels-colored", "Colored Labels" },
  { "media-type.labels-glossy", "Glossy Labels" },
  { "media-type.labels-high-gloss", "High Gloss Labels" },
  { "media-type.labels-inkjet", "Inkjet Labels" },
  { "media-type.labels-matte", "Matte Labels" },
  { "media-type.labels-permanent", "Permanent Labels" },
  { "media-type.labels-satin", "Satin Labels" },
  { "media-type.labels-security", "Security Labels" },
  { "media-type.labels-semi-gloss", "Semi-Gloss Labels" },
  { "media-type.photographic", "Photo Paper" },
  { "media-type.photographic-film", "Photo Film" },
  { "media-type.photographic-glossy", "Glossy Photo Paper" },
  { "media-type.photographic-high-gloss", "High Gloss Photo Paper" },
  { "media-type.photographic-matte", "Matte Photo Paper" },
  { "media-type.photographic-satin", "Satin Photo Paper" },
  { "media-type.photographic-semi-gloss", "Semi-Gloss Photo Paper" },
  { "media-type.stationery-coated", "Coated Paper" },
  { "media-type.stationery-fine", "Vellum Paper" },
  { "media-type.stationery-heavyweight", "Heavyweight Paper" },
  { "media-type.stationery-inkjet", "Stationery Inkjet Paper" },
  { "media-type.stationery-letterhead", "Letterhead" },
  { "media-type.stationery-lightweight", "Lightweight Paper" },
  { "media-type.stationery-preprinted", "Preprinted Paper" },
  { "media-type.stationery-prepunched", "Punched Paper" },
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
 * 'ipp_compare_loc()' - Compare two localizations.
 */

static int				/* O - Result of comparison */
ipp_compare_loc(ipp_loc_t *a,		/* I - First localization */
                ipp_loc_t *b)		/* I - Second localization */
{
  return (strcmp(a->id, b->id));
}


/*
 * 'ipp_get_localized()' - Get the localized string for a value.
 *
 * Converts keywords of the form "some-name" to "Some Name".
 */

static const char *			/* O - Localized string */
ipp_get_localized(const char *attribute,/* I - Attribute */
		  const char *name,	/* I - Name string */
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
  * Sort the strings array the first time...
  */

  if (!ipp_strings_sorted)
  {
    ipp_strings_sorted = 1;

    qsort(ipp_strings, sizeof(ipp_strings) / sizeof(ipp_strings[0]), sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc);
  }

 /*
  * See if we have a canned localization...
  */

  if (*attribute)
    snprintf(id, sizeof(id), "%s.%s", attribute, value);
  else
    strlcpy(id, value, sizeof(id));

  key.id = id;
  if ((match = (ipp_loc_t *)bsearch(&key, ipp_strings, sizeof(ipp_strings) / sizeof(ipp_strings[0]), sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc)) != NULL)
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
#endif /* !__IPP_STRINGS_H__ */
