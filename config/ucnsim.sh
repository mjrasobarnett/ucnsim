#------------------------------------------------------------------------------
# GENERAL
#------------------------------------------------------------------------------
echo "------------------------------------------------------------------------"
echo "Setting up environment for UCNSIM"
echo "------------------------------------------------------------------------"
export UCNSIM=${HOME}/Projects/git/ucnsim
export UCN_GEOM=${UCNSIM}/geom
#------------------------------------------------------------------------------
# ROOT
#------------------------------------------------------------------------------
export ROOTMACROS=${UCNSIM}/macros:${ROOTSYS}/macros
export ROOTALIAS=${UCNSIM}/config/ucn_rootalias.C
export ROOTLOGON=${UCNSIM}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCNSIM}/config/ucn_rootlogoff.C
#------------------------------------------------------------------------------
# PATHS
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
