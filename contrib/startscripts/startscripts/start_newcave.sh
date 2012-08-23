#!/bin/bash

# script to configure and start an application on the new cave at RWTH Aachen University
# Modify this file to set options specific to the system, e.g. anti-aliasing and 
# system-dependent command line parameters like ViSTA's display-ini

# set up antialiasing
#    __GL_FSAA_MODE     GeForce FX, GeForce 6xxx, GeForce 7xxx, Quadro FX
#    ---------------    ------------------------------------------------------
#    0                  FSAA disabled
#    1                  2x Bilinear Multisampling
#    2                  2x Quincunx Multisampling
#    3                  FSAA disabled
#    4                  4x Bilinear Multisampling
#    5                  4x Gaussian Multisampling
#    6                  2x Bilinear Multisampling by 4x Supersampling
#    7                  4x Bilinear Multisampling by 4x Supersampling
#    8                  4x Bilinear Multisampling by 2x Supersampling
#                       (available on GeForce FX and later GPUs; not
#                       available on Quadro GPUs)
export __GL_FSAA_MODE=8
# set anisotropic filtering
export __GL_DEFAULT_LOG_ANISO=3
# disable vsync here - state per client will be set by display_newcave.ini
export __GL_SYNC_TO_VBLANK=0

./startscripts/start_Inside.sh $@ -vista inside_newcave.ini -displayini display_newcave.ini -clusterini display_newcave.ini
