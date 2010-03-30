#------------------------------------------------------------------------------
# GENERAL
#------------------------------------------------------------------------------
echo "------------------------------------------------------------------------"
echo "Setting up environment for UCNSIM"
echo "------------------------------------------------------------------------"

if [[ `uname -n` == "MJRasoBarnett.local" ]] ; then
	# First test if we are running on matt's machine
	echo "Configuring for Matt's local machine"
	export UCNSIM=${HOME}/Projects/git/ucnsim
	export UCN_GEOM=${UCNSIM}/geom
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/Apps/ROOT/root
		export PATH=${UCNSIM}/bin:${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
elif [[ `uname -n` == "sjp39.local" ]] ; then # Need Simon's local hostname
	# Test if we are running on simon's machine
	echo "Configuring for Simon's local machine"
	export UCNSIM=${HOME}/Software/ucnsim/trunk # Simon to fill in here where he stores the code	
	export UCN_GEOM=${UCNSIM}/geom
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}Software/ROOT/v5.24.00 # Simon to fill in where he keeps his root
		export PATH=${UCNSIM}/bin:${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
elif [[ `uname -n` == "neutrino.pact.cpes.susx.ac.uk" ]] ; then 
	# Test if we are running on neutrino
	echo "Configuring for neutrino remote machine"
	export UCNSIM=/export/eppcode/ucnsim
	export UCN_GEOM=${UCNSIM}/geom
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=/export/eppcode/matt/root/root
		export PATH=${UCNSIM}/bin:${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
	# Add more machines here as necessary
elif [[ `uname -n` == "ubuntu" ]] ; then 
	# Test if we are running on matt's linux virtual machine
	echo "Configuring for Matt's linux virtual machine"
	export UCNSIM=${HOME}/code/git/ucnsim
	export UCN_GEOM=${UCNSIM}/geom
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/code/root
		export PATH=${UCNSIM}/bin:${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
fi

#------------------------------------------------------------------------------
# ROOT & UCNSIM PATHS
#------------------------------------------------------------------------------
if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
	export LD_LIBRARY_PATH=${UCNSIM}/lib:${ROOTSYS}/lib:${LD_LIBRARY_PATH}
else
	export LD_LIBRARY_PATH=${UCNSIM}/lib:${ROOTSYS}/lib:
fi

if [[ `uname` == Darwin ]] ; then
  if [[ -n "${DYLD_LIBRARY_PATH}" ]] ; then
   export DYLD_LIBRARY_PATH=${UCNSIM}/lib:${ROOTSYS}/lib:${DYLD_LIBRARY_PATH}
  else
   export DYLD_LIBRARY_PATH=${UCNSIM}/lib:${ROOTSYS}/lib:
  fi
fi

export ROOTMACROS=${UCNSIM}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCNSIM}/config/ucn_rootalias.C
export ROOTLOGON=${UCNSIM}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCNSIM}/config/ucn_rootlogoff.C

#------------------------------------------------------------------------------
echo "UCN   : " $UCNSIM
echo "ROOT  : " $ROOTSYS
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
