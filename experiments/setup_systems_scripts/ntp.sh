#  Copyright IBM Corp. 1992, 1993 All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

#!/bin/bash

DIR=`dirname $0`
SYSDIR=$DIR/../systems/ntp


if [ ! -d $SYSDIR ]; then
    pushd $DIR/../systems
    git clone https://github.com/uclmr/ntp.git
    popd
fi

if source activate ntp; then
	:
else
	echo 'Creating environment ntp'
	conda create -n ntp python=3.6 -y
	source activate ntp
fi

pip install -r $SYSDIR/requirements.txt
pip install -r $DIR/requirements_ntp2.txt