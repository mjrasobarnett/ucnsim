#------------------------------------------------------------------------------
# GENERAL
#------------------------------------------------------------------------------
echo "------------------------------------------------------------------------"
echo "Setting up environment for UCN"
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
# GEANT4
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# ROOT
#------------------------------------------------------------------------------
export ROOTSYS=${SNO_ROOT}/ROOT/v5.18.00
export ROOTMACROS=${SNO_ROOT}/macros
export ROOTLOGON=${SNO_ROOT}/config/rootlogon.C
export ROOTLOGOFF=${SNO_ROOT}/config/rootlogoff.C
#------------------------------------------------------------------------------
# CERNLIB
#------------------------------------------------------------------------------
export CERN=${SOFT_ROOT}/CERNLIB
export CERN_LEVEL=2005
export CERN_ROOT=${CERN}/${CERN_LEVEL}
export CERNLIBPATH=${CERN_ROOT}/lib
#------------------------------------------------------------------------------
# GEANT4 (AND CLHEP)
#------------------------------------------------------------------------------
export G4INSTALL=${SOFT_ROOT}/GEANT4/4.8.1.p02
export G4SYSTEM=Darwin-g++
source ${G4INSTALL}/.config/bin/${G4SYSTEM}/env.sh > /dev/null
if [ ! -d "${G4INSTALL}/workdir" ]; then
mkdir ${G4INSTALL}/workdir
fi
export G4WORKDIR=${G4INSTALL}/workdir
#------------------------------------------------------------------------------
# PATHS
#------------------------------------------------------------------------------
export PATH=${CERN_ROOT}/bin:${G4WORKDIR}/bin:${ROOTSYS}/bin:${XSNOED_PATH}:{$PATH}
if [[ -n "${LD_LIBRARY_PATH}" ]] ; then
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}
else
export LD_LIBRARY_PATH=${ROOTSYS}/lib
fi
#------------------------------------------------------------------------------
echo "CERNLIB: " ${CERNLIBPATH}
echo "GEANT4 : " ${G4INSTALL}
echo "ROOT   : " ${ROOTSYS}
echo "------------------------------------------------------------------------"
#------------------------------------------------------------------------------
