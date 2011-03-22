#!/bin/bash
# Script to copy over latest run data from scratch to code base on Feynman
#########################################
# Written by: Matt Raso-Barnett
#########################################
#########################################
# Configuration section
#########################################
####
# Remote directory; What we are copying to
R_DIRECTORY="$UCN_DIR/runs"
####
# Local directory; Where we will copy from
L_DIRECTORY="$UCN_RUNS"
#########################################
# Program section
#########################################

####
# Rsync; Rsync synchronises two directories. First argument is the source
# directory and the second is the target directory
# -a ; Archive mode, I think recursively traverses the directory
# -z ; Compress file data during transfer
echo "-----------------------------"
echo "Copying from  $L_DIRECTORY to  $R_DIRECTORY"
echo "Checking Source Directory: $L_DIRECTORY" 
rsync -avz "$L_DIRECTORY" "$R_DIRECTORY"
EXIT_V="$?"
case $EXIT_V in
	0) O="Success";;
	1) O="Error";;
esac
echo "-----------------------------"
echo "rsync result = $O"
echo "Date $(date)"
echo "-----------------------------"
####

