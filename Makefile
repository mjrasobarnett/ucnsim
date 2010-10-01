# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

include ./Makefile.arch
#------------------------------------------------------------------------------
# standalone programs
#------------------------------------------------------------------------------
UCNSIMS			= src/ucnsim.$(SrcSuf)
UCNSIMO			= src/ucnsim.$(ObjSuf)
UCNSIM			= bin/ucnsim$(ExeSuf)
SANDBOXS			= src/sandbox.$(SrcSuf)
SANDBOXO			= src/sandbox.$(ObjSuf)
SANDBOX			= bin/sandbox$(ExeSuf)
BUILDTESTS		= src/buildtest.$(SrcSuf)
BUILDTESTO		= src/buildtest.$(ObjSuf)
BUILDTEST		= bin/buildtest$(ExeSuf)
FITDATAS			= src/fitdata.$(SrcSuf)
FITDATAO			= src/fitdata.$(ObjSuf)
FITDATA			= bin/fitdata$(ExeSuf)

#------------------------------------------------------------------------------
# my classes
#------------------------------------------------------------------------------
TUCNCONFIGFILEO 		= src/TUCNConfigFile.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNCONFIGFILES 		= src/TUCNConfigFile.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNRUNO					= src/TUCNRun.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNRUNS					= src/TUCNRun.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNEXPERIMENTO		= src/TUCNExperiment.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNEXPERIMENTS		= src/TUCNExperiment.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNFIELDMANAGERO 	= src/TUCNFieldManager.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNFIELDMANAGERS 	= src/TUCNFieldManager.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNGEOBUILDERO		= src/TUCNGeoBuilder.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOBUILDERS		= src/TUCNGeoBuilder.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEOBBOXO			= src/TUCNGeoBBox.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOBBOXS			= src/TUCNGeoBBox.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEOTUBEO			= src/TUCNGeoTube.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOTUBES			= src/TUCNGeoTube.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNMATERIALO			= src/TUCNMaterial.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNMATERIALS			= src/TUCNMaterial.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGRAVFIELDO			= src/TUCNGravField.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGRAVFIELDS			= src/TUCNGravField.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNPARTICLEO			= src/TUCNParticle.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNPARTICLES			= src/TUCNParticle.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNSTATEO				= src/TUCNState.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNSTATES				= src/TUCNState.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNSPINO				= src/TUCNSpin.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNSPINS				= src/TUCNSpin.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNDATAPARSERO		= src/TUCNDataParser.$(ObjSuf)   UCNDict.$(ObjSuf)
TUCNDATAPARSERS		= src/TUCNDatParser.$(SrcSuf)  	 UCNDict.$(SrcSuf)
TUCNPARABOLAO			= src/TUCNParabola.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNPARABOLAS			= src/TUCNParabola.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNPOLYNOMIALO		= src/TUCNPolynomial.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNPOLYNOMIALS		= src/TUCNPolynomial.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNDATAO				= src/TUCNData.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNDATAS				= src/TUCNData.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNMAGFIELDO			= src/TUCNMagField.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNMAGFIELDS			= src/TUCNMagField.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNUNIFORMMAGFIELDO	= src/TUCNUniformMagField.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNUNIFORMMAGFIELDS	= src/TUCNUniformMagField.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNPARABOLICMAGFIELDO	= src/TUCNParabolicMagField.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNPARABOLICMAGFIELDS	= src/TUCNParabolicMagField.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNGEOCOMPOSITESHAPEO		= src/TUCNGeoCompositeShape.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNGEOCOMPOSITESHAPES		= src/TUCNGeoCompositeShape.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNGEOBOOLNODEO		= src/TUCNGeoBoolNode.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNGEOBOOLNODES		= src/TUCNGeoBoolNode.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNELEMENTO         = src/TUCNElement.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNELEMENTS         = src/TUCNElement.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNVOLUMEO          = src/TUCNVolume.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNVOLUMES          = src/TUCNVolume.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNMAGFIELDMANAGERO = src/TUCNMagFieldManager.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNMAGFIELDMANAGERS = src/TUCNMagFieldManager.$(SrcSuf) UCNDict.$(SrcSuf)


#------------------------------------------------------------------------------
# my library with my classes
#------------------------------------------------------------------------------
LINKDEF			= UCNLinkDef.h
UCNSO				= lib/libUCN.$(DllSuf)
UCNLIB			= -L$(UCN_DIR)/lib -lUCN  -L$(ROOTSYS)/lib -lEG -lGeom -lGeomPainter -lMathMore
#------------------------------------------------------------------------------
OBJS				=	$(UCNSIMO) $(SANDBOXO) $(BUILDTESTO) $(FITDATAO) $(TUCNRUNO) $(TUCNGEOBUILDERO) \
 						$(TUCNGEOBBOXO) $(TUCNGEOTUBEO) $(TUCNMATERIALO) $(TUCNGRAVFIELDO) \
 						$(TUCNPARTICLEO) $(TUCNSTATEO) $(TUCNSPINO) $(TUCNDATAPARSERO) $(TUCNPARABOLAO) \
 						$(TUCNPOLYNOMIALO) $(TUCNEXPERIMENTO) $(TUCNDATAO) $(TUCNMAGFIELDO) \
 						$(TUCNUNIFORMMAGFIELDO)  $(TUCNPARABOLICMAGFIELDO) \
						$(TUCNFIELDMANAGERO) $(TUCNCONFIGFILEO) $(TUCNGEOCOMPOSITESHAPEO) \
						$(TUCNGEOBOOLNODEO) $(TUCNVOLUMEO) $(TUCNELEMENTO) $(TUCNMAGFIELDMANAGERO)
PROGRAMS			=	$(UCNSO) $(UCNSIM) $(BUILDTEST) $(FITDATA) $(SANDBOX) 
#------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:     UCN ucnsim sandbox buildtest fitdata genpart

all:         $(PROGRAMS)

clean:
		@rm -f $(OBJS) $(PROGRAMS) ./lib/* UCNDict.* core *Dict* ./*.txt

ucnsim:			$(UCNSIM)
$(UCNSIM):		$(UCNSIMO) $(UCNSO)
					$(LD) $(LDFLAGS) $(UCNSIMO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

sandbox:			$(SANDBOX)
$(SANDBOX):		$(SANDBOXO) $(UCNSO)
					$(LD) $(LDFLAGS) $(SANDBOXO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

buildtest:		$(BUILDTEST)
$(BUILDTEST):	$(BUILDTESTO) $(UCNSO)
					$(LD) $(LDFLAGS) $(BUILDTESTO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"		

fitdata:			$(FITDATA)
$(FITDATA):		$(FITDATAO) $(TUCNDATAPARSERO)
					$(LD) $(LDFLAGS) $(FITDATAO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"
					
UCN:				$(UCNSO)
$(UCNSO):		$(TUCNRUNO) $(TUCNGEOBUILDERO) $(TUCNGEOBBOXO) $(TUCNGEOTUBEO) $(TUCNMATERIALO) \
               $(TUCNGRAVFIELDO) $(TUCNPARTICLEO) $(TUCNSTATEO) $(TUCNSPINO) $(TUCNDATAPARSERO) \
					$(TUCNPARABOLAO) $(TUCNPOLYNOMIALO) $(TUCNEXPERIMENTO) $(TUCNDATAO) \
					$(TUCNMAGFIELDO) $(TUCNUNIFORMMAGFIELDO)  $(TUCNPARABOLICMAGFIELDO) \
					$(TUCNFIELDMANAGERO) $(TUCNCONFIGFILEO) $(TUCNGEOCOMPOSITESHAPEO) \
					$(TUCNGEOBOOLNODEO) $(TUCNVOLUMEO) $(TUCNELEMENTO) $(TUCNMAGFIELDMANAGERO)

ifeq ($(ARCH),aix)
		/usr/ibmcxx/bin/makeC++SharedLib $(OutPutOpt) $@ $(LIBS) -p 0 $^
else
ifeq ($(ARCH),aix5)
		/usr/vacpp/bin/makeC++SharedLib $(OutPutOpt) $@ $(LIBS) -p 0 $^
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
		   $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(EXPLLINKLIBS) $(OutPutOpt)$@
		$(MT_DLL)
endif
endif
endif

TUCNGEOBUILDERO:				include/TUCNGeoBuilder.h
TUCNGEOBBOXO:					include/TUCNGeoBBox.h
TUCNGEOTUBEO:					include/TUCNGeoTube.h
TUCNMATERIALO:					include/TUCNMaterial.h
TUCNGRAVFIELDO:				include/TUCNGravField.h
TUCNPARTICLEO:					include/TUCNParticle.h
TUCNSTATEO:						include/TUCNState.h
TUCNSPINO:						include/TUCNSpin.h
TUCNDATAPARSERO:				include/TUCNDataParser.h
TUCNPARABOLAO:					include/TUCNParabola.h
TUCNPOLYNOMIALO:				include/TUCNPolynomial.h
TUCNEXPERIMENTO:				include/TUCNExperiment.h
TUCNDATAO:						include/TUCNData.h
TUCNMAGFIELDO:					include/TUCNMagField.h
TUCNUNIFORMMAGFIELDO:		include/TUCNUniformMagField.h
TUCNRUNO:						include/TUCNRun.h
TUCNPARABOLICMAGFIELDO:		include/TUCNParabolicMagField.h
TUCNFIELDMANAGERO:			include/TUCNFieldManager.h
TUCNCONFIGFILEO:				include/TUCNConfigFile.h
TUCNGEOCOMPOSITESHAPEO:		include/TUCNGeoCompositeShape.h
TUCNGEOBOOLNODEO:				include/TUCNGeoBoolNode.h
TUCNVOLUMEO:               include/TUCNVolume.h
TUCNELEMENTO:              include/TUCNElement.h
TUCNMAGFIELDMANAGERO:		include/TUCNMagFieldManager.h

UCNDict.$(SrcSuf):      include/TUCNGeoBuilder.h include/TUCNGeoBBox.h include/TUCNGeoTube.h \
								include/TUCNMaterial.h include/TUCNGravField.h include/TUCNParticle.h \
								include/TUCNState.h include/TUCNSpin.h include/TUCNDataParser.h \
								include/TUCNPolynomial.h include/TUCNParabola.h \
								include/TUCNExperiment.h include/TUCNData.h include/TUCNMagField.h \
								include/TUCNUniformMagField.h include/TUCNParabolicMagField.h \
								include/TUCNRun.h include/TUCNFieldManager.h include/TUCNConfigFile.h \
								include/TUCNGeoCompositeShape.h include/TUCNGeoBoolNode.h \
								include/TUCNVolume.h include/TUCNElement.h include/TUCNMagFieldManager.h \
								$(LINKDEF)
								@echo "Generating dictionary $@..."
								$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -I$(UCN_DIR)/include -I$(BOOST) -c $< -o $@