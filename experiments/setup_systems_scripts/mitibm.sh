#  Copyright IBM Corp. 1992, 1993 All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

#!/bin/bash

DIR=`dirname $0`
SYSDIR=$DIR/../systems/Neural-LP


if [ ! -d $SYSDIR ]; then
    pushd $DIR/../systems
    git clone git@git.ng.bluemix.net:MIT-IBM-Watson-AI-Lab/AI-Algorithms/inducing-probabilistic-programs/neural-theorem-proving.git
    popd
fi

if source activate neural-theorem-proving; then
	:
else
	echo 'Creating environment neural-theorem-proving'
	conda create -n neural-theorem-proving python=3.7 -y
	source activate neural-theorem-proving
fi
pip install -r $DIR/../systems/neural-theorem-proving/requirements.txt