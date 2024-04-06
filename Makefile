#
# Makefile for IANA IPP registry tools. Depends on Mini-XML, available at:
#
#   https://www.msweet.org/mxml
#
# Copyright © 2018-2024 by The IEEE-ISTO Printer Working Group.
# Copyright © 2008-2018 by Michael R Sweet
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#


# Flag this as a POSIX makefile
.POSIX:


# Options...
CFLAGS		=	$(OPTIM) $(CPPFLAGS)
CPPFLAGS	=	-Wall `pkg-config --cflags mxml4`
LIBS		=	`pkg-config --libs mxml4`
OPTIM		=	-g -Os


# Default targets...
SOURCES		=	register.c regtosm.c regtostrings.c
TARGETS		=	register regtosm regtostrings


# Make everything...
all:	$(TARGETS)


# Clean everything...
clean:
	/bin/rm -f $(TARGETS)
	for file in $(TARGETS); do \
		test -d $$file.dSYM && /bin/rm -rf $$file.dSYM; \
	done


# Analyze code with the Clang static analyzer <https://clang-analyzer.llvm.org>
clang:
	clang $(CPPFLAGS) --analyze $(SOURCES) 2>clang.log
	rm -rf $(SOURCES:.c=.plist)
	test -s clang.log && (echo "$(GHA_ERROR)Clang detected issues."; echo ""; cat clang.log; exit 1) || exit 0


# Analyze code using Cppcheck <http://cppcheck.sourceforge.net>
cppcheck:
	cppcheck $(CPPFLAGS) --template=gcc --addon=cert.py --suppressions-list=.cppcheck $(SOURCES) 2>cppcheck.log
	test -s cppcheck.log && (echo "$(GHA_ERROR)Cppcheck detected issues."; echo ""; cat cppcheck.log; exit 1) || exit 0


# Build the register program which manages the IANA IPP registry XML file
register:	register.c ipp-registry.h
	$(CC) $(CFLAGS) -o register register.c $(LIBS)


# Build the regtosm program which creates a Semantic Model schema
regtosm:	regtosm.c ipp-registry.h
	$(CC) $(CFLAGS) -o regtosm regtosm.c $(LIBS)


# Build the regtostrings program which creates a .strings file
regtostrings:	regtostrings.c ipp-registry.h ipp-strings.h
	$(CC) $(CFLAGS) -o regtostrings regtostrings.c $(LIBS)


# Update the localizations of the IANA IPP registry
.PHONY: strings
strings:	localizations/base.strings
#	for lang in de es fr it; do \
#		echo Generating $$lang strings...; \
#		./regtostrings-mt --language $$lang iana-ipp-registrations.xml >localizations/ipp-$$lang.strings; \
#	done


# Preview the
.PHONY: preview
preview:
	xsltproc preview/iana-registry.xsl ipp-registrations.xml >preview/ipp-registrations.xhtml


# Update the IANA IPP registration XML file...
.PHONY: iana-ipp-registrations.xml
iana-ipp-registrations.xml:
	curl -z $@ -o $@ http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml


# Update the base .strings file for the IANA IPP registry...
localizations/base.strings:	regtostrings iana-ipp-registrations.xml
	./regtostrings iana-ipp-registrations.xml >localizations/base.strings
