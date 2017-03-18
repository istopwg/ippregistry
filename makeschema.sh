#!/bin/sh
#
# Script to make a PWG Semantic Model schema.
#
# Usage:
#
#     ./makeschema.sh schema-name service-name [version]
#
# Example:
#
#     ./makeschema.sh smpjt3d10 Print3D 1.0
#

if test $# -lt 2 -o $# -gt 3; then
	echo "Usage: ./makeschema.sh schema-name service-name [version]"
	exit 1
fi

schema="$1"
service="$2"

if test $# -gt 2; then
	version="$3"
	directory="$schema-$version"
else
	date="`date '+%Y%m%d'`"
	version="0.$date"
	directory="$schema-$date"
fi

if test ! -f ipp-registrations.xml; then
	echo Fetching IANA source registry...
	curl -o ipp-registrations.xml http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml
fi

echo Building regtosm tool, if necessary...
make regtosm

echo Creating schema...
test -d $directory && rm -rf $directory
mkdir $directory
cp services/$service.md $directory/README.md
./regtosm -a services/$service.txt -n http://www.pwg.org/schemas/$schema/$version -s $service -v $version ipp-registrations.xml $directory

echo Creating schema archive...
zip -rv9 $directory.zip $directory
