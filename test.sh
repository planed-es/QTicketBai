#!/bin/sh -e
# usage: ./test.sh [test-certificate-name]

CERTIFICATE_ARCHIVE_VERSION="1_0_2"
CERTIFICATE_ARCHIVE="batuz_lroe_ziurtagiriak_kit_certificados_v$CERTIFICATE_ARCHIVE_VERSION"
CERTIFICATE_FOLDER="Batuz_LROE_Ziurtagiriak_KIT_Certificados_V$CERTIFICATE_ARCHIVE_VERSION"
EXAMPLE_ARCHIVE="Ejemplos"
TESTDIR=ticketbai-test-env

##
## Download testing resources
##
mkdir -p "$TESTDIR"
cd "$TESTDIR"

echo "+ Downloading LROE testing resources"

if ! [ -d "$CERTIFICATE_FOLDER" ] ; then
  curl -sSO "https://www.batuz.eus/fitxategiak/batuz/lroe/$CERTIFICATE_ARCHIVE.7z"
  7z x $CERTIFICATE_ARCHIVE.7z
  rm   $CERTIFICATE_ARCHIVE.7z
else
  echo "- certificates are already present. skipping download."
fi

if ! [ -f "Ejemplo_TicketBAI_B00000034_B2022_0101.xml" ] ; then
  curl -sSO "https://www.batuz.eus/fitxategiak/Batuz/LROE/ejemplos/$EXAMPLE_ARCHIVE.7z"
  7z x $EXAMPLE_ARCHIVE.7z
  rm   $EXAMPLE_ARCHIVE.7z
else
  echo "- examples are already present. skipping download."
fi

##
## Prepare environment variables
##

echo "+ Preparing testing environment"

CERTIFICATE_NAME="$1"
if [ -z "$CERTIFICATE_NAME" ] ; then
  CERTIFICATE_NAME="EnpresaZigilua_SelloDeEmpresa"
fi

export LROE_ENVIRONMENT="test"
export TBAI_CERTIFICATE_PATH="$PWD/$CERTIFICATE_FOLDER/$CERTIFICATE_NAME.p12"
export TBAI_CERTIFICATE_PASSWORD=`cat "$PWD/$CERTIFICATE_FOLDER/${CERTIFICATE_NAME}_pin.txt"`
export TBAI_EXAMPLES_PATH="$PWD"

if ! [ -f "$TBAI_CERTIFICATE_PATH" ] ; then
  echo "Certificate file not found: " $TBAI_CERTIFICATE_PATH
  exit -1
fi

if [ -z "$TBAI_CERTIFICATE_PASSWORD" ] ; then
  echo "Certificate password is empty."
  exit -1
fi

##
## Run tests
##
cd -
echo "+ Running the tests"

tests/lroesubmitprocess

#rm -rf "$TESTDIR"
