#!/bin/bash

DIR=`dirname $0`
SYSDIR=$DIR/../systems/Neural-LP


if [ ! -d $SYSDIR ]; then
    pushd $DIR/../systems
    git clone https://github.com/fanyangxyz/Neural-LP.git
    popd
fi

if source activate Neural-LP; then
	:
else
	echo 'Creating environment Neural-LP'
	conda create -n Neural-LP python=2.7 -y
	source activate Neural-LP
fi
pip install -r $DIR/requirements_neurallp.txt