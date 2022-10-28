#!/bin/bash

AUTOFIRMA_MAJOR_VERSION="1"
AUTOFIRMA_MINOR_VERSION="7"
AUTOFIRMA_PATCH_VERSION="1"
AUTOFIRMA_VERSION="$AUTOFIRMA_MAJOR_VERSION"_"$AUTOFIRMA_MINOR_VERSION"_"$AUTOFIRMA_PATCH_VERSION"
AUTOFIRMA_JAR_PATH="/usr/lib/AutoFirma/AutoFirma.jar"
OUTPUT_PATH=$1
TMP_DIR="/tmp/autofirma"

usage="$(basename "$0") [filepath] -- downloads AutoFirma program v$AUTOFIRMA_MAJOR_VERSION to the given filepath"

[ "$1" = "-h" ] && echo "$usage" && exit
[ -z "$1" ]  && echo "$usage" && exit -1

rm -rf $TMP_DIR
mkdir $TMP_DIR
cd $TMP_DIR
wget "https://estaticos.redsara.es/comunes/autofirma/$AUTOFIRMA_MAJOR_VERSION/$AUTOFIRMA_MINOR_VERSION/$AUTOFIRMA_PATCH_VERSION/AutoFirma_Linux.zip"
unzip AutoFirma_Linux.zip
ar x AutoFirma_$AUTOFIRMA_VERSION.deb
tar xf data.tar.xz

mkdir -p "$(basename "$OUTPUT_PATH")"
cp $TMP_DIR$AUTOFIRMA_JAR_PATH $OUTPUT_PATH
rm -rf $TMP_DIR
