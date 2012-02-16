#------------------------------------------------------------------------------
# GENERAL
#------------------------------------------------------------------------------
echo "------------------------------------------------------------------------"
echo "Setting up environment for UCNSIM"
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
# ROOT & UCN_DIR PATHS
#------------------------------------------------------------------------------
export PATH=${UCN_DIR}/build:${UCN_DIR}/scripts:${PATH}

if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
	export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib:${UCN_DIR}/lib:${LD_LIBRARY_PATH}
else
	export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib:${UCN_DIR}/lib:${ROOTSYS}/lib
fi

if [[ `uname` == Darwin ]] ; then
  if [[ -n "${DYLD_LIBRARY_PATH}" ]] ; then
   export DYLD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib:${UCN_DIR}/lib:${DYLD_LIBRARY_PATH}
  else
   export DYLD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib:${UCN_DIR}/lib:${ROOTSYS}/lib
  fi
fi

export ROOTMACROS=${UCN_DIR}/macros:${ROOTSYS}/macros
#export ROOTLOGON=${UCN_DIR}/config/ucn_rootlogon.C
#export ROOTLOGOFF=${UCN_DIR}/config/ucn_rootlogoff.C

#------------------------------------------------------------------------------
echo "ROOTSYS    : " $ROOTSYS
echo "GSL_DIR    : " $GSL_DIR
echo "BOOST_ROOT : " $BOOST_ROOT
echo "------------------------------------------------------------------------"
echo "UCN_DIR  : " $UCN_DIR
echo "UCN_GEOM : " $UCN_GEOM
echo "UCN_RUNS : " $UCN_RUNS
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
