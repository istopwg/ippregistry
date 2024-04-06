//
// Standard localizations of IPP keywords and enums.
//
// Copyright © 2018-2024 by The IEEE-ISTO Printer Working Group.
// Copyright © 2008-2017 by Michael R Sweet
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#ifndef IPP_STRINGS_H
#  define IPP_STRINGS_H


//
// "Canned" localizations for media sizes and types...
//

typedef struct
{
  const char	*id,			// Keyword.value
		*str;			// Localized string
} ipp_loc_t;

static bool		ipp_strings_sorted = false;
static ipp_loc_t	ipp_strings[] =
{
  { "accuracy-units", "Units" },
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
  { "coating-sides.both", "Front and back" },
  { "coating-type", "Type of Coating" },
  { "confirmation-sheet-print", "Print Confirmation Sheet" },
  { "cover-back", "Back Cover" },
  { "cover-front", "Front Cover" },
  { "cover-type", "Printed Cover" },
  { "cover-type.print-back", "Back only" },
  { "cover-type.print-both", "Front and back" },
  { "cover-type.print-front", "Front only" },
  { "cover-type.print-none", "None" },
  { "covering", "Cover Output" },
  { "covering-name", "Add Cover" },
  { "finishing-template.fold-half-z", "Half Z fold" },
  { "finishing-template.fold-engineering-z", "Engineering Z fold" },
  { "finishing-template.trim", "Cut" },
  { "finishing-template.trim-after-copies", "Cut after every set" },
  { "finishing-template.trim-after-documents", "Cut after every document" },
  { "finishing-template.trim-after-job", "Cut after job" },
  { "finishing-template.trim-after-pages", "Cut after every page" },
  { "finishings.101", "Engineering Z fold" },
  { "finishings.11", "Cut" },
  { "finishings.60", "Cut after every page" },
  { "finishings.61", "Cut after every document" },
  { "finishings.62", "Cut after every set" },
  { "finishings.63", "Cut after job" },
  { "finishings.94", "Half Z fold" },
  { "folding", "Fold" },
  { "folding-direction", "Fold Direction" },
  { "folding-offset", "Fold Position" },
  { "folding-reference-edge", "Fold Edge" },
  { "input-attributes", "Scanner Options" },
  { "input-color-mode", "Scanning Mode" },
  { "input-color-mode.bi-level", "Text" },
  { "input-color-mode.cmyk_16", "16-bit CMYK" },
  { "input-color-mode.cmyk_8", "8-bit CMYK" },
  { "input-color-mode.color_8", "8-bit Color" },
  { "input-color-mode.monochrome_16", "16-bit monochrome" },
  { "input-color-mode.monochrome_4", "4-bit monochrome" },
  { "input-color-mode.monochrome_8", "8-bit monochrome" },
  { "input-color-mode.rgb_16", "16-bit RGB" },
  { "input-color-mode.rgba_16", "16-bit RGBA" },
  { "input-color-mode.rgba_8", "8-bit RGBA" },
  { "input-content-type", "Scan Content" },
  { "input-film-scan-mode", "Film Type" },
  { "input-scan-regions", "Scan Regions" },
  { "input-sides", "2-Sided Scanning" },
  { "input-source.adf", "Automatic Document Feeder" },
  { "job-delay-output-until", "Delay Output Until" },
  { "job-delay-output-until.day-time", "Daytime" },
  { "job-delay-output-until.indefinite", "Released" },
  { "job-delay-output-until.no-delay-output", "No delay" },
  { "job-error-action", "On Error" },
  { "job-error-sheet", "Print Error Sheet" },
  { "job-error-sheet-type", "Type of Error Sheet" },
  { "job-error-sheet-when", "Print Error Sheet" },
  { "job-hold-until", "Hold Until" },
  { "job-hold-until.day-time", "Daytime" },
  { "job-hold-until.indefinite", "Released" },
  { "job-name", "Title" },
  { "job-retain-until", "Retain Job" },
  { "job-retain-until.indefinite", "Forever" },
  { "job-retain-until.none", "Never" },
  { "job-sheets", "Banner Pages" },
  { "job-sheets-col", "Banner Pages" },
  { "job-sheets.first-print-stream-page", "First page in document" },
  { "job-sheets.job-both-sheet", "Start and end sheets" },
  { "job-sheets.job-end-sheet", "End sheet" },
  { "job-sheets.job-start-sheet", "Start sheet" },
  { "job-state-reasons.aborted-by-system", "Stopping" },
  { "job-state-reasons.compression-error", "Decompression error" },
  { "job-state-reasons.hold-new-jobs", "Printer holding new jobs" },
  { "job-state-reasons.job-canceled-at-device", "Canceled at printer" },
  { "job-state-reasons.job-canceled-by-operator", "Canceled by operator" },
  { "job-state-reasons.job-canceled-by-user", "Canceled by user" },
  { "job-state-reasons.job-completed-successfully", " " },
  { "job-state-reasons.job-completed-with-errors", "Completed with errors" },
  { "job-state-reasons.job-completed-with-warnings", "Completed with warnings" },
  { "job-state-reasons.job-data-insufficient", "Insufficient data" },
  { "job-state-reasons.job-hold-until-specified", "Job held" },
  { "job-state-reasons.job-incoming", "Incoming" },
  { "job-state-reasons.job-interpreting", "Interpreting" },
  { "job-state-reasons.job-outgoing", "Outgoing" },
  { "job-state-reasons.job-printing", "Printing" },
  { "job-state-reasons.job-queued-for-marker", "Processing document" },
  { "job-state-reasons.job-queued", "Preparing to print" },
  { "job-state-reasons.job-restartable", "Restartable" },
  { "job-state-reasons.job-suspended", "Suspended" },
  { "job-state-reasons.job-transforming", "Transforming" },
  { "job-state-reasons.marker-supply-missing", "Ink/toner not installed" },
  { "job-state-reasons.printer-stopped-partly", "Printer partially stopped" },
  { "job-state-reasons.printer-stopped", "Printer offline" },
  { "job-state-reasons.processing-to-stop-point", "Stopping" },
  { "job-state-reasons.queued-in-device", "Ready" },
  { "job-state-reasons.service-needed", "Printer maintenance required" },
  { "job-state-reasons.service-off-line", "Service offline" },
  { "laminating", "Laminate Pages" },
  { "laminating-sides", "Laminate" },
  { "laminating-sides.back", "Back only" },
  { "laminating-sides.both", "Front and back" },
  { "laminating-sides.front", "Front only" },
  { "laminating-type", "Type of Lamination" },
  { "material-amount", "Amount of Material" },
  { "material-amount-units", "Units" },
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
  { "material-rate-units", "Units" },
  { "material-rate-units.mg_second", "Milligrams per second" },
  { "material-rate-units.ml_second", "Milliliters per second" },
  { "material-rate-units.mm_second", "Millimeters per second" },
  { "material-type.abs", "ABS" },
  { "material-type.abs-carbon-fiber", "Carbon fiber ABS" },
  { "material-type.abs-carbon-nanotube", "Carbon nanotube ABS" },
  { "material-type.pet", "PET" },
  { "material-type.pla", "PLA" },
  { "material-type.pla-conductive", "Conductive PLA" },
  { "material-type.pla-dissolvable", "Dissolvable PLA" },
  { "material-type.pla-flexible", "Flexible PLA" },
  { "material-type.pla-magnetic", "Magnetic PLA" },
  { "material-type.pla-steel", "Steel PLA" },
  { "material-type.pla-stone", "Stone PLA" },
  { "material-type.pla-wood", "Wood PLA" },
  { "material-type.pva-dissolvable", "Dissolvable PVA" },
  { "media", "Media Size" },
  { "media-back-coating", "Back Coating of Media" },
  { "media-col", "Media" },
  { "media-front-coating", "Front Coating of Media" },
  { "media-grain.x-direction", "Cross-Feed Direction" },
  { "media-grain.y-direction", "Feed Direction" },
  { "media-pre-printed.blank", "Blank" },
  { "media-pre-printed.letter-head", "Letterhead" },
  { "media-pre-printed.pre-printed", "Preprinted" },
  { "media-pre-printed", "Media Markings" },
  { "media-recycled", "Recycled Media" },
  { "media-size", "Media Dimensions" },
  { "media-size-name", "Media Name" },
  { "media-source.by-pass-tray", "Multipurpose tray" },
  { "media-tooth", "Media Tooth (Texture)" },
  { "media-tooth.calendared", "Extra smooth" },
  { "media-tooth.uncalendared", "Rough" },
  { "media-type.cd", "CD" },
  { "media-type.disc", "Optical disc" },
  { "media-type.disc-glossy", "Glossy optical disc" },
  { "media-type.disc-high-gloss", "High gloss optical disc" },
  { "media-type.disc-matte", "Matte optical disc" },
  { "media-type.disc-satin", "Satin optical disc" },
  { "media-type.disc-semi-gloss", "Semi-gloss optical disc" },
  { "media-type.dvd", "DVD" },
  { "media-type.envelope-archival", "Archival envelope" },
  { "media-type.envelope-bond", "Bond envelope" },
  { "media-type.envelope-coated", "Coated envelope" },
  { "media-type.envelope-colored", "Colored envelope" },
  { "media-type.envelope-cotton", "Cotton envelope" },
  { "media-type.envelope-fine", "Fine envelope" },
  { "media-type.envelope-heavyweight", "Heavyweight envelope" },
  { "media-type.envelope-inkjet", "Inkjet envelope" },
  { "media-type.envelope-lightweight", "Lightweight envelope" },
  { "media-type.envelope-plain", "Plain envelope" },
  { "media-type.envelope-preprinted", "Preprinted envelope" },
  { "media-type.envelope-window", "Windowed envelope" },
  { "media-type.fabric", "Fabric" },
  { "media-type.fabric-archival", "Archival fabric" },
  { "media-type.fabric-glossy", "Glossy fabric" },
  { "media-type.fabric-high-gloss", "High gloss fabric" },
  { "media-type.fabric-matte", "Matte fabric" },
  { "media-type.fabric-semi-gloss", "Semi-gloss fabric" },
  { "media-type.fabric-waterproof", "Waterproof fabric" },
  { "media-type.labels-colored", "Colored labels" },
  { "media-type.labels-glossy", "Glossy labels" },
  { "media-type.labels-high-gloss", "High gloss labels" },
  { "media-type.labels-inkjet", "Inkjet labels" },
  { "media-type.labels-matte", "Matte labels" },
  { "media-type.labels-permanent", "Permanent labels" },
  { "media-type.labels-satin", "Satin labels" },
  { "media-type.labels-security", "Security labels" },
  { "media-type.labels-semi-gloss", "Semi-gloss labels" },
  { "media-type.photographic", "Photo paper" },
  { "media-type.photographic-film", "Photo film" },
  { "media-type.photographic-glossy", "Glossy photo paper" },
  { "media-type.photographic-high-gloss", "High gloss photo paper" },
  { "media-type.photographic-matte", "Matte photo paper" },
  { "media-type.photographic-satin", "Satin photo paper" },
  { "media-type.photographic-semi-gloss", "Semi-gloss photo paper" },
  { "media-type.stationery-coated", "Coated paper" },
  { "media-type.stationery-fine", "Vellum paper" },
  { "media-type.stationery-heavyweight", "Heavyweight paper" },
  { "media-type.stationery-inkjet", "Stationery inkjet paper" },
  { "media-type.stationery-letterhead", "Letterhead" },
  { "media-type.stationery-lightweight", "Lightweight paper" },
  { "media-type.stationery-preprinted", "Preprinted paper" },
  { "media-type.stationery-prepunched", "Punched paper" },
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
  { "media.jpn_chou40_90x225mm", "Envelope Chou 40" },
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
  { "media.prc_1_102x165mm", "Envelope chinese #1" },
  { "media.prc_10_324x458mm", "Envelope chinese #10" },
  { "media.prc_16k_146x215mm", "Chinese 16k" },
  { "media.prc_2_102x176mm", "Envelope chinese #2" },
  { "media.prc_3_125x176mm", "Envelope chinese #3" },
  { "media.prc_32k_97x151mm", "Chinese 32k" },
  { "media.prc_4_110x208mm", "Envelope chinese #4" },
  { "media.prc_5_110x220mm", "Envelope chinese #5" },
  { "media.prc_6_120x320mm", "Envelope chinese #6" },
  { "media.prc_7_160x230mm", "Envelope chinese #7" },
  { "media.prc_8_120x309mm", "Envelope chinese #8" },
  { "media.roc_16k_7.75x10.75in", "ROC 16k" },
  { "media.roc_8k_10.75x15.5in", "ROC 8k" },
  { "number-up", "Number-Up" },
  { "orientation-requested", "Orientation" },
  { "output-attributes", "Scanned Image Options" },
  { "output-bin", "Output Tray" },
  { "output-compression-quality-factor", "Scanned Image Quality" },
  { "post-dial-string", "Post-Dial String" },
  { "pre-dial-string", "Pre-Dial String" },
  { "presentation-direction-number-up", "Number-Up Layout" },
  { "presentation-direction-number-up.tobottom-toleft", "Top-bottom, right-left" },
  { "presentation-direction-number-up.tobottom-toright", "Top-bottom, left-right" },
  { "presentation-direction-number-up.toleft-tobottom", "Right-left, top-bottom" },
  { "presentation-direction-number-up.toleft-totop", "Right-left, bottom-top" },
  { "presentation-direction-number-up.toright-tobottom", "Left-right, top-bottom" },
  { "presentation-direction-number-up.toright-totop", "Left-right, bottom-top" },
  { "presentation-direction-number-up.totop-toleft", "Bottom-top, right-left" },
  { "presentation-direction-number-up.totop-toright", "Bottom-top, left-right" },
  { "print-color-mode.bi-level", "Text" },
  { "print-color-mode.process-bi-level", "Process Text" },
  { "print-content-optimize", "Print Optimization" },
  { "print-content-optimize.graphic", "Graphics" },
  { "print-content-optimize.text-and-graphic", "Text and graphics" },
  { "print-rendering-intent.relative-bpc", "Relative w/black point compensation" },
  { "print-scaling.auto-fit", "Auto-fit" },
  { "print-supports.material", "With specified material" },
  { "printer-resolution", "Print Resolution" },
  { "printer-state-reasons.alert-removal-of-binary-change-entry", "Old Alerts Have Been Removed" },
  { "printer-state-reasons.bander-life-almost-over", "Replace bander soon" },
  { "printer-state-reasons.bander-life-over", "Replace bander" },
  { "printer-state-reasons.bander-missing", "Install bander" },
  { "printer-state-reasons.bander-motor-failure", "Bander motor failed" },
  { "printer-state-reasons.bander-over-temperature", "Bander temperature high" },
  { "printer-state-reasons.bander-thermistor-failure", "Bander temperature sensor failed" },
  { "printer-state-reasons.bander-timing-failure", "Bander failed" },
  { "printer-state-reasons.bander-under-temperature", "Bander temperature low" },
  { "printer-state-reasons.camera-failure", "Camera failed" },
  { "printer-state-reasons.cleaner-life-almost-over", "Replace cleaner soon" },
  { "printer-state-reasons.cleaner-life-over", "Replace cleaner" },
  { "printer-state-reasons.die-cutter-life-almost-over", "Replace die cutter soon" },
  { "printer-state-reasons.die-cutter-lift-over", "Replace die cutter" },
  { "printer-state-reasons.die-cutter-missing", "Install die cutter" },
  { "printer-state-reasons.die-cutter-motor-failure", "Die cutter motor failed" },
  { "printer-state-reasons.die-cutter-over-temperature", "Die cutter temperature high" },
  { "printer-state-reasons.die-cutter-thermistor-failure", "Die cutter temperature sensor failed" },
  { "printer-state-reasons.die-cutter-timing-failure", "Die cutter failed" },
  { "printer-state-reasons.die-cutter-under-temperature", "Die cutter temperature low" },
  { "printer-state-reasons.extruder-failed", "Extruder failed" },
  { "printer-state-reasons.extruder-jam", "Extruder jammed" },
  { "printer-state-reasons.fan-failure", "Fan failed" },
  { "printer-state-reasons.fax-modem-life-almost-over", "Replace fax modem soon" },
  { "printer-state-reasons.fax-modem-life-over", "Replace fax modem" },
  { "printer-state-reasons.fax-modem-missing", "Install fax modem" },
  { "printer-state-reasons.folder-jam", "Folder jammed" },
  { "printer-state-reasons.folder-life-almost-over", "Replace folder soon" },
  { "printer-state-reasons.folder-life-over", "Replace folder" },
  { "printer-state-reasons.folder-missing", "Install folder" },
  { "printer-state-reasons.folder-over-temperature", "Folder temperature high" },
  { "printer-state-reasons.folder-thermistor-failure", "Folder temperature sensor failed" },
  { "printer-state-reasons.folder-timing-failure", "Folder failed" },
  { "printer-state-reasons.folder-under-temperature", "Folder temperature low" },
  { "printer-state-reasons.fuser-over-temp", "Fuser temperature high" },
  { "printer-state-reasons.fuser-under-temp", "Fuser temperature low" },
  { "printer-state-reasons.input-tray-missing", "Paper tray is missing" },
  { "printer-state-reasons.marker-ink-almost-empty", "Ink low" },
  { "printer-state-reasons.marker-ink-empty", "Ink empty" },
  { "printer-state-reasons.marker-ink-missing", "Install ink" },
  { "printer-state-reasons.imprinter-jam", "Imprinter jammed" },
  { "printer-state-reasons.imprinter-life-almost-over", "Replace imprinter soon" },
  { "printer-state-reasons.imprinter-life-over", "Replace imprinter" },
  { "printer-state-reasons.imprinter-missing", "Install imprinter" },
  { "printer-state-reasons.imprinter-motor-failure", "Imprinter motor failed" },
  { "printer-state-reasons.imprinter-over-temperature", "Imprinter temperature high" },
  { "printer-state-reasons.imprinter-thermistor-failure", "Imprinter temperature sensor failed" },
  { "printer-state-reasons.imprinter-timing-failure", "Imprinter failed" },
  { "printer-state-reasons.imprinter-under-temperature", "Imprinter temperature low" },
  { "printer-state-reasons.input-cannot-feed-side-selected", "Paper size not supported" },
  { "printer-state-reasons.input-manual-input-request", "Load paper" },
  { "printer-state-reasons.input-media-color-change", "Paper changed" },
  { "printer-state-reasons.input-media-form-parts-change", "Paper changed" },
  { "printer-state-reasons.input-media-size-change", "Paper changed" },
  { "printer-state-reasons.input-media-tray-failure", "Paper tray failed" },
  { "printer-state-reasons.input-media-tray-feed-error", "Paper tray feed error" },
  { "printer-state-reasons.input-media-type-change", "Paper changed" },
  { "printer-state-reasons.input-media-weight-change", "Paper changed" },
  { "printer-state-reasons.input-pick-roller-failure", "Pick roller failed" },
  { "printer-state-reasons.input-pick-roller-life-over", "Replace pick roller" },
  { "printer-state-reasons.input-pick-roller-life-warn", "Replace pick roller soon" },
  { "printer-state-reasons.input-tray-elevation-failure", "Paper tray elevation failed" },
  { "printer-state-reasons.input-tray-missing", "Install paper tray" },
  { "printer-state-reasons.input-tray-position-failure", "Paper tray position failed" },
  { "printer-state-reasons.inserter-jam", "Inserter jammed" },
  { "printer-state-reasons.inserter-life-almost-over", "Replace inserter soon" },
  { "printer-state-reasons.inserter-life-over", "Replace inserter" },
  { "printer-state-reasons.inserter-missing", "Install inserter" },
  { "printer-state-reasons.inserter-motor-failure", "Inserter motor failed" },
  { "printer-state-reasons.inserter-over-temperature", "Inserter temperature high" },
  { "printer-state-reasons.inserter-thermistor-failure", "Inserter temperature sensor failed" },
  { "printer-state-reasons.inserter-timing-failure", "Inserter failed" },
  { "printer-state-reasons.inserter-under-temperature", "Inserter temperature low" },
  { "printer-state-reasons.interpreter-cartridge-deleted", "Interpreter cartridge removed" },
  { "printer-state-reasons.interpreter-resource-unavailable", "Printer resource unavailable" },
  { "printer-state-reasons.lamp-at-eol", "Replace lamp" },
  { "printer-state-reasons.lamp-failure", "Lamp failed" },
  { "printer-state-reasons.lamp-near-eol", "Replace lamp soon" },
  { "printer-state-reasons.laser-at-eol", "Replace laser" },
  { "printer-state-reasons.laser-failure", "Laser failed" },
  { "printer-state-reasons.laser-near-eol", "Replace laser soon" },
  { "printer-state-reasons.make-envelope-jam", "Envelope maker jammed" },
  { "printer-state-reasons.make-envelope-life-almost-over", "Replace envelope maker soon" },
  { "printer-state-reasons.make-envelope-life-over", "Replace envelope maker" },
  { "printer-state-reasons.make-envelope-missing", "Install envelope maker" },
  { "printer-state-reasons.make-envelope-motor-failure", "Envelope maker motor failed" },
  { "printer-state-reasons.make-envelope-over-temperature", "Envelope maker temperature high" },
  { "printer-state-reasons.make-envelope-power-saver", "Envelope maker in power save mode" },
  { "printer-state-reasons.make-envelope-thermistor-failure", "Envelope maker temperature sensor failed" },
  { "printer-state-reasons.make-envelope-timing-failure", "Envelope maker failed" },
  { "printer-state-reasons.make-envelope-under-temperature", "Envelope maker temperature low" },
  { "printer-state-reasons.marker-adjusting-print-quality", "Optimizing print quality" },
  { "printer-state-reasons.marker-cleaner-missing", "Install cleaner" },
  { "printer-state-reasons.marker-developer-almost-empty", "Developer almost empty" },
  { "printer-state-reasons.marker-developer-empty", "Developer empty" },
  { "printer-state-reasons.marker-developer-missing", "Replace developer" },
  { "printer-state-reasons.marker-fuser-missing", "Install fuser" },
  { "printer-state-reasons.marker-fuser-thermistor-failure", "Fuser temperature sensor failed" },
  { "printer-state-reasons.marker-fuser-timing-failure", "Fuser failed" },
  { "printer-state-reasons.marker-ink-almost-empty", "Ink low" },
  { "printer-state-reasons.marker-ink-empty", "Ink empty" },
  { "printer-state-reasons.marker-ink-missing", "Install ink" },
  { "printer-state-reasons.marker-opc-missing", "Install OPC" },
  { "printer-state-reasons.marker-print-ribbon-almost-empty", "Print ribbon low" },
  { "printer-state-reasons.marker-print-ribbon-empty", "Print ribbon empty" },
  { "printer-state-reasons.marker-print-ribbon-missing", "Install print ribbon" },
  { "printer-state-reasons.marker-supply-almost-empty", "Ink/toner very low" },
  { "printer-state-reasons.marker-supply-empty", "Ink/toner empty" },
  { "printer-state-reasons.marker-supply-low", "Ink/toner low" },
  { "printer-state-reasons.marker-toner-cartridge-missing", "Replace toner cartridge" },
  { "printer-state-reasons.marker-toner-missing", "Install toner" },
  { "printer-state-reasons.marker-waste-almost-full", "Ink/toner waste bin almost full" },
  { "printer-state-reasons.marker-waste-full", "Ink/toner waste bin full" },
  { "printer-state-reasons.marker-waste-ink-receptacle-almost-full", "Ink waste bin almost full" },
  { "printer-state-reasons.marker-waste-ink-receptacle-full", "Ink waste bin full" },
  { "printer-state-reasons.marker-waste-ink-receptacle-missing", "Replace ink waste bin" },
  { "printer-state-reasons.marker-waste-missing", "Install waste bin" },
  { "printer-state-reasons.marker-waste-toner-receptacle-almost-full", "Toner waste bin almost full" },
  { "printer-state-reasons.marker-waste-toner-receptacle-full", "Toner waste bin full" },
  { "printer-state-reasons.marker-waste-toner-receptacle-missing", "Install toner waste bin" },
  { "printer-state-reasons.material-empty", "Material empty" },
  { "printer-state-reasons.material-low", "Material low" },
  { "printer-state-reasons.material-needed", "Load material" },
  { "printer-state-reasons.media-drying", "Paper drying" },
  { "printer-state-reasons.media-empty", "Paper tray is empty" },
  { "printer-state-reasons.media-jam", "Paper jam" },
  { "printer-state-reasons.media-low", "Paper tray is almost empty" },
  { "printer-state-reasons.media-needed", "Load paper" },
  { "printer-state-reasons.media-path-cannot-duplex-media-selected", "Cannot print 2-sided" },
  { "printer-state-reasons.media-path-failure", "Paper path failed" },
  { "printer-state-reasons.media-path-input-empty", "Paper input empty" },
  { "printer-state-reasons.media-path-input-feed-error", "Paper input error" },
  { "printer-state-reasons.media-path-input-jam", "Paper input jam" },
  { "printer-state-reasons.media-path-input-request", "Load paper" },
  { "printer-state-reasons.media-path-jam", "Paper path jam" },
  { "printer-state-reasons.media-path-media-tray-almost-full", "Output tray almost full" },
  { "printer-state-reasons.media-path-media-tray-full", "Output tray full" },
  { "printer-state-reasons.media-path-media-tray-missing", "Install output tray" },
  { "printer-state-reasons.media-path-output-feed-error", "Paper output error" },
  { "printer-state-reasons.media-path-output-full", "Paper output full" },
  { "printer-state-reasons.media-path-output-jam", "Paper output jam" },
  { "printer-state-reasons.media-path-pick-roller-failure", "Pickup roller failed" },
  { "printer-state-reasons.media-path-pick-roller-life-over", "Replace pickup roller" },
  { "printer-state-reasons.media-path-pick-roller-life-warn", "Replace pickup roller soon" },
  { "printer-state-reasons.media-path-pick-roller-missing", "Install pickup roller" },
  { "printer-state-reasons.motor-failure", "Motor failed" },
  { "printer-state-reasons.moving-to-paused", "Printer going offline" },
  { "printer-state-reasons.opc-life-over", "Replace OPC" },
  { "printer-state-reasons.opc-near-eol", "Replace OPC soon" },
  { "printer-state-reasons.other", "Check printer" },
  { "printer-state-reasons.output-area-almost-full", "Output bin almost full" },
  { "printer-state-reasons.output-area-full", "Output bin full" },
  { "printer-state-reasons.output-mailbox-select-failure", "Mailbox failed" },
  { "printer-state-reasons.output-media-tray-failure", "Output bin failed" },
  { "printer-state-reasons.output-media-tray-feed-error", "Output bin feeder failed" },
  { "printer-state-reasons.output-media-tray-jam", "Output bin jammed" },
  { "printer-state-reasons.output-tray-missing", "Install output tray" },
  { "printer-state-reasons.perforator-configuration-change", "Perforator changed" },
  { "printer-state-reasons.perforator-jam", "Perforator jammed" },
  { "printer-state-reasons.perforator-life-almost-over", "Replace perforator soon" },
  { "printer-state-reasons.perforator-life-over", "Replace perforator" },
  { "printer-state-reasons.perforator-missing", "Install perforator" },
  { "printer-state-reasons.perforator-motor-failure", "Perforator motor failed" },
  { "printer-state-reasons.perforator-over-temperature", "Perforator temperature high" },
  { "printer-state-reasons.perforator-power-saver", "Perforator in power save mode" },
  { "printer-state-reasons.perforator-thermistor-failure", "Perforator temperature sensor failed" },
  { "printer-state-reasons.perforator-timing-failure", "Perforator failed" },
  { "printer-state-reasons.perforator-under-temperature", "Perforator temperature low" },
  { "printer-state-reasons.platform-failure", "Platform failed" },
  { "printer-state-reasons.printer-manual-reset", "Printer reset manually" },
  { "printer-state-reasons.printer-nms-reset", "Printer reset remotely" },
  { "printer-state-reasons.puncher-configuration-change", "Puncher changed" },
  { "printer-state-reasons.puncher-jam", "Puncher jammed" },
  { "printer-state-reasons.puncher-life-almost-over", "Replace puncher soon" },
  { "printer-state-reasons.puncher-life-over", "Replace puncher" },
  { "printer-state-reasons.puncher-missing", "Install puncher" },
  { "printer-state-reasons.puncher-motor-failure", "Puncher motor failed" },
  { "printer-state-reasons.puncher-over-temperature", "Puncher temperature high" },
  { "printer-state-reasons.puncher-power-saver", "Puncher in power save mode" },
  { "printer-state-reasons.puncher-thermistor-failure", "Puncher temperature sensor failed" },
  { "printer-state-reasons.puncher-timing-failure", "Puncher failed" },
  { "printer-state-reasons.puncher-under-temperature", "Puncher temperature low" },
  { "printer-state-reasons.scan-media-path-failure", "Scanner ADF failed" },
  { "printer-state-reasons.scan-media-path-input-empty", "Scanner ADF empty" },
  { "printer-state-reasons.scan-media-path-input-feed-error", "Scanner ADF load error " },
  { "printer-state-reasons.scan-media-path-input-jam", "Scanner ADF jammed" },
  { "printer-state-reasons.scan-media-path-input-request", "Load document" },
  { "printer-state-reasons.scan-media-path-jam", "Scanner ADF jammed" },
  { "printer-state-reasons.scan-media-path-output-feed-error", "Scanner ADF eject error" },
  { "printer-state-reasons.scan-media-path-output-full", "Scanner ADF full" },
  { "printer-state-reasons.scan-media-path-output-jam", "Scanner ADF jammed" },
  { "printer-state-reasons.scan-media-path-pick-roller-failure", "Scanner ADF pick roller failed" },
  { "printer-state-reasons.scan-media-path-pick-roller-life-over", "Replace scanner ADF pick roller" },
  { "printer-state-reasons.scan-media-path-pick-roller-life-warn", "Replace scanner ADF pick roller soon" },
  { "printer-state-reasons.scan-media-path-pick-roller-missing", "Install scanner ADF pick roller" },
  { "printer-state-reasons.scan-media-path-tray-almost-full", "Scanner ADF output tray almost full" },
  { "printer-state-reasons.scan-media-path-tray-full", "Scanner ADF output tray full" },
  { "printer-state-reasons.scan-media-path-tray-missing", "Install scanner ADF output tray" },
  { "printer-state-reasons.scanner-light-failure", "Scan light failed" },
  { "printer-state-reasons.scanner-light-life-almost-over", "Replace scan light soon" },
  { "printer-state-reasons.scanner-light-life-over", "Replace scan light" },
  { "printer-state-reasons.scanner-light-missing", "Install scan light" },
  { "printer-state-reasons.scanner-sensor-failure", "Scan sensor failed" },
  { "printer-state-reasons.scanner-sensor-life-almost-over", "Replace scan sensor soon" },
  { "printer-state-reasons.scanner-sensor-life-over", "Replace scan sensor" },
  { "printer-state-reasons.scanner-sensor-missing", "Install scan sensor" },
  { "printer-state-reasons.separation-cutter-configuration-change", "Separation cutter changed" },
  { "printer-state-reasons.separation-cutter-jam", "Separation cutter jammed" },
  { "printer-state-reasons.separation-cutter-life-almost-over", "Replace separation cutter soon" },
  { "printer-state-reasons.separation-cutter-life-over", "Replace separation cutter" },
  { "printer-state-reasons.separation-cutter-missing", "Install separation cutter" },
  { "printer-state-reasons.separation-cutter-motor-failure", "Separation cutter motor failed" },
  { "printer-state-reasons.separation-cutter-over-temperature", "Separation cutter temperature high" },
  { "printer-state-reasons.separation-cutter-power-saver", "Separation cutter in power save mode" },
  { "printer-state-reasons.separation-cutter-thermistor-failure", "Separation cutter temperature sensor failed" },
  { "printer-state-reasons.separation-cutter-timing-failure", "Separation cutter failed" },
  { "printer-state-reasons.separation-cutter-under-temperature", "Separation cutter temperature low" },
  { "printer-state-reasons.sheet-rotator-configuration-change", "Sheet rotator changed" },
  { "printer-state-reasons.sheet-rotator-jam", "Sheet rotator jammed" },
  { "printer-state-reasons.sheet-rotator-life-almost-over", "Replace sheet rotator soon" },
  { "printer-state-reasons.sheet-rotator-life-over", "Replace sheet rotator" },
  { "printer-state-reasons.sheet-rotator-missing", "Install sheet rotator" },
  { "printer-state-reasons.sheet-rotator-motor-failure", "Sheet rotator motor failed" },
  { "printer-state-reasons.sheet-rotator-over-temperature", "Sheet rotator temperature high" },
  { "printer-state-reasons.sheet-rotator-power-saver", "Sheet rotator in power save mode" },
  { "printer-state-reasons.sheet-rotator-thermistor-failure", "Sheet rotator temperature sensor failed" },
  { "printer-state-reasons.sheet-rotator-timing-failure", "Sheet rotator failed" },
  { "printer-state-reasons.sheet-rotator-under-temperature", "Sheet rotator temperature low" },
  { "printer-state-reasons.shutdown", "Printer offline" },
  { "printer-state-reasons.slitter-configuration-change", "Slitter changed" },
  { "printer-state-reasons.slitter-jam", "Slitter jammed" },
  { "printer-state-reasons.slitter-life-almost-over", "Replace slitter soon" },
  { "printer-state-reasons.slitter-life-over", "Replace slitter" },
  { "printer-state-reasons.slitter-missing", "Install slitter" },
  { "printer-state-reasons.slitter-motor-failure", "Slitter motor failed" },
  { "printer-state-reasons.slitter-over-temperature", "Slitter temperature high" },
  { "printer-state-reasons.slitter-power-saver", "Slitter in power save mode" },
  { "printer-state-reasons.slitter-thermistor-failure", "Slitter temperature sensor failed" },
  { "printer-state-reasons.slitter-timing-failure", "Slitter failed" },
  { "printer-state-reasons.slitter-under-temperature", "Slitter temperature low" },
  { "printer-state-reasons.spool-area-full", "Printer busy" },
  { "printer-state-reasons.stacker-configuration-change", "Stacker changed" },
  { "printer-state-reasons.stacker-jam", "Stacker jammed" },
  { "printer-state-reasons.stacker-life-almost-over", "Replace stacker soon" },
  { "printer-state-reasons.stacker-life-over", "Replace stacker" },
  { "printer-state-reasons.stacker-missing", "Install stacker" },
  { "printer-state-reasons.stacker-motor-failure", "Stacker motor failed" },
  { "printer-state-reasons.stacker-over-temperature", "Stacker temperature high" },
  { "printer-state-reasons.stacker-power-saver", "Stacker in power save mode" },
  { "printer-state-reasons.stacker-thermistor-failure", "Stacker temperature sensor failed" },
  { "printer-state-reasons.stacker-timing-failure", "Stacker failed" },
  { "printer-state-reasons.stacker-under-temperature", "Stacker temperature low" },
  { "printer-state-reasons.stapler-configuration-change", "Stapler changed" },
  { "printer-state-reasons.stapler-jam", "Stapler jammed" },
  { "printer-state-reasons.stapler-life-almost-over", "Replace stapler soon" },
  { "printer-state-reasons.stapler-life-over", "Replace stapler" },
  { "printer-state-reasons.stapler-missing", "Install stapler" },
  { "printer-state-reasons.stapler-motor-failure", "Stapler motor failed" },
  { "printer-state-reasons.stapler-over-temperature", "Stapler temperature high" },
  { "printer-state-reasons.stapler-power-saver", "Stapler in power save mode" },
  { "printer-state-reasons.stapler-thermistor-failure", "Stapler temperature sensor failed" },
  { "printer-state-reasons.stapler-timing-failure", "Stapler failed" },
  { "printer-state-reasons.stapler-under-temperature", "Stapler temperature low" },
  { "printer-state-reasons.stitcher-configuration-change", "Stitcher changed" },
  { "printer-state-reasons.stitcher-jam", "Stitcher jammed" },
  { "printer-state-reasons.stitcher-life-almost-over", "Replace stitcher soon" },
  { "printer-state-reasons.stitcher-life-over", "Replace stitcher" },
  { "printer-state-reasons.stitcher-missing", "Install stitcher" },
  { "printer-state-reasons.stitcher-motor-failure", "Stitcher motor failed" },
  { "printer-state-reasons.stitcher-over-temperature", "Stitcher temperature high" },
  { "printer-state-reasons.stitcher-power-saver", "Stitcher in power save mode" },
  { "printer-state-reasons.stitcher-thermistor-failure", "Stitcher temperature sensor failed" },
  { "printer-state-reasons.stitcher-timing-failure", "Stitcher failed" },
  { "printer-state-reasons.stitcher-under-temperature", "Stitcher temperature low" },
  { "printer-state-reasons.stopped-partly", "Partially stopped" },
  { "printer-state-reasons.subunit-configuration-change", "Subunit changed" },
  { "printer-state-reasons.subunit-jam", "Subunit jammed" },
  { "printer-state-reasons.subunit-life-almost-over", "Replace subunit soon" },
  { "printer-state-reasons.subunit-life-over", "Replace subunit" },
  { "printer-state-reasons.subunit-missing", "Install subunit" },
  { "printer-state-reasons.subunit-motor-failure", "Subunit motor failed" },
  { "printer-state-reasons.subunit-over-temperature", "Subunit temperature high" },
  { "printer-state-reasons.subunit-power-saver", "Subunit in power save mode" },
  { "printer-state-reasons.subunit-thermistor-failure", "Subunit temperature sensor failed" },
  { "printer-state-reasons.subunit-timing-failure", "Subunit failed" },
  { "printer-state-reasons.subunit-under-temperature", "Subunit temperature low" },
  { "printer-state-reasons.timed-out", "Printer stopped responding" },
  { "printer-state-reasons.toner-empty", "Out of toner" },
  { "printer-state-reasons.toner-low", "Toner low" },
  { "printer-state-reasons.trimmer-configuration-change", "Trimmer changed" },
  { "printer-state-reasons.trimmer-jam", "Trimmer jammed" },
  { "printer-state-reasons.trimmer-life-almost-over", "Replace trimmer soon" },
  { "printer-state-reasons.trimmer-life-over", "Replace trimmer" },
  { "printer-state-reasons.trimmer-missing", "Install trimmer" },
  { "printer-state-reasons.trimmer-motor-failure", "Trimmer motor failed" },
  { "printer-state-reasons.trimmer-over-temperature", "Trimmer temperature high" },
  { "printer-state-reasons.trimmer-power-saver", "Trimmer in power save mode" },
  { "printer-state-reasons.trimmer-thermistor-failure", "Trimmer temperature sensor failed" },
  { "printer-state-reasons.trimmer-timing-failure", "Trimmer failed" },
  { "printer-state-reasons.trimmer-under-temperature", "Trimmer temperature low" },
  { "printer-state-reasons.wrapper-configuration-change", "Wrapper changed" },
  { "printer-state-reasons.wrapper-jam", "Wrapper jammed" },
  { "printer-state-reasons.wrapper-life-almost-over", "Replace wrapper soon" },
  { "printer-state-reasons.wrapper-life-over", "Replace wrapper" },
  { "printer-state-reasons.wrapper-missing", "Install wrapper" },
  { "printer-state-reasons.wrapper-motor-failure", "Wrapper motor failed" },
  { "printer-state-reasons.wrapper-over-temperature", "Wrapper temperature high" },
  { "printer-state-reasons.wrapper-power-saver", "Wrapper in power save mode" },
  { "printer-state-reasons.wrapper-thermistor-failure", "Wrapper temperature sensor failed" },
  { "printer-state-reasons.wrapper-timing-failure", "Wrapper failed" },
  { "printer-state-reasons.wrapper-under-temperature", "Wrapper temperature low" },
  { "punching-reference-edge", "Punch Edge" },
  { "retry-time-out", "Retry Timeout" },
  { "separator-sheets-type", "Type of Separator Sheets" },
  { "separator-sheets-type.both-sheets", "Start and end sheets" },
  { "sides", "2-Sided Printing" },
  { "sides.one-sided", "Off" },
  { "sides.two-sided-long-edge", "On (portrait)" },
  { "sides.two-sided-short-edge", "On (landscape)" },
  { "stitching", "Staple" },
  { "stitching-method", "Staple Method" },
  { "stitching-reference-edge", "Staple Edge" },
  { "stitching-when", "Staple After" },
  { "trimming", "Cut" },
  { "trimming-offset", "Cut Position" },
  { "trimming-reference-edge", "Cut Edge" },
  { "trimming-type", "Type of Cut" },
  { "trimming-when", "Cut After" },
  { "trimming-when.after-documents", "Every document" },
  { "trimming-when.after-job", "Job" },
  { "trimming-when.after-sets", "Every set" },
  { "trimming-when.after-sheets", "Every page" }
};


//
// 'ipp_compare_loc()' - Compare two localizations.
//

static int				// O - Result of comparison
ipp_compare_loc(ipp_loc_t *a,		// I - First localization
                ipp_loc_t *b)		// I - Second localization
{
  return (strcmp(a->id, b->id));
}


//
// 'ipp_get_localized()' - Get the localized string for a value.
//
// Converts keywords of the form "some-name" to "Some Name".
//

static const char *			// O - Localized string
ipp_get_localized(const char *attribute,// I - Attribute
		  const char *name,	// I - Name string
		  const char *value,	// I - Value
		  char       *buffer,	// I - Buffer
		  size_t     bufsize)	// I - Size of buffer
{
  char		*bufptr,		// Pointer into buffer
		*bufend,		// End of buffer
		id[256];		// Localization identifier
  const char    *suffix = NULL;         // Suffix to add
  ipp_loc_t	key,			// Search key
		*match;			// Matching localization
  const char	*size;			// Size string


  // Sort the strings array the first time...
  if (!ipp_strings_sorted)
  {
    ipp_strings_sorted = true;

    qsort(ipp_strings, sizeof(ipp_strings) / sizeof(ipp_strings[0]), sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc);
  }

  // See if we have a canned localization...
  if (*attribute)
    snprintf(id, sizeof(id), "%s.%s", attribute, value);
  else
    ipp_copy_string(id, value, sizeof(id));

  key.id = id;
  if ((match = (ipp_loc_t *)bsearch(&key, ipp_strings, sizeof(ipp_strings) / sizeof(ipp_strings[0]), sizeof(ipp_loc_t), (int (*)(const void *, const void *))ipp_compare_loc)) != NULL)
    return (match->str);
  else if (!*attribute)
    return (NULL);

  // Not a canned localization, is this a media size name?
  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  if (!strcmp(attribute, "media") && (size = strrchr(value, '_')) != NULL)
  {
    // Yes, use the ISO/US name or the size from the end of the self-describing name...
    if (!strncmp(value, "iso_", 4))
    {
      // Use ISO name...
      value += 4;

      *bufptr++ = toupper(*value++);

      if (buffer[0] == 'C')
      {
        ipp_copy_string(bufptr, "Envelope ", bufend - bufptr + 1);
        bufptr += strlen(bufptr);
      }

      while (*value && *value != '_' && bufptr < bufend)
      {
        if (!strncmp(value, "-extra", 6))
        {
          value += 6;
          ipp_copy_string(bufptr, " (extra)", bufend - bufptr + 1);
          bufptr += strlen(bufptr);
        }
        else if (!strncmp(value, "-tab", 4))
        {
          value += 4;
          ipp_copy_string(bufptr, " (tab)", bufend - bufptr + 1);
          bufptr += strlen(bufptr);
        }
        else
	  *bufptr++ = *value++;
      }

      *bufptr = '\0';
    }
    else
    {
      // Use dimensional name...
      size ++;
      while (*size && bufptr < bufend)
      {
        if (*size == 'x')
        {
          size ++;
          ipp_copy_string(bufptr, " x ", bufend - bufptr + 1);
          bufptr += strlen(bufptr);
        }
        else if (!strcmp(size, "in"))
        {
          size += 2;

          // Unicode double prime character
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

  // Common words?
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

  // Nope, make one from the keyword/name...
  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  while (*name && bufptr < bufend)
  {
    if (!strncmp(name, "fold-", 5))
    {
      name += 5;
      suffix = " fold";

      if (!*name)
        break;
    }
    else if (!strncmp(name, "input-", 6) && strcmp(attribute, "printer-state-reasons"))
    {
      name += 6;
      ipp_copy_string(bufptr, "Scan ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "jdf-", 4))
    {
      name += 4;
      ipp_copy_string(bufptr, "JDF ", bufend - bufptr + 1);
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

      ipp_copy_string(bufptr, "Envelope maker ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-dual-", 11))
    {
      name += 11;

      ipp_copy_string(bufptr, "2-hole punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-triple-", 13))
    {
      name += 13;

      ipp_copy_string(bufptr, "3-hole punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-quad-", 11))
    {
      name += 11;

      ipp_copy_string(bufptr, "4-hole punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "punch-multiple-", 15))
    {
      name += 15;

      ipp_copy_string(bufptr, "Multi-hole punch ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "staple-dual-", 12))
    {
      name += 12;

      ipp_copy_string(bufptr, "2 staples on ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "staple-triple-", 14))
    {
      name += 14;

      ipp_copy_string(bufptr, "3 staples on ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "reference-edge", 14))
    {
      name += 10;			// Drop "reference-"
    }
    else if (!strncmp(name, "rgb_", 4))
    {
      name += 4;
      ipp_copy_string(bufptr, "RGB ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "rgba_", 5))
    {
      name += 5;
      ipp_copy_string(bufptr, "RGBA ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }
    else if (!strncmp(name, "cmyk_", 5))
    {
      name += 5;
      ipp_copy_string(bufptr, "CMYK ", bufend - bufptr + 1);
      bufptr += strlen(bufptr);

      if (!*name)
        break;
    }

    if (bufptr > buffer)
      *bufptr++ = *name++;
    else
      *bufptr++ = toupper(*name++ & 255);

    while (*name && bufptr < bufend)
    {
      if (!strcmp(name, "-col"))
      {
        name += 4;
        break;
      }
      else if (!strcmp(name, "-face-down"))
      {
	name += 10;
	ipp_copy_string(bufptr, " Face-down", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
	break;
      }
      else if (!strcmp(name, "-face-up"))
      {
	name += 8;
	ipp_copy_string(bufptr, " Face-up", bufend - bufptr + 1);
	bufptr += strlen(bufptr);
	break;
      }
      else if (!strcmp(name, "-id"))
      {
	name += 3;
	ipp_copy_string(bufptr, " ID", bufend - bufptr + 1);
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
	ipp_copy_string(bufptr, " URI", bufend - bufptr + 1);
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
    ipp_copy_string(bufptr, suffix, bufend - bufptr + 1);
  else
    *bufptr = '\0';

  return (buffer);
}
#endif // !IPP_STRINGS_H
