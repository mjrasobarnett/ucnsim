#------------------------------------------------------------------------------
# GENERAL
#------------------------------------------------------------------------------
echo "------------------------------------------------------------------------"
echo "Setting up environment for UCNSIM"
echo "------------------------------------------------------------------------"

if [[ `uname -n` == "MJRasoBarnett.local" ]] ; then
	# First test if we are running on matt's machine
	echo "Configuring for Matt's local machine"
	export UCN_DIR=${HOME}/Projects/ucnsim/source
	export UCN_GEOM=${UCN_DIR}/geom
	export UCN_RUNS=${UCN_DIR}/runs
	export PATH=${UCN_DIR}/build:${PATH}
	# Define path to ROOT installation
	if [[ -z "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/Apps/ROOT/root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
	# Define path to GSL installation
	export GSL=${HOME}/Apps/GSL/gsl
	# Define path to BOOST installation
	export BOOST=${HOME}/Apps/Boost/boost
elif [[ `uname -n` == "feynman" ]] ; then 
	# Test if we are running on matt's linux virtual machine
	echo "Configuring for Feynman remote machine"
	export UCN_DIR=${HOME}/projects/ucnsim
	export UCN_GEOM=${UCN_DIR}/geom
	export SCRATCH=/epp/scratch/nedm/mb325
	export UCN_RUNS=${SCRATCH}/runs
	export PATH=${UCN_DIR}/bin:${UCN_DIR}/scripts:${PATH}
	if [[ -z "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/root/root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
	# Define path to GSL installation
	export GSL=${HOME}/gsl/gsl
	# Define path to BOOST installation
	export BOOST=${HOME}/boost/boost
fi
#------------------------------------------------------------------------------
# ROOT & UCN_DIR PATHS
#------------------------------------------------------------------------------
if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
	export LD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib:${LD_LIBRARY_PATH}
else
	export LD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib
fi

if [[ `uname` == Darwin ]] ; then
  if [[ -n "${DYLD_LIBRARY_PATH}" ]] ; then
   export DYLD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib:${DYLD_LIBRARY_PATH}
  else
   export DYLD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib
  fi
fi

export ROOTMACROS=${UCN_DIR}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCN_DIR}/config/ucn_rootalias.C
export ROOTLOGON=${UCN_DIR}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCN_DIR}/config/ucn_rootlogoff.C

#------------------------------------------------------------------------------
echo "ROOT  : " $ROOTSYS
echo "GSL   : " $GSL
echo "BOOST : " $BOOST
echo "------------------------------------------------------------------------"
echo "UCN_DIR  : " $UCN_DIR
echo "UCN_GEOM : " $UCN_GEOM
echo "UCN_RUNS : " $UCN_RUNS
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
