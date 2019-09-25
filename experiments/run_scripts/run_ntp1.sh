#  Copyright IBM Corp. 1992, 1993 All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

#!/bin/bash

NAMES=(DRDG-XS-3/COMPLETE1 RDG-S-3/COMPLETE1)
TESTS=exp1

DIR=`dirname $0`
SYSDIR=$DIR/../systems
DATA=$DIR/../data/$TESTS/

for NAME in ${NAMES[@]}
do

SYSTEM=ntp
echo "Running ntp..."
source activate $SYSTEM
export PYTHONPATH=$PYTHONPATH:$SYSDIR/$SYSTEM
python $SYSDIR/$SYSTEM/ntp/experiments/learn.py $DATA/$SYSTEM/$NAME/run.conf > $DIR/../output/$TESTS/$SYSTEM/$NAME/log.txt 2> $DIR/../output/$TESTS/$SYSTEM/$NAME/err.txt
mv out/$NAME/* $DIR/../output/$TESTS/$SYSTEM/$NAME/
rm -r out/$NAME
echo "done."

done
exit 0

