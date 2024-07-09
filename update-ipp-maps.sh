#!/bin/sh
#
# Script to build IPP attribute mapping pages.
#
# Usage:
#
#   ./update-ipp-maps.sh [--jekyll] DIRECTORY
#

# Parse command-line...
if test "x$1" = x--jekyll; then
    option="$1"
    shift
else
    option=""
fi

if test $# != 1; then
    echo "Usage: ./update-ipp-maps.sh [--jekyll] DIRECTORY"
    exit 1
fi

outdir="$1"

# Get IANA registry files...
echo "Fetching IANA source registry..."
src="iana-ipp-registrations.xml"
if test -f $src; then
    curl -z $src -o $src http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml
else
    curl -o $src http://www.iana.org/assignments/ipp-registrations/ipp-registrations.xml
fi

# Write mapping tables for each service type...
echo "Generating mapping tables..."
./regtomap $option $src >"$outdir/print.html"
./regtomap $option --print3d $src >"$outdir/print3d.html"
./regtomap $option --faxout $src >"$outdir/faxout.html"
./regtomap $option --scan $src >"$outdir/scan.html"
