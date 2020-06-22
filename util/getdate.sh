#!/usr/bin/env bash
#
# Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Generate build date information for the EC binary

echo "/* This file is generated by util/getdate.sh */"

echo "/* DATE is used to form build info string in common/version.c. */"
echo "#define DATE \"$(date '+%F %T')\""
