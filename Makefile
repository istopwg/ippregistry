#
# Makefile for IANA IPP registry tools. Depends on Mini-XML, available at:
#
#   http://www.msweet.org/projects.php/mini-xml
#
# Copyright © 2018 by The IEEE-ISTO Printer Working Group.
# Copyright © 2008-2018 by Michael R Sweet
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

CFLAGS	=	-g -Os -Wall -I$(HOME)/oss/include -I/usr/local/include -L$(HOME)/oss/lib -L/usr/local/lib
TARGETS	=	register regtoc regtosm regtostrings

all:	$(TARGETS)

clean:
	/bin/rm -f $(TARGETS)
	for file i $(TARGETS); do \
		test -d $$file.dSYM && /bin/rm -rf $$file.dSYM; \
	done

register:	register.c ipp-registry.h
	cc $(CFLAGS) -o register register.c -lmxml

regtoc:	regtoc.c ipp-registry.h ipp-strings.h
	cc $(CFLAGS) -o regtoc regtoc.c -lmxml

regtosm:	regtosm.c ipp-registry.h
	cc $(CFLAGS) -o regtosm regtosm.c -lmxml

regtostrings:	regtostrings.c ipp-registry.h ipp-strings.h
	cc $(CFLAGS) -o regtostrings regtostrings.c -lmxml

strings:	localizations/ipp.strings localizations/ipp.pot

.PHONY: preview

preview:
	xsltproc preview/iana-registry.xsl ipp-registrations.xml >preview/ipp-registrations.xhtml

.PHONY: iana-ipp-registrations.xml

iana-ipp-registrations.xml:
	curl -o $@ http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml

localizations/ipp.pot:	regtostrings iana-ipp-registrations.xml
	./regtostrings --po iana-ipp-registrations.xml >localizations/ipp.pot

localizations/ipp.strings:	regtostrings iana-ipp-registrations.xml
	./regtostrings iana-ipp-registrations.xml >localizations/ipp.strings
