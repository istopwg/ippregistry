#
# Makefile for IANA IPP registry tools. Depends on Mini-XML, available at:
#
#   http://www.msweet.org/projects.php/mini-xml
#

all:	register regtostrings

register:	register.c
	cc -o register -g register.c -lmxml

regtostrings:	regtostrings.c
	cc -o regtostrings -g regtostrings.c -lmxml
