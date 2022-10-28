#!/bin/sh -ex

keytool -v -list \
  -storetype pkcs12 \
  -storepass "$TBAI_CERTIFICATE_PASSWORD" \
  -keystore "$TBAI_CERTIFICATE_PATH" \
  | grep -e '[aA]lias' \
  | cut -d: -f2 \
  | sed 's/^ *//g'
