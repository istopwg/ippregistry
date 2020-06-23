IANA IPP Registry Tools
=======================

This repository contains the tools used by the PWG IPP workgroup to maintain
the [IANA IPP registry](https://www.iana.org/assignments/ipp-registrations) and
sample IPP strings file.  Four tools and a script are provided:

- `apply-pending.sh`: A script that applies a list of plain text registrations;
- `register`: A tool to apply plain text registration templates to the current
  IANA IPP XML registry file;
- `regtocups`: A prototype tool for generating encoding and validation functions
  using the CUPS API;
- `regtosm`: A prototype tool for generating a PWG Semantic Model XML schema;
  and
- `regtostrings`: A tool for generating the `ipp.strings` file that provides
  localized strings for all registered attributes and values;


Prerequisites
-------------

The tools use the [Mini-XML](https://www.msweet.org/mxml) library to access the
XML registry file.  Otherwise you need a C compiler and a `make` program.

The "apply-pending.sh" script uses the [curl](https://curl.haxx.se) program to
download the current IANA IPP registry file.


Compiling
---------

Run `make`.  The included makefile should not require any additional tweaking.


Using the "apply-pending.sh" Script
-----------------------------------

The `apply-pending.sh` script applies one or more plain text registrations to
the current IANA IPP XML registry file:

    ./apply-pending.sh [list-of-templates.txt]

If omitted, the script will use all `.txt` files in the `pending` directory.

The current IANA IPP registry (input) file is cached in the current directory
with the name `iana-ipp-registrations.xml`.

The output XML file is named `ipp-registrations.xml` and may be uploaded to the
PWG FTP server or submitted to IANA.


Using the "register" Tool
-------------------------

The `register` tool applies a single plain text registration template to the
specified XML file:

    ./register -o DESTINATION.xml -t 'TITLE' -x URL SOURCE.xml TEMPLATE.txt

The following options are understood:

- `-o DESTINATION.xml`: Specifies the output (XML) file
- `-t TITLE`: Specifies the title of the reference for the registration
- `-x URL`: Specifies the URL of the reference for the registration
- `SOURCE.xml`: Specifies the input (XML) file
- `TEMPLATE.txt`: Specifies the registration template file

The source XML file should be the current IANA IPP registration file at
<https://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml>.

The template file is a plain text registration of attributes, values, status
codes, operations, etc. as documented in
[RFC 8011](https://tools.ietf.org/html/rfc8011).  This format is summarized in
the `pending/PENDING.md` file.


Using the "regtocups" Tool
--------------------------

The `regtocups` tool reads the source XML registry file and writes CUPS code to
encode or validate IPP attributes using the CUPS API:

    ./regtocups --encode [OPTIONS] SOURCE.xml >DESTINATION.c
    ./regtocups --validate [OPTIONS] SOURCE.xml >DESTINATION.c

The `--encode` option tells `regtocups` to generate a C function that encodes a
`cups_option_t` array as IPP attributes, while the `--validate` option tells it
to generate a C function that validates the attributes in an `ipp_t` message.

The common options are as follows:

- `-f FUNCTION-NAME`: Specify the function name.
- `-g "GROUP-NAME"`: Specify the attribute group name.
- `-n "ATTRIBUTE-NAME"`: Specify one or more attributes; separate attributes by
  commas or use multiple `-a` options to specify multiple attributes.

The source XML file should be the current IANA IPP registration file at
<http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml>.


Using the "regtosm" Tool
------------------------

The `regtosm` tool reads the source XML registry file and writes a PWG Semantic Model XML v3 schema to the specified directory.  The following command will write the current registry to a directory called `test-schema`:

    ./regtosm ipp-registrations.xml test-schema

The source XML file should be the current IANA IPP registration file at
<http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml>.


Using the "regtostrings" Tool
-----------------------------

The `regtostrings` tool reads the source XML registry file and writes the
localizable strings to an Apple `.strings` file, a GNU gettext `.po` file, or
a C source file to the standard output.  For example, to generate a template
`.strings` file with US English strings, run the following command:

    ./regtostrings ipp-registrations.xml >DESTINATION.strings

The `--code` option tells `regtostrings` to generate C code:

    ./regtostrings --code ipp-registrations.xml >DESTINATION.c

The `--po` option tells `regtostrings` to generate a GNU gettext `.po` file:

    ./regtostrings --po ipp-registrations.xml >DESTINATION.po

The source XML file should be the current IANA IPP registration file at
<http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml>.
