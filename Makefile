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
SANDBOXS			= src/sandbox.$(SrcSuf)
SANDBOXO			= src/sandbox.$(ObjSuf)
SANDBOX				= bin/sandbox$(ExeSuf)
UCNSIMS				= src/ucnsim.$(SrcSuf)
UCNSIMO				= src/ucnsim.$(ObjSuf)
UCNSIM				= bin/ucnsim$(ExeSuf)
FITDATAS			= src/fitdata.$(SrcSuf)
FITDATAO			= src/fitdata.$(ObjSuf)
FITDATA				= bin/fitdata$(ExeSuf)

#------------------------------------------------------------------------------
# root that knows about my classes
#------------------------------------------------------------------------------
#NEWROOTS	 = src/new_root.$(SrcSuf)
#NEWROOTO	 = src/new_root.$(ObjSuf)
#NEWROOT	 = bin/new_root$(ExeSuf)

#------------------------------------------------------------------------------
# my classes
#------------------------------------------------------------------------------
TUCNGEOMANAGERO			= src/TUCNGeoManager.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOMANAGERS			= src/TUCNGeoManager.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEONAVIGATORO			= src/TUCNGeoNavigator.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEONAVIGATORS			= src/TUCNGeoNavigator.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEOBUILDERO			= src/TUCNGeoBuilder.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOBUILDERS			= src/TUCNGeoBuilder.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEOBBOXO				= src/TUCNGeoBBox.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOBBOXS				= src/TUCNGeoBBox.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEOTUBEO				= src/TUCNGeoTube.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOTUBES				= src/TUCNGeoTube.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGEOMATERIALO			= src/TUCNGeoMaterial.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGEOMATERIALS			= src/TUCNGeoMaterial.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNGRAVFIELDO				= src/TUCNGravField.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNGRAVFIELDS				= src/TUCNGravField.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNPARTICLEO				= src/TUCNParticle.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNPARTICLES				= src/TUCNParticle.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNEXPERIMENTO			= src/TUCNExperiment.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNEXPERIMENTS			= src/TUCNExperiment.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNDATAPARSERO			= src/TUCNDataParser.$(ObjSuf)   UCNDict.$(ObjSuf)
TUCNDATAPARSERS			= src/TUCNDatParser.$(SrcSuf)  	 UCNDict.$(SrcSuf)
TUCNPARABOLAO				= src/TUCNParabola.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNPARABOLAS				= src/TUCNParabola.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNPOLYNOMIALO			= src/TUCNPolynomial.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNPOLYNOMIALS			= src/TUCNPolynomial.$(SrcSuf)  UCNDict.$(SrcSuf)
TUCNDATAO					= src/TUCNData.$(ObjSuf) UCNDict.$(ObjSuf)
TUCNDATAS					= src/TUCNData.$(SrcSuf) UCNDict.$(SrcSuf)
TUCNRUNMANAGERO			= src/TUCNRunManager.$(ObjSuf)  UCNDict.$(ObjSuf)
TUCNRUNMANAGERS			= src/TUCNRunManager.$(SrcSuf)  UCNDict.$(SrcSuf)

#------------------------------------------------------------------------------
# my library with my classes
#------------------------------------------------------------------------------
LINKDEF			= UCNLinkDef.h
UCNSO				= lib/libUCN.$(DllSuf)
UCNLIB			= -L./lib -lUCN -L$(ROOTSYS)/lib -lEG -lGeom -lGeomPainter
#------------------------------------------------------------------------------
OBJS         = $(SANDBOXO) $(UCNSIMO) $(FITDATAO) $(TUCNGEOMANAGERO) $(TUCNGEONAVIGATORO) $(TUCNGEOBUILDERO) $(TUCNGEOBBOXO) $(TUCNGEOTUBEO) \
					$(TUCNGEOMATERIALO) $(TUCNGRAVFIELDO) $(TUCNPARTICLEO) $(TUCNDATAPARSERO) $(TUCNPARABOLAO) \
					$(TUCNPOLYNOMIALO) $(TUCNRUNMANAGERO) $(TUCNDATAO) 
PROGRAMS     = $(UCNSO) $(UCNSIM) $(FITDATA) $(SANDBOX)
#------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:     UCN

all:         $(PROGRAMS)

clean:
		@rm -f $(OBJS) $(PROGRAMS) ./lib/* UCNDict.* core *Dict* ./*.txt

$(SANDBOX):		$(SANDBOXO) $(TUCNGEOMANAGERO) $(TUCNGEONAVIGATORO) $(TUCNGEOBUILDERO) $(TUCNGEOBBOXO) \
					$(TUCNGEOTUBEO) $(TUCNGEOMATERIALO) $(TUCNGRAVFIELDO) $(TUCNPARTICLEO) $(TUCNPARABOLAO) \
					$(TUCNPOLYNOMIALO) $(TUCNRUNMANAGERO) $(TUCNDATAO)
					$(LD) $(LDFLAGS) $(SANDBOXO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

$(UCNSIM):		$(UCNSIMO) $(TUCNGEOMANAGERO) $(TUCNGEONAVIGATORO) $(TUCNGEOBUILDERO) $(TUCNGEOBBOXO) \
					$(TUCNGEOTUBEO) $(TUCNGEOMATERIALO) $(TUCNGRAVFIELDO) $(TUCNPARTICLEO) $(TUCNPARABOLAO) \
					$(TUCNPOLYNOMIALO) $(TUCNRUNMANAGERO) $(TUCNDATAO)
					$(LD) $(LDFLAGS) $(UCNSIMO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"		

$(FITDATA):		$(FITDATAO) $(TUCNDATAPARSERO)
					$(LD) $(LDFLAGS) $(FITDATAO) $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

UCN:				$(UCNSO)
$(UCNSO):		$(TUCNGEOMANAGERO) $(TUCNGEONAVIGATORO) $(TUCNGEOBUILDERO) $(TUCNGEOBBOXO) $(TUCNGEOTUBEO) \
					$(TUCNGEOMATERIALO) $(TUCNGRAVFIELDO) $(TUCNPARTICLEO) $(TUCNDATAPARSERO) $(TUCNPARABOLAO) \
					$(TUCNPOLYNOMIALO) $(TUCNRUNMANAGERO) $(TUCNDATAO)

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

TUCNGEOMANAGERO:				include/TUCNGeoManager.h
TUCNGEONAVIGATORO:			include/TUCNGeoNavigator.h 
TUCNGEOBUILDERO:				include/TUCNGeoBuilder.h
TUCNGEOBBOXO:					include/TUCNGeoBBox.h
TUCNGEOTUBEO:					include/TUCNGeoTube.h
TUCNGEOMATERIALO:				include/TUCNGeoMaterial.h
TUCNGRAVFIELDO:				include/TUCNGravField.h
TUCNPARTICLEO:					include/TUCNParticle.h
TUCNDATAPARSERO:				include/TUCNDataParser.h
TUCNPARABOLAO:					include/TUCNParabola.h
TUCNPOLYNOMIALO:				include/TUCNPolynomial.h
TUCNRUNMANAGERO:				include/TUCNRunManager.h
TUCNDATAO:						include/TUCNData.h

UCNDict.$(SrcSuf):		include/TUCNGeoManager.h include/TUCNGeoNavigator.h include/TUCNGeoBuilder.h \
								include/TUCNGeoBBox.h include/TUCNGeoTube.h include/TUCNGeoMaterial.h \
								include/TUCNGravField.h include/TUCNParticle.h \
								include/TUCNDataParser.h include/TUCNPolynomial.h include/TUCNParabola.h \
								include/TUCNRunManager.h include/TUCNData.h $(LINKDEF)
								@echo "Generating dictionary $@..."
								$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -I./include -c $< -o $@