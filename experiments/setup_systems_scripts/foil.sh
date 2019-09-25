#  Copyright IBM Corp. 1992, 1993 All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

#!/bin/bash

DIR=`dirname $0`
SYSDIR=$DIR/../systems/FOIL

rm -rf $DIR/../systems/FOIL
mkdir $DIR/../systems/FOIL


wget -O $DIR/../systems/FOIL/foil6.sh "https://www.rulequest.com/Personal/foil6.sh"

cd $DIR/../systems/FOIL/
sh foil6.sh
make
cd ../../setup_systems_scripts/
