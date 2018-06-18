/*
 * Standard localizations of IPP keywords and enums.
 *
 * Copyright © 2018 by The IEEE-ISTO Printer Working Group.
 * Copyright © 2008-2017 by Michael R Sweet
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
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
  { "accuracy-units.mm", "Millimeters" },
  { "accuracy-units.nm", "Nanometers" },
  { "accuracy-units.um", "Micrometers" },
  { "baling", "Bale Output" },
  { "baling-type", "Bale Using" },
  { "baling-when", "Bale After" },
  { "binding", "Bind Output" },
  { "binding-reference-edge", "Bind Edge" },
  { "binding-type", "Binder Type" },
  { "charge-info-message", "Print Job Cost" },
  { "coating", "Coat Sheets" },
  { "coating-sides", "Add Coating To" },
  { "coating-sides.both", "Front and Back" },
  { "coating-type", "Type of Coating" },
  { "confirmation-sheet-print", "Print Confirmation Sheet" },
  { "cover-back", "Back Cover" },
  { "cover-front", "Front Cover" },
  { "cover-type", "Printed Cover" },
  { "cover-type.print-back", "Back Only" },
  { "cover-type.print-both", "Front and Back" },
  { "cover-type.print-front", "Front Only" },
  { "cover-type.print-none", "None" },
  { "covering", "Cover Output" },
  { "covering-name", "Add Cover" },
  { "document-copies", "Copies" },
  { "document-state-reasons", "Detailed Document State" },
  { "finishing-template.trim-after-copies", "Trim After Every Set" },
  { "finishing-template.trim-after-documents", "Trim After Every Document" },
  { "finishing-template.trim-after-job", "Trim After Job" },
  { "finishing-template.trim-after-pages", "Trim After Every Page" },
  { "folding", "Fold" },
  { "folding-direction", "Fold Direction" },
  { "folding-offset", "Fold Position" },
  { "folding-reference-edge", "Fold Edge" },
  { "font-name-requested", "Font Name" },
  { "font-size-requested", "Font Size" },
  { "input-attributes", "Scanner Options" },
  { "input-color-mode", "Scanning Mode" },
  { "input-color-mode.bi-level", "Text" },
  { "input-color-mode.cmyk_16", "16-bit CMYK" },
  { "input-color-mode.cmyk_8", "8-bit CMYK" },
  { "input-color-mode.color_8", "8-bit Color" },
  { "input-color-mode.monochrome_16", "16-bit Monochrome" },
  { "input-color-mode.monochrome_4", "4-bit Monochrome" },
  { "input-color-mode.monochrome_8", "8-bit Monochrome" },
  { "input-color-mode.rgb_16", "16-bit RGB" },
  { "input-color-mode.rgba_16", "16-bit RGBA" },
  { "input-color-mode.rgba_8", "8-bit RGBA" },
  { "input-content-type", "Scan Content" },
  { "input-film-scan-mode", "Film Type" },
  { "input-scan-regions", "Scan Regions" },
  { "input-sides", "2-Sided Scanning" },
  { "input-source.adf", "Automatic Document Feeder" },
  { "job-accounting-sheets-type", "Type of Job Accounting Sheets" },
  { "job-collation-type", "Collate Copies" },
  { "job-copies", "Copies" },
  { "job-cover-back", "Back Cover" },
  { "job-cover-front", "Front Cover" },
  { "job-delay-output-until", "Delay Output Until" },
  { "job-delay-output-until.day-time", "Daytime" },
  { "job-delay-output-until.indefinite", "Released" },
  { "job-delay-output-until.no-delay-output", "No Delay" },
  { "job-delay-output-until-time", "Delay Output Until" },
  { "job-error-action", "On Error" },
  { "job-error-sheet", "Print Error Sheet" },
  { "job-error-sheet-type", "Type of Error Sheet" },
  { "job-error-sheet-when", "Print Error Sheet" },
  { "job-hold-until", "Hold Until" },
  { "job-hold-until.day-time", "Daytime" },
  { "job-hold-until.indefinite", "Released" },
  { "job-hold-until-time", "Hold Until" },
  { "job-name", "Title" },
  { "job-sheets", "Banner Page" },
  { "job-sheets-col", "Banner Page" },
  { "job-sheets.first-print-stream-page", "First Page in Document" },
  { "job-sheets.job-both-sheet", "Start and End Sheets" },
  { "job-sheets.job-end-sheet", "End Sheet" },
  { "job-sheets.job-start-sheet", "Start Sheet" },
  { "job-state-reasons", "Detailed Job State" },
  { "laminating", "Laminate Pages" },
  { "laminating-sides", "Laminate" },
  { "laminating-sides.back", "Back Only" },
  { "laminating-sides.both", "Front and Back" },
  { "laminating-sides.front", "Front Only" },
  { "laminating-type", "Type of Lamination" },
  { "material-amount", "Amount of Material" },
  { "material-amount-units", "Amount Units" },
  { "material-amount-units.g", "Grams" },
  { "material-amount-units.kg", "Kilograms" },
  { "material-amount-units.l", "Liters" },
  { "material-amount-units.m", "Meters" },
  { "material-amount-units.ml", "Milliliters" },
  { "material-amount-units.mm", "Millimeters" },
  { "material-purpose", "Use Material For" },
  { "material-purpose.all", "Everything" },
  { "material-purpose.base", "Base" },
  { "material-purpose.in-fill", "In-fill" },
  { "material-purpose.shell", "Shell" },
  { "material-purpose.support", "Supports" },
  { "material-rate", "Feed Rate" },
  { "material-rate-units", "Feed Rate Units" },
  { "material-rate-units.mg_second", "Milligrams per second" },
  { "material-rate-units.ml_second", "Milliliters per second" },
  { "material-rate-units.mm_second", "Millimeters per second" },
  { "material-type.abs", "ABS" },
  { "material-type.abs-carbon-fiber", "Carbon Fiber ABS" },
  { "material-type.abs-carbon-nanotube", "Carbon Nanotube ABS" },
  { "material-type.pla", "PLA" },
  { "material-type.pla-conductive", "Conductive PLA" },
  { "material-type.pla-flexible", "Flexible PLA" },
  { "material-type.pla-magnetic", "Magnetic PLA" },
  { "material-type.pla-steel", "Steel PLA" },
  { "material-type.pla-stone", "Stone PLA" },
  { "material-type.pla-wood", "Wood PLA" },
  { "material-type.pva-dissolvable", "Dissolvable PVA" },
  { "media-back-coating", "Back Coating of Media" },
  { "media-front-coating", "Front Coating of Media" },
  { "media-grain.x-direction", "Cross-Feed Direction" },
  { "media-grain.y-direction", "Feed Direction" },
  { "media-input-tray-check", "Force Media" },
  { "media-pre-printed.blank", "Blank" },
  { "media-pre-printed.letter-head", "Letterhead" },
  { "media-pre-printed", "Pre-printed Media" },
  { "media-recycled", "Recycled Media" },
  { "media-size", "Media Dimensions" },
  { "media-size-name", "Media Name" },
  { "media-source.by-pass-tray", "By-pass Tray" },
  { "media-tooth", "Media Tooth (Texture)" },
  { "media-tooth.calendared", "Extra Smooth" },
  { "media-tooth.uncalendared", "Rough" },
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
  { "media-weight-metric", "Media Weight (grams per m²)" },
  { "media.choice_iso_a4_210x297mm_na_letter_8.5x11in", "A4 or US Letter" },
  { "media.iso_b5_176x250mm", "Envelope B5" },
  { "media.iso_b6c4_125x324mm", "Envelope B6/C4" },
  { "media.iso_dl_110x220mm", "Envelope DL" },
  { "media.iso_ra0_860x1220mm", "ISO RA0" },
  { "media.iso_ra1_610x860mm", "ISO RA1" },
  { "media.iso_ra2_430x610mm", "ISO RA2" },
  { "media.iso_ra3_305x430mm", "ISO RA3" },
  { "media.iso_ra4_215x305mm", "ISO RA4" },
  { "media.iso_sra0_900x1280mm", "ISO SRA0" },
  { "media.iso_sra1_640x900mm", "ISO SRA1" },
  { "media.iso_sra2_450x640mm", "ISO SRA2" },
  { "media.iso_sra3_320x450mm", "ISO SRA3" },
  { "media.iso_sra4_225x320mm", "ISO SRA4" },
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
  { "media.jpn_chou2_111.1x146mm", "Envelope Chou 2" },
  { "media.jpn_chou3_120x235mm", "Envelope Chou 3" },
  { "media.jpn_chou4_90x205mm", "Envelope Chou 4" },
  { "media.jpn_hagaki_100x148mm", "Hagaki" },
  { "media.jpn_kahu_240x322.1mm", "Envelope Kahu" },
  { "media.jpn_kaku2_240x332mm", "Envelope Kahu 2" },
  { "media.jpn_oufuku_148x200mm", "Oufuku Reply Postcard" },
  { "media.jpn_you4_105x235mm", "Envelope You 4" },
  { "media.na_a2_4.375x5.75in", "Envelope A2" },
  { "media.na_c5_6.5x9.5in", "Envelope C5" },
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
  { "media.na_monarch_3.875x7.5in", "Envelope Monarch" },
  { "media.na_number-10_4.125x9.5in", "Envelope #10" },
  { "media.na_number-11_4.5x10.375in", "Envelope #11" },
  { "media.na_number-12_4.75x11in", "Envelope #12" },
  { "media.na_number-14_5x11.5in", "Envelope #14" },
  { "media.na_number-9_3.875x8.875in", "Envelope #9" },
  { "media.na_personal_3.625x6.5in", "Envelope Personal" },
  { "media.na_quarto_8.5x10.83in", "Quarto" },
  { "media.om_folio_210x330mm", "Folio (Special)" },
  { "media.om_folio-sp_215x315mm", "Folio" },
  { "media.om_invite_220x220mm", "Envelope Invitation" },
  { "media.om_italian_110x230mm", "Envelope Italian" },
  { "media.om_postfix_114x229mm", "Envelope Postfix" },
  { "media.prc_1_102x165mm", "Envelope Chinese #1" },
  { "media.prc_10_324x458mm", "Envelope Chinese #10" },
  { "media.prc_16k_146x215mm", "Chinese 16k" },
  { "media.prc_2_102x176mm", "Envelope Chinese #2" },
  { "media.prc_3_125x176mm", "Envelope Chinese #3" },
  { "media.prc_32k_97x151mm", "Chinese 32k" },
  { "media.prc_4_110x208mm", "Envelope Chinese #4" },
  { "media.prc_5_110x220mm", "Envelope Chinese #5" },
  { "media.prc_6_120x320mm", "Envelope Chinese #6" },
  { "media.prc_7_160x230mm", "Envelope Chinese #7" },
  { "media.prc_8_120x309mm", "Envelope Chinese #8" },
  { "media.roc_16k_7.75x10.75in", "ROC 16k" },
  { "media.roc_8k_10.75x15.5in", "ROC 8k" },
  { "multiple-operation-time-out-action", "On Timeout" },
  { "notify-recipient-uri", "Notify Recipient" },
  { "notify-subscribed-event", "Notify Event" },
  { "number-up", "Number-Up" },
  { "orientation-requested", "Orientation" },
  { "output-attributes", "Scanned Image Options" },
  { "output-bin", "Output Tray" },
  { "output-compression-quality-factor", "Scanned Image Quality" },
  { "page-order-received.1-to-n-order", "1 To N" },
  { "page-order-received.n-to-1-order", "N To 1" },
  { "platform-shape.ellipse", "Round" },
  { "post-dial-string", "Post-dial String" },
  { "pre-dial-string", "Pre-dial String" },
  { "presentation-direction-number-up", "Number-Up Layout" },
  { "presentation-direction-number-up.tobottom-toleft", "Top-bottom, Right-left" },
  { "presentation-direction-number-up.tobottom-toright", "Top-bottom, Left-right" },
  { "presentation-direction-number-up.toleft-tobottom", "Right-left, Top-bottom" },
  { "presentation-direction-number-up.toleft-totop", "Right-left, Bottom-top" },
  { "presentation-direction-number-up.toright-tobottom", "Left-right, Top-bottom" },
  { "presentation-direction-number-up.toright-totop", "Left-right, Bottom-top" },
  { "presentation-direction-number-up.totop-toleft", "Bottom-top, Right-left" },
  { "presentation-direction-number-up.totop-toright", "Bottom-top, Left-right" },
  { "print-color-mode.bi-level", "Text" },
  { "print-color-mode.process-bi-level", "Process Text" },
  { "print-content-optimize", "Print Optimization" },
  { "print-content-optimize.graphic", "Graphics" },
  { "print-content-optimize.text-and-graphic", "Text and Graphics" },
  { "print-rendering-intent.relative-bpc", "Relative w/Black Point Compensation" },
  { "print-scaling.auto-fit", "Auto-fit" },
  { "print-supports.material", "With Specified Material" },
  { "printer-message-from-operator", "Message From Operator" },
  { "printer-resolution", "Print Resolution" },
  { "printer-state-reasons", "Detailed Printer State" },
  { "printer-state-reasons.alert-removal-of-binary-change-entry", "Old Alerts Have Been Removed" },
  { "printer-state-reasons.interpreter-cartridge-deleted", "Interpreter Cartridge Removed" },
  { "printer-state-reasons.media-path-cannot-duplex-media-selected", "Media Path Cannot Do 2-Sided Printing" },
  { "printer-state-reasons.opc-life-over", "Optical Photoconductor Life Over" },
  { "printer-state-reasons.opc-near-eol", "Optical Photoconductor Near End-of-life" },
  { "printer-state-reasons.printer-manual-reset", "Printer Reset Manually" },
  { "printer-state-reasons.printer-nms-reset", "Printer Reset Remotely" },
  { "printer-up-time", "Printer Uptime" },
  { "profile-uri-actual", "Actual Profile URI" },
  { "punching-reference-edge", "Punch Edge" },
  { "retry-time-out", "Retry Timeout" },
  { "save-disposition.print-save", "Print and Save" },
  { "separator-sheets-type", "Type of Separator Sheets" },
  { "separator-sheets-type.both-sheets", "Start and End Sheets" },
  { "sheet-collate", "Collate Copies" },
  { "sheet-collate.collated", "Yes" },
  { "sheet-collate.uncollated", "No" },
  { "sides", "2-Sided Printing" },
  { "sides.one-sided", "Off" },
  { "sides.two-sided-long-edge", "On (Portrait)" },
  { "sides.two-sided-short-edge", "On (Landscape)" },
  { "stitching", "Staple" },
  { "stitching-method", "Staple Method" },
  { "stitching-reference-edge", "Staple Edge" },
  { "stitching-when", "Staple After" },
  { "trimming", "Cut" },
  { "trimming-offset", "Cut Position" },
  { "trimming-reference-edge", "Cut Edge" },
  { "trimming-type", "Type of Cut" },
  { "trimming-when", "Cut After" },
  { "trimming-when.after-documents", "Every Document" },
  { "trimming-when.after-job", "Job" },
  { "trimming-when.after-sets", "Every Set" },
  { "trimming-when.after-sheets", "Every Page" }
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
  const char    *suffix = NULL;         /* Suffix to add */
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

      if (buffer[0] == 'C')
      {
        strlcpy(bufptr, "Envelope ", bufend - bufptr + 1);
        bufptr += strlen(bufptr);
      }

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

          /* Unicode double prime character */
          *bufptr++ = (char)0xE2;
          *bufptr++ = (char)0x80;
          *bufptr++ = (char)0xB3;
	}
	else
	  *bufptr++ = *size++;
      }

      *bufptr = '\0';
    }

    return (buffer);
  }

 /*
  * Common words?
  */

  if (!strcmp(name, "auto"))
    return ("Automatic");
  else if (!strcmp(name, "semi-gloss"))
    return ("Semi-gloss");
  else if (!strcmp(name, "multi-color"))
    return ("Multi-color");
  else if (!strcmp(name, "pending-held"))
    return ("Held");
  else if (!strcmp(name, "pre-cut"))
    return ("Pre-cut");
  else if (!strcmp(name, "pre-printed"))
    return ("Pre-printed");
  else if (!strcmp(name, "processing-stopped"))
    return ("Stopped");
  else if (!strcmp(name, "cmyk"))
    return ("CMYK");
  else if (!strcmp(name, "icc"))
    return ("ICC Profiles");

 /*
  * Nope, make one from the keyword/name...
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  while (*name && bufptr < bufend)
  {
    if (!strncmp(name, "fold-", 5))
    {
      name += 5;
      suffix = " Fold";

      if (!*name)
        break;
    }
    else if (!strncmp(name, "input-", 6) && strcmp(attribute, "printer-state-reasons"))
    {
      name += 6;
      strlcpy(bufptr, "Scan ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "jdf-", 4))
    {
      name += 4;
      strlcpy(bufptr, "JDF ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      while (*name && bufptr < bufend)
      {
        *bufptr++ = toupper(*name & 255);
        name ++;
      }
      break;
    }
    else if (!strncmp(name, "after-", 6))
    {
      name += 6;

      if (!*name)
        break;
    }
    else if (!strncmp(name, "make-envelope-", 14))
    {
      name += 14;

      strlcpy(bufptr, "Envelope Maker ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-dual-", 11))
    {
      name += 11;

      strlcpy(bufptr, "2-hole Punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-triple-", 13))
    {
      name += 13;

      strlcpy(bufptr, "3-hole Punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-quad-", 11))
    {
      name += 11;

      strlcpy(bufptr, "4-hole Punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-multiple-", 15))
    {
      name += 15;

      strlcpy(bufptr, "Multi-hole Punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "staple-dual-", 12))
    {
      name += 12;

      strlcpy(bufptr, "2 Staples on ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "staple-triple-", 14))
    {
      name += 14;

      strlcpy(bufptr, "3 Staples on ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "reference-edge", 14))
    {
      name += 10; /* Drop "reference-" */
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
      else if (!strcmp(name, "-eol"))
      {
	name += 4;
	strlcpy(bufptr, " End of Life", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
	break;
      }
      else if (!strcmp(name, "-face-down"))
      {
	name += 10;
	strlcpy(bufptr, " Face-down", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
	break;
      }
      else if (!strcmp(name, "-face-up"))
      {
	name += 8;
	strlcpy(bufptr, " Face-up", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
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

  if (suffix)
    strlcpy(bufptr, suffix, bufend - bufptr + 1);
  else
    *bufptr = '\0';

  return (buffer);
}
#endif /* !__IPP_STRINGS_H__ */
