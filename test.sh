#!/bin/sh -e
# usage: ./test.sh [test-certificate-name]

CERTIFICATE_ARCHIVE_VERSION="1_0_2"
CERTIFICATE_ARCHIVE="batuz_lroe_ziurtagiriak_kit_certificados_v$CERTIFICATE_ARCHIVE_VERSION"
CERTIFICATE_FOLDER="Batuz_LROE_Ziurtagiriak_KIT_Certificados_V$CERTIFICATE_ARCHIVE_VERSION"
EXAMPLE_ARCHIVE="Ejemplos"
TESTDIR="$PWD/.ticketbai-test-env"
export LD_LIBRARY_PATH=/usr/local/lib

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

cd -

##
## Prepare environment variables
##

echo "+ Preparing testing environment"

CERTIFICATE_NAME="$1"
if [ -z "$CERTIFICATE_NAME" ] ; then
  CERTIFICATE_NAME="EnpresaZigilua_SelloDeEmpresa"
fi

export LROE_ENVIRONMENT="test"
export TBAI_CERTIFICATE_PATH="$TESTDIR/$CERTIFICATE_FOLDER/$CERTIFICATE_NAME.p12"
export TBAI_CERTIFICATE_PASSWORD=`cat "$TESTDIR/$CERTIFICATE_FOLDER/${CERTIFICATE_NAME}_pin.txt"`
export TBAI_CERTIFICATE_ALIAS=`scripts/alias_from_certificate.sh`
export TBAI_EXAMPLES_PATH="$TESTDIR"
export TBAI_LICENSE="TBAIBI00000000PRUEBA"
export TBAI_SOFTWARE_NIF="A99800005"
export TBAI_SOFTWARE_NAME="SOFTWARE GARANTE TICKETBAI PRUEBA"

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
echo "+ Running the tests"

if ! scripts/qtest_runner.sh build/tests ; then
  exit -1
fi

#rm -rf "$TESTDIR"
