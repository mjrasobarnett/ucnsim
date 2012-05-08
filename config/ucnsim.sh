#------------------------------------------------------------------------------
# Add build and scripts dir to $PATH and set custom ROOTLOGON macros 
#------------------------------------------------------------------------------
export PATH=${UCN_DIR}/build:${UCN_DIR}/scripts:${PATH}
export ROOTMACROS=${UCN_DIR}/macros:${ROOTSYS}/macros
export ROOTLOGON=${UCN_DIR}/config/ucn_rootlogon.C
export ROOTLOGOFF=${UCN_DIR}/config/ucn_rootlogoff.C
export ROOTALIAS=${UCN_DIR}/config/ucn_rootalias.C
export LD_LIBRARY_PATH=${UCN_DIR}/lib:${LD_LIBRARY_PATH}
if [[ `uname` == Darwin ]] ; then
    export DYLD_LIBRARY_PATH=${UCN_DIR}/lib:${DYLD_LIBRARY_PATH}
fi
#------------------------------------------------------------------------------
if [[ -n $PS1 ]]
then
    echo "------------------------------------------------------------------------"
    echo "Setting up environment for UCNSIM"
    echo "------------------------------------------------------------------------"
    echo "ROOTSYS    : " $ROOTSYS
    echo "GSL_DIR    : " $GSL_DIR
    echo "BOOST_ROOT : " $BOOST_ROOT
    echo "------------------------------------------------------------------------"
    echo "UCN_DIR  : " $UCN_DIR
    echo "UCN_GEOM : " $UCN_GEOM
    echo "UCN_RUNS : " $UCN_RUNS
    echo "------------------------------------------------------------------------"
fi
#------------------------------------------------------------------------------

