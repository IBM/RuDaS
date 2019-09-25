#  Copyright IBM Corp. 1992, 1993 All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

#!/bin/bash

DIR=`dirname $0`


rm -rf $DIR/../systems
mkdir $DIR/../systems


# NEURAL-LP
/bin/bash $DIR/neurallp.sh

# NTP
/bin/bash $DIR/ntp.sh

# MIT-IBM system
#/bin/bash $DIR/mitibm.sh

#Amiep
/bin/bash $DIR/amiep.sh


#FOIL
/bin/bash $DIR/foil.sh