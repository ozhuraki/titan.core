###############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   >
#   Balasko, Jeno
#
###############################################################################
#!/bin/sh

set -xe

# set environment
. ./path.sh

# build titan from source on solaris
mkdir -p titan
cd titan
# .tar is exported with git archive -o t.tar
tar xf ../src/t.tar
cp ../Makefile.personal .
{ make || kill $$; } |tee make.log
{ make install || kill $$; } |tee make.install.log
# TODO: make release, cp doc/*.pdf

