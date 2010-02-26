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
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=${HOME}/Apps/ROOT/root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi	
elif [[ `uname -n` == "sjp39.local" ]] ; then # Need Simon's local hostname
	# Test if we are running on simon's machine
	echo "Configuring for Simon's local machine"
	export UCNSIM=${HOME} # Simon to fill in here where he stores the code	
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=/usr/local/bin # Simon to fill in where he keeps his root
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi	
elif [[ `uname -n` == "neutrino.pact.cpes.susx.ac.uk" ]] ; then 
	# Test if we are running on neutrino
	echo "Configuring for neutrino remote machine"
	export UCNSIM=/export/eppcode/ucnsim
	if [[ -n "${ROOTSYS}" ]] ; then
		export ROOTSYS=/export/eppcode/root_v5.22.00
		export PATH=${ROOTSYS}/bin:${PATH}
		export MANPATH=${ROOTSYS}/man:${MANPATH}
	fi
	# Add more machines here as necessary
fi	

export UCN_GEOM=${UCNSIM}/geom

#------------------------------------------------------------------------------
# ROOT
#------------------------------------------------------------------------------
if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ROOTSYS}/lib
else
	export LD_LIBRARY_PATH=${ROOTSYS}/lib
fi

if [[ `uname` == Darwin ]] ; then
  if [[ -n "${DYLD_LIBRARY_PATH}" ]] ; then
   export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${ROOTSYS}/lib
  else
   export DYLD_LIBRARY_PATH=${ROOTSYS}/lib
  fi
fi
export ROOTMACROS=${UCNSIM}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCNSIM}/config/ucn_rootalias.C
export ROOTLOGON=${UCNSIM}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCNSIM}/config/ucn_rootlogoff.C

#------------------------------------------------------------------------------
# UCNSIM PATHS
#------------------------------------------------------------------------------
export PATH=${UCNSIM}/bin:$PATH
if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
export LD_LIBRARY_PATH=${UCNSIM}/lib:${LD_LIBRARY_PATH}
else
export LD_LIBRARY_PATH=${UCNSIM}/lib
fi
if [[ -n "${DYLD_LIBRARY_PATH}" ]] ; then
export DYLD_LIBRARY_PATH=${UCNSIM}/lib:${DYLD_LIBRARY_PATH}
else
export DYLD_LIBRARY_PATH=${UCNSIM}/lib
fi
#------------------------------------------------------------------------------
echo "UCN   : " $UCNSIM
echo "ROOT  : " $ROOTSYS
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
