#!/bin/bash
# -- Make sure that this script is exited if we try to use any uninitialised variable
set -u
#
# -- Fetch arguments from command line
# -- $# stores the number of comman line arguments passed to shell script
# -- Arguments accessed by $1, $2, $3...
if [ "$#" -ne 2 ] ; then
   echo "Incorrect number of command line arguments: $@ passed to submit_job.sh"
   exit 1
else
   export JOB_DIR="$1"
   export RUN_NUM="$2"
   export JOB_NAME="run$RUN_NUM"
fi
#
# -- Setup the required environment variables on machine
export HOME=/home/mb325
echo "Configuring for Feynman remote machine"
export UCN_DIR=${HOME}/projects/ucnsim
export UCN_GEOM=${UCN_DIR}/geom
export UCN_RUNS=/epp/scratch/nedm/mb325/ucnsim/runs
export ROOTSYS=${HOME}/root/root
export PATH=${UCN_DIR}/bin:${ROOTSYS}/bin:${PATH}
export GSL=${HOME}/gsl/gsl
export BOOST=${HOME}/boost/boost
export LD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib:${LD_LIBRARY_PATH}
export ROOTMACROS=${UCN_DIR}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCN_DIR}/config/ucn_rootalias.C
export ROOTLOGON=${UCN_DIR}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCN_DIR}/config/ucn_rootlogoff.C
echo "ROOT  : $ROOTSYS"
echo "GSL   : $GSL"
echo "BOOST : $BOOST"
echo "------------------------------------------------------------------------"
echo "UCN_DIR  : $UCN_DIR"
echo "UCN_GEOM : $UCN_GEOM"
echo "UCN_RUNS : $UCN_RUNS"
echo "------------------------------------------------------------------------"
#
# -- cd to workarea
echo "Work Area: $JOB_DIR"
cd $JOB_DIR
# Check that move to work area was successful
if [ "$PWD" -ne "$JOB_DIR" ] ; then
  echo "Can't change to $JOB_DIR."
  exit 1
fi
#
# -- QSUB options
# When a single command in the array job is sent to a compute node,
# its task number is stored in the variable SGE_TASK_ID, so we can
# use the value of that variable to create subdirs or log names
# that depend on the jobID number
#
# set your email address if you want notification
-M rasobarnettmatthew@gmail.com
# set the next line to the job title (optional)
-N $JOB_NAME
# -- Redirect standard output to scratch
-o $JOB_DIR
-e $JOB_DIR
# -- Redirect standard err to scratch
# -- specify queue (all.q is the default and needn't be specified)
-q short.q
#
# -- Execute Job
echo "Simulating Run No: $RUN_NUM"
simulate_ucn config.cfg $RUN_NUM
#
echo 'Job Ended Here!'
exit 0