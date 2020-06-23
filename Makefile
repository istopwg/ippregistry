#
# Makefile for IANA IPP registry tools. Depends on Mini-XML, available at:
#
#   https://www.msweet.org/mxml
#
# Copyright © 2018-2020 by The IEEE-ISTO Printer Working Group.
# Copyright © 2008-2018 by Michael R Sweet
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

CFLAGS	=	-g -Wall -I/usr/local/include -L/usr/local/lib
TARGETS	=	register regtocups regtosm regtostrings

all:	$(TARGETS)

clean:
	/bin/rm -f $(TARGETS)
	for file in $(TARGETS); do \
		test -d $$file.dSYM && /bin/rm -rf $$file.dSYM; \
	done

register:	register.c ipp-registry.h
	$(CC) $(CFLAGS) -o register register.c -lmxml

regtocups:	regtocups.c ipp-registry.h
	$(CC) $(CFLAGS) -o regtocups regtocups.c -lmxml

regtosm:	regtosm.c ipp-registry.h
	$(CC) $(CFLAGS) -o regtosm regtosm.c -lmxml

regtostrings:	regtostrings.c ipp-registry.h ipp-strings.h
	$(CC) $(CFLAGS) -o regtostrings regtostrings.c -lmxml

regtostrings-mt:	regtostrings-mt.c ipp-registry.h ipp-strings.h
	$(CC) $(CFLAGS) -o regtostrings-mt regtostrings-mt.c -lmxml -lcups

strings:	localizations/ipp.c localizations/ipp.pot localizations/ipp.strings

.PHONY: preview

preview:
	xsltproc preview/iana-registry.xsl ipp-registrations.xml >preview/ipp-registrations.xhtml

.PHONY: iana-ipp-registrations.xml

iana-ipp-registrations.xml:
	curl -z $@ -o $@ http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml

localizations/ipp.c:	regtostrings iana-ipp-registrations.xml
	./regtostrings --code iana-ipp-registrations.xml >localizations/ipp.c

localizations/ipp.pot:	regtostrings iana-ipp-registrations.xml
	./regtostrings --po iana-ipp-registrations.xml >localizations/ipp.pot

localizations/ipp.strings:	regtostrings iana-ipp-registrations.xml
	./regtostrings iana-ipp-registrations.xml >localizations/ipp.strings

mtstrings:	regtostrings-mt iana-ipp-registrations.xml
	for lang in de es fr it; do \
		echo Generating $$lang strings...; \
		./regtostrings-mt --language $$lang iana-ipp-registrations.xml >localizations/ipp-$$lang.strings; \
	done
