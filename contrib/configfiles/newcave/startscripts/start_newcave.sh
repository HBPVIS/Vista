#!/bin/bash
# generic script to start the application

source ./set_path_for_YOURAPPNAME.sh

./YOURAPPNAME $@ -vistaini vista_newcave.ini -displayini display_newcave.ini -clusterini display_newcave.ini
