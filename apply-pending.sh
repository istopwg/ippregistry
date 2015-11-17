#!/bin/sh
src="iana-ipp-registrations.xml"
dst="-o ipp-registrations.xml"

echo Fetching IANA source registry...
curl -z $src -o $src http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml

if test $# -gt 0; then
	files="$@"
else
	files=`ls -1 pending/*.txt`
fi

echo Applying pending registrations:
for file in $files; do
	title=`basename $file .txt | sed -e '1,$s/^vendor-//' | awk '{print toupper($1);}'`
	url=`head -1 $file`
	echo "    $file: $url"
	./register $dst -x $url -t $title $src $file
	src="ipp-registrations.xml"
	dst=""
	echo ""
done

echo Cleaning up...
sed -e '1,$s/&quot;/"/g' ipp-registrations.xml >ipp-registrations.xml.N
mv ipp-registrations.xml.N ipp-registrations.xml
