#------------------------------------------------------------------------------
# GENERAL
#------------------------------------------------------------------------------
echo "------------------------------------------------------------------------"
echo "Setting up environment for UCNSIM"
echo "------------------------------------------------------------------------"
export SOFT_ROOT=/Users/sjp39/Software
export UCN_ROOT=${SOFT_ROOT}/ucnsim/trunk
export UCN_GEOM=${UCN_ROOT}/geom
#------------------------------------------------------------------------------
# ROOT
#------------------------------------------------------------------------------
export ROOTSYS=${SOFT_ROOT}/ROOT/v5.24.00
export ROOTMACROS=${UCN_ROOT}/macros:${SOFT_ROOT}/macros
export ROOTLOGON=${SOFT_ROOT}/config/ucn_rootlogon.C
export ROOTLOGOFF=${SOFT_ROOT}/config/ucn_rootlogoff.C
#------------------------------------------------------------------------------
# PATHS
#------------------------------------------------------------------------------
export PATH=${UCN_ROOT}/bin:$ROOTSYS/bin:$PATH
if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${UCN_ROOT}/lib:${LD_LIBRARY_PATH}
else
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${UCN_ROOT}/lib
fi
if [[ -n "${DYLD_LIBRARY_PATH}" ]] ; then
export DYLD_LIBRARY_PATH=${ROOTSYS}/lib:${UCN_ROOT}/lib:${DYLD_LIBRARY_PATH}
else
export DYLD_LIBRARY_PATH=${ROOTSYS}/lib:${UCN_ROOT}/lib
fi
#------------------------------------------------------------------------------
echo "UCN   : " $UCN_ROOT
echo "ROOT  : " $ROOTSYS
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
