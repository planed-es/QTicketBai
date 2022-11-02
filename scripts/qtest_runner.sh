#!/bin/sh -e

echo "" > .test-log
for test_bin_dir in "$@" ; do
  for test in `find "$1" -maxdepth 1 -type f -executable` ; do
    $test >> .test-log
  done
done

     passed=`cat .test-log | grep -e "^Totals" | cut -d' ' -f 2 | awk '{s+=$1} END {print s}'`
     failed=`cat .test-log | grep -e "^Totals" | cut -d' ' -f 4 | awk '{s+=$1} END {print s}'`
    skipped=`cat .test-log | grep -e "^Totals" | cut -d' ' -f 6 | awk '{s+=$1} END {print s}'`
blacklisted=`cat .test-log | grep -e "^Totals" | cut -d' ' -f 8 | awk '{s+=$1} END {print s}'`

line_render="/^PASS/{print \"\033[32m\",\$0,\"\033[0m\"} /^(FAIL|\s)/{print \"\033[31m\",\$0,\"\033[0m\"}"

if [[ $1 == "debug" ]] ; then
  cat .test-log | awk "$line_render /^QDEBUG/{print \$0}"
else
  cat .test-log | awk "$line_render"
fi

echo

/usr/bin/echo -e \
  "Totals: ${GREEN}$passed passed${NC}, ${RED}$failed failed${NC}, \033${ORANGE}$skipped skipped${NC}, $blacklisted blacklisted."

if [[ $failed > 0 ]] ; then
  exit -1
fi
