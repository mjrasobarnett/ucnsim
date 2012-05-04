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
# -- Fetch arguments from command line
# -- $# stores the number of comman line arguments passed to shell script
# -- Arguments accessed by $1, $2, $3...
if [ "$#" -ne 2 ] ; then
   echo "Incorrect number of command line arguments: $@ passed to submit_job.sh" 1>&2
   exit 1
else
   export CONFIG_FILE="$1"
   export RUN_NUM="$2"
fi
#
# -- Setup the required environment variables on machine
export HOME=/home/m/mb/mb325
echo "Configuring for Feynman remote machine"
export UCN_DIR=${HOME}/projects/cryoedm/ucnsim
export UCN_GEOM=${UCN_DIR}/geom
export SCRATCH=/epp/scratch/nedm/mb325/
export UCN_RUNS=${SCRATCH}/runs
export ROOTSYS=${SCRATCH}/ROOT/root
export GSL=${HOME}/gsl/gsl
export BOOST_ROOT=${HOME}/packages/boost/boost
export PATH=${UCN_DIR}/build:${ROOTSYS}/bin:${PATH}
export LD_LIBRARY_PATH=${UCN_DIR}/build:${ROOTSYS}/lib/root:${LD_LIBRARY_PATH}
export ROOTMACROS=${UCN_DIR}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCN_DIR}/config/ucn_rootalias.C
export ROOTLOGON=${UCN_DIR}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCN_DIR}/config/ucn_rootlogoff.C
echo "------------------------------------------------------------------------"
echo "ROOT  : $ROOTSYS"
echo "GSL   : $GSL"
echo "BOOST : $BOOST_ROOT"
echo "PATH  : $PATH"
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
