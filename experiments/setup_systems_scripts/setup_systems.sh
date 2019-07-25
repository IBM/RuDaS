#!/bin/bash

DIR=`dirname $0`

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