#!/bin/bash
# -- QSUB options
# When a single command in the array job is sent to a compute node,
# its task number is stored in the variable SGE_TASK_ID, so we can
# use the value of that variable to create subdirs or log names
# that depend on the jobID number
#
#$ -cwd
#$ -o /epp/scratch/nedm/mb325/logs
#$ -e /epp/scratch/nedm/mb325/logs
#$ -S /bin/bash
#$ -q short.q

#
# -- Before running anything, ensure that we are on Feynman
if [[ "uname -n" != "feynman" ]] ; then
   echo "Not on Feynman system!"
   exit 1
fi
#
# -- Fetch arguments from command line
# -- $# stores the number of comman line arguments passed to shell script
# -- Arguments accessed by $1, $2, $3...
if [ "$#" -ne 2 ] ; then
   echo "Incorrect number of command line arguments: $@ passed to submit_job.sh"
   exit 1
else
   export CONFIG_FILE="$1"
   export RUN_NUM="$2"
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
export LD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib/root:${LD_LIBRARY_PATH}
export ROOTMACROS=${UCN_DIR}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCN_DIR}/config/ucn_rootalias.C
export ROOTLOGON=${UCN_DIR}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCN_DIR}/config/ucn_rootlogoff.C
echo "ROOT  : $ROOTSYS"
echo "GSL   : $GSL"
echo "BOOST : $BOOST"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "------------------------------------------------------------------------"
echo "UCN_DIR  : $UCN_DIR"
echo "UCN_GEOM : $UCN_GEOM"
echo "UCN_RUNS : $UCN_RUNS"
echo "------------------------------------------------------------------------"
#
# -- Start Job
echo "Simulating Run No: $RUN_NUM"
simulate_ucn $CONFIG_FILE $RUN_NUM
#
echo 'Job Ended Here!'
exit 0
