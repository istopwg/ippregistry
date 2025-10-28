#!/bin/sh
#
# Script to apply pending registrations to the current IANA IPP registry.
#
# Copyright © 2018-2025 by The ISTO Printer Working Group.
# Copyright © 2008-2018 by Michael R Sweet
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

src="iana-ipp-registrations.xml"
dst="-o ipp-registrations.xml"

echo Fetching IANA source registry...
if test -f $src; then
	curl -z $src -o $src http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml
else
	curl -o $src http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml
fi

if test $# -gt 0; then
	files="$@"
else
	files=`ls -1 pending/*.txt`
fi

echo Applying pending registrations:
for file in $files; do
	title=`basename $file .txt | sed -e '1,$s/^vendor-//' -e '1,$s/^reg-//' | awk '{print toupper($1);}'`
	url=`head -1 $file`
	echo "$file: $url"
	./register $dst -x $url -t $title $src $file || exit 1
	src="ipp-registrations.xml"
	dst=""
	echo ""
done

echo Cleaning up...
sed -e '1,$s/&quot;/"/g' ipp-registrations.xml >ipp-registrations.xml.N
mv ipp-registrations.xml.N ipp-registrations.xml
