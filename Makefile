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
SIMULATE_UCNS	= src/simulate_ucn.$(SrcSuf)
SIMULATE_UCNO	= src/simulate_ucn.$(ObjSuf)
SIMULATE_UCN	= bin/simulate_ucn$(ExeSuf)
GENERATE_UCNS	= src/generate_ucn.$(SrcSuf)
GENERATE_UCNO	= src/generate_ucn.$(ObjSuf)
GENERATE_UCN	= bin/generate_ucn$(ExeSuf)
SANDBOXS			= src/sandbox.$(SrcSuf)
SANDBOXO			= src/sandbox.$(ObjSuf)
SANDBOX			= bin/sandbox$(ExeSuf)
BUILDTESTS		= src/buildtest.$(SrcSuf)
BUILDTESTO		= src/buildtest.$(ObjSuf)
BUILDTEST		= bin/buildtest$(ExeSuf)
PLOT_DATAS		= src/plot_data.$(SrcSuf)
PLOT_DATAO		= src/plot_data.$(ObjSuf)
PLOT_DATA		= bin/plot_data$(ExeSuf)
DRAW_TRACKSS	= src/draw_tracks.$(SrcSuf)
DRAW_TRACKSO	= src/draw_tracks.$(ObjSuf)
DRAW_TRACKS		= bin/draw_tracks$(ExeSuf)

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
TUCNINITIALCONFIGO = src/TUCNInitialConfig.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNINITIALCONFIGS = src/TUCNInitialConfig.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNRUNCONFIGO = src/TUCNRunConfig.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNRUNCONFIGS = src/TUCNRunConfig.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNOBSERVERO = src/TUCNObserver.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNOBSERVERS = src/TUCNObserver.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNOBSERVABLESO = src/TUCNObservables.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNOBSERVABLESS = src/TUCNObservables.$(SrcSuf) UCNDict.$(SrcSuf)


#------------------------------------------------------------------------------
# my library with my classes
#------------------------------------------------------------------------------
LINKDEF			= UCNLinkDef.h
UCNSO				= lib/libUCN.$(DllSuf)
UCNLIB			= -L$(UCN_DIR)/lib -lUCN  -L$(ROOTSYS)/lib -lEG -lGeom -lGeomPainter -lMathMore \
					  -lGui -lRGL -lGed
#------------------------------------------------------------------------------
OBJS				=	$(SIMULATE_UCNO) $(GENERATE_UCNO) $(SANDBOXO) $(BUILDTESTO) $(PLOT_DATAO) \
						$(DRAW_TRACKSO) $(TUCNRUNO) \
 						$(TUCNGEOBBOXO) $(TUCNGEOTUBEO) $(TUCNMATERIALO) $(TUCNGRAVFIELDO) \
 						$(TUCNPARTICLEO) $(TUCNSTATEO) $(TUCNSPINO) $(TUCNDATAPARSERO) $(TUCNPARABOLAO) \
 						$(TUCNPOLYNOMIALO) $(TUCNEXPERIMENTO) $(TUCNDATAO) $(TUCNMAGFIELDO) \
 						$(TUCNUNIFORMMAGFIELDO)  $(TUCNPARABOLICMAGFIELDO) \
						$(TUCNFIELDMANAGERO) $(TUCNCONFIGFILEO) $(TUCNGEOCOMPOSITESHAPEO) \
						$(TUCNGEOBOOLNODEO) $(TUCNVOLUMEO) $(TUCNELEMENTO) $(TUCNMAGFIELDMANAGERO) \
						$(TUCNINITIALCONFIGO) $(TUCNRUNCONFIGO) $(TUCNOBSERVERO) $(TUCNOBSERVABLESO)
PROGRAMS			=	$(UCNSO) $(SIMULATE_UCN) $(GENERATE_UCN) $(BUILDTEST) $(PLOT_DATA) $(SANDBOX) \
 						$(DRAW_TRACKS) 
#------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:     UCN simulate_ucn sandbox buildtest fitdata generate_ucn plot_data draw_tracks

all:         $(PROGRAMS)

clean:
		@rm -f $(OBJS) $(PROGRAMS) ./lib/* UCNDict.* core *Dict* ./*.txt

simulate_ucn:			$(SIMULATE_UCN)
$(SIMULATE_UCN):		$(SIMULATE_UCNO) $(UCNSO)
							$(LD) $(LDFLAGS) $(SIMULATE_UCNO) $(UCNLIB) $(LIBS)  \
							$(OutPutOpt)$@
							$(MT_EXE)
							@echo "$@ done"

generate_ucn:			$(GENERATE_UCN)
$(GENERATE_UCN):		$(GENERATE_UCNO) $(UCNSO)
							$(LD) $(LDFLAGS) $(GENERATE_UCNO) $(UCNLIB) $(LIBS)  \
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

plot_data:		$(PLOT_DATA)
$(PLOT_DATA):	$(PLOT_DATAO) $(UCNSO)
					$(LD) $(LDFLAGS) $(PLOT_DATAO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

draw_tracks:	 $(DRAW_TRACKS)
$(DRAW_TRACKS): $(DRAW_TRACKSO) $(UCNSO)
					 $(LD) $(LDFLAGS) $(DRAW_TRACKSO) $(UCNLIB) $(LIBS)  \
					 $(OutPutOpt)$@
					 $(MT_EXE)
					 @echo "$@ done"

UCN:				$(UCNSO)
$(UCNSO):		$(TUCNRUNO) $(TUCNGEOBBOXO) $(TUCNGEOTUBEO) $(TUCNMATERIALO) \
               $(TUCNGRAVFIELDO) $(TUCNPARTICLEO) $(TUCNSTATEO) $(TUCNSPINO) $(TUCNDATAPARSERO) \
					$(TUCNPARABOLAO) $(TUCNPOLYNOMIALO) $(TUCNEXPERIMENTO) $(TUCNDATAO) \
					$(TUCNMAGFIELDO) $(TUCNUNIFORMMAGFIELDO)  $(TUCNPARABOLICMAGFIELDO) \
					$(TUCNFIELDMANAGERO) $(TUCNCONFIGFILEO) $(TUCNGEOCOMPOSITESHAPEO) \
					$(TUCNGEOBOOLNODEO) $(TUCNVOLUMEO) $(TUCNELEMENTO) $(TUCNMAGFIELDMANAGERO) \
					$(TUCNINITIALCONFIGO) $(TUCNRUNCONFIGO) $(TUCNOBSERVERO) $(TUCNOBSERVABLESO)

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
TUCNINITIALCONFIGO:			include/TUCNInitialConfig.h
TUCNRUNCONFIGO:				include/TUCNRunConfig.h
TUCNOBSERVERO:					include/TUCNObserver.h
TUCNOBSERVABLESO:				include/TUCNObservables.h

UCNDict.$(SrcSuf):      include/TUCNGeoBuilder.h include/TUCNGeoBBox.h include/TUCNGeoTube.h \
								include/TUCNMaterial.h include/TUCNGravField.h include/TUCNParticle.h \
								include/TUCNState.h include/TUCNSpin.h include/TUCNDataParser.h \
								include/TUCNPolynomial.h include/TUCNParabola.h \
								include/TUCNExperiment.h include/TUCNData.h include/TUCNMagField.h \
								include/TUCNUniformMagField.h include/TUCNParabolicMagField.h \
								include/TUCNRun.h include/TUCNFieldManager.h include/TUCNConfigFile.h \
								include/TUCNGeoCompositeShape.h include/TUCNGeoBoolNode.h \
								include/TUCNVolume.h include/TUCNElement.h include/TUCNMagFieldManager.h \
								include/TUCNInitialConfig.h include/TUCNRunConfig.h include/TUCNObserver.h \
								include/TUCNObservables.h $(LINKDEF)
								@echo "Generating dictionary $@..."
								$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -I$(UCN_DIR)/include -I$(BOOST) -c $< -o $@