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
	export PATH=${UCN_DIR}/bin:${PATH}
	# Define path to ROOT installation
	if [[ -z "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/Apps/ROOT/root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
elif [[ `uname -n` == "simon-mbp.pact.cpes.susx.ac.uk" ]] ; then # Need Simon's local hostname
	# Test if we are running on simon's machine
	echo "Configuring for Simon's local machine"
	export UCN_DIR=${HOME}/Software/ucnsim # Simon to fill in here where he stores the code	
	export UCN_GEOM=${UCN_DIR}/geom
	export PATH=${UCN_DIR}/bin:${PATH}
	if [[ -z "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}Software/ROOT/v5.24.00 # Simon to fill in where he keeps his root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
elif [[ `uname -n` == "neutrino.pact.cpes.susx.ac.uk" ]] ; then 
	# Test if we are running on neutrino
	echo "Configuring for neutrino remote machine"
	export UCN_DIR=/export/eppcode/ucnsim
	export UCN_GEOM=${UCN_DIR}/geom
	export PATH=${UCN_DIR}/bin:${PATH}
	export default="default"
   if [[ "${ROOTSYS:-default}" == "default" ]] ; then
		export ROOTSYS=/export/eppcode/ROOT/v5.24.00b
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
	# Add more machines here as necessary
elif [[ `uname -n` == "ubuntu" ]] ; then 
	# Test if we are running on matt's linux virtual machine
	echo "Configuring for Matt's linux virtual machine"
	export UCN_DIR=${HOME}/code/git/ucnsim
	export UCN_GEOM=${UCN_DIR}/geom
	export PATH=${UCN_DIR}/bin:${PATH}
	if [[ -z "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/code/root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
fi

#------------------------------------------------------------------------------
# ROOT & UCN_DIR PATHS
#------------------------------------------------------------------------------
if [[ -z "${LD_LIBRARY_PATH}" ]] ; then
	export LD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib:${LD_LIBRARY_PATH}
else
	export LD_LIBRARY_PATH=${UCN_DIR}/lib:${ROOTSYS}/lib
fi

if [[ `uname` == Darwin ]] ; then
  if [[ -z "${DYLD_LIBRARY_PATH}" ]] ; then
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
echo "UCN   : " $UCN_DIR
echo "ROOT  : " $ROOTSYS
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
