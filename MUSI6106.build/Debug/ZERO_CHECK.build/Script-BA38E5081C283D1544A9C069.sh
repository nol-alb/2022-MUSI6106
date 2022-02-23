#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/noelalben/github/2022-MUSI6106
  make -f /Users/noelalben/github/2022-MUSI6106/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/noelalben/github/2022-MUSI6106
  make -f /Users/noelalben/github/2022-MUSI6106/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/noelalben/github/2022-MUSI6106
  make -f /Users/noelalben/github/2022-MUSI6106/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/noelalben/github/2022-MUSI6106
  make -f /Users/noelalben/github/2022-MUSI6106/CMakeScripts/ReRunCMake.make
fi

