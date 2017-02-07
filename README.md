# IANA IPP Registry Tools

This repository contains the tools used by the PWG IPP workgroup to maintain the [IANA IPP registry](http://www.iana.org/assignments/ipp-registrations) and sample IPP strings file. Three tools and a script are provided:

- apply-pending.sh, a script that applies a list of plain text registrations;
- register, a tool to apply plain text registration templates to the current IANA IPP XML registry file; and
- regtostrings, a tool for generating the ipp.strings file that provides localized strings for all registered attributes and values.
- regtosm, a prototype tool for generating a PWG Semantic Model XML schema (in development, *not* for production use!)


## Prerequisites

The tools use the [Mini-XML](http://www.msweet.org/projects.php/mini-xml) library to access the XML registry file. Otherwise you'll need a C compiler and a "make" program.

The "apply-pending.sh" script uses the curl program to download the current IANA IPP registry file.


## Compiling

Run "make". The included makefile should not require any additional tweaking.


# Using the "apply-pending.sh" Script

The "apply-pending.sh" script applies one or more plain text registrations to the current IANA IPP XML registry file:

    ./apply-pending.sh [list-of-templates.txt]

If omitted, the script will use all .txt files in the "pending" directory.

The current IANA IPP registry (input) file is stored in the current directory with the name "iana-ipp-registrations.xml".

The output XML file is named "ipp-registrations.xml" and may be uploaded to the PWG FTP server or submitted to IANA.


# Using the "register" Tool

The "register" tool applies a single plain text registration template to the specified XML file:

    ./register -o destination.xml -t 'TITLE' -x http://example.com/file source.xml template.txt

The following options are understood:

- "-o destination.xml" - specifies an output (XML) file
- "-t title" - specifies the title of the reference for the registration
- "-x url" - specifies the URL of the reference for the registration

The source XML file should be the current IANA IPP registration file at: [http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml](http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml).

The template file is a plain text registration of attributes, values, status codes, operations, etc. as documented in [RFC 8011](http://tools.ietf.org/html/rfc8011).


# Using the "regtostrings" Tool

The "regtostrings" tool reads the source XML registry file and writes an Apple .strings file to the standard output. Usually you will want to sort the output as well:

    ./regtostrings ipp-registrations.xml | sort -u >ipp.strings

The source XML file should be the current IANA IPP registration file at: (http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml).


# Using the "regtosm" Tool

" Note: The "regtosm" tool is under active development and should not be used as a replacement for the currently approved and published PWG Semantic Model schema.

The "regtosm" tool reads the source XML registry file and writes a PWG Semantic Model XML schema to the specified directory. The following command will write the current registry to a directory called "test-schema":

    ./regtosm ipp-registrations test-schema

The source XML file should be the current IANA IPP registration file at: (http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml).
