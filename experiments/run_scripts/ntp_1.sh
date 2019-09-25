#  Copyright IBM Corp. 1992, 1993 All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

#!/bin/bash

NAMES=(DRDG-XS-2/COMPLETE1 RDG-S-3/COMPLETE1 DRDG-XS-2/INCOMPLETE1 RDG-S-3/INCOMPLETE1 DRDG-XS-2/INCOMPLETE_NOISE1 RDG-S-3/INCOMPLETE_NOISE1)
TESTS=exp1

DIR=`dirname $0`
SYSDIR=$DIR/../systems
DATA=$DIR/../data/$TESTS/

for NAME in ${NAMES[@]}
do

echo "Running ntp..."
rm -r out/$NAME
rm -r $DIR/../output/$TESTS/ntp/$NAME/*
source activate ntp
export PYTHONPATH=$PYTHONPATH:$SYSDIR/ntp
python $SYSDIR/ntp/ntp/experiments/learn.py $DATA/ntp/$NAME/run.conf > $DIR/../output/$TESTS/ntp/$NAME/log.txt 2> $DIR/../output/$TESTS/ntp/$NAME/err.txt
mv out/$NAME/* $DIR/../output/$TESTS/ntp/$NAME/
rm -r out/$NAME
echo "done."

done
exit 0

