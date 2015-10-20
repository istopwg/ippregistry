# IANA IPP Registry Tools

This repository contains the tools used by the PWG IPP workgroup to maintain the [IANA IPP registry](http://www.iana.org/assignments/ipp-registrations) and sample IPP strings file. Two tools and a script are provided:

- apply-pending.sh, a script that applies a list of plain text registrations;
- register, a tool to apply plain text registration templates to the current IANA IPP XML registry file; and
- regtostrings, a tool for generating the ipp.strings file that provides localized strings for all registered attributes and values.

## Prerequisites

The tools use the [Mini-XML](http://www.msweet.org/projects.php/mini-xml) library to access the XML registry file. Otherwise you'll need a C compiler and a "make" program.

## Compiling

Run "make". The included makefile should not require any additional tweaking.


# Using the "apply-pending.sh" Script

The "apply-pending.sh" script applies one or more plain text registrations to a source XML registry file:

    ./apply-pending.sh [list-of-templates.txt]

If omitted, the script will use all .txt files starting with "pwg51" or "vendor-".

The source XML file should be the current IANA IPP registration file at: (http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml) and must be named "base-registrations.xml".

The destination XML file is named "ipp-registrations.xml" and may be uploaded to the PWG FTP server or submitted to IANA.


# Using the "register" Tool

The "register" tool applies a single plain text registration template to the specified XML file:

    ./register -o destination.xml -t 'TITLE' -x http://example.com/file source.xml template.txt

The following options are understood:

- "-o destination.xml" - specifies an output (XML) file
- "-t title" - specifies the title of the reference for the registration
- "-x url" - specifies the URL of the reference for the registration

The source XML file should be the current IANA IPP registration file at: (http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml).

The template file is a plain text registration of attributes, values, status codes, operations, etc. as documented in [RFC 2911](http://tools.ietf.org/html/rfc2911).


# Using the "regtostrings" Tool

The "regtostrings" tool reads the source XML registry file and writes an Apple .strings file to the standard output. Usually you will want to sort the output as well:

    ./regtostrings ipp-registrations.xml | sort -u >ipp.strings

Again, the source XML file should be the current IANA IPP registration file at: (http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml).
