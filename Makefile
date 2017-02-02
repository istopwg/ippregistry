#
# Makefile for IANA IPP registry tools. Depends on Mini-XML, available at:
#
#   http://www.msweet.org/projects.php/mini-xml
#
# Copyright (c) 2008-2017 by Michael R Sweet
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

CFLAGS	=	-g -Os -Wall -I/usr/local/include -L/usr/local/lib
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
