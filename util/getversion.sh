#!/usr/bin/env bash
#
# Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Generate version information for the EC binary

if ghash=`git rev-parse --short --verify HEAD 2>/dev/null`; then
	if gdesc=`git describe --dirty --match='v*' 2>/dev/null`; then
		IFS="-" fields=($gdesc)
		tag="${fields[0]}"
		IFS="." vernum=($tag)
		numcommits=$((${vernum[2]}+${fields[1]:-0}))
		ver_major="${vernum[0]}"
		ver_branch="${vernum[1]}"
	else
		numcommits=`git rev-list HEAD | wc -l`
		ver_major="v0"
		ver_branch="0"
	fi
	# avoid putting the -dirty attribute if only the timestamp
	# changed
	git status > /dev/null 2>&1

	dirty=`sh -c "[ '$(git diff-index --name-only HEAD)' ] && echo '-dirty'"`
	vbase="${ver_major}.${ver_branch}.${numcommits}-${ghash}${dirty}"
else
	vbase="no_version"
fi

ver="${BOARD}_${vbase}"

echo "/* This file is generated by util/getversion.sh */"

echo "/* Version string for use by common/version.c */"
echo "#ifdef SHIFT_CODE_FOR_TEST"
echo "#define CROS_EC_VERSION \"${ver}_shift\""
echo "#else"
echo "#define CROS_EC_VERSION \"${ver}\""
echo "#endif"

echo "/* Version string, truncated to 31 chars (+ terminating null = 32) */"
echo "#define CROS_EC_VERSION32 \"${ver:0:31}\""

echo "/* Sub-fields for use in Makefile.rules and to form build info string"
echo " * in common/version.c. */"
echo "#define VERSION \"${ver}\""
echo "#define BUILDER \"Mirrexagon\""
