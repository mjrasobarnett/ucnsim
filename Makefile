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
ProgPath = src/programs
SIMULATE_UCNS = $(ProgPath)/simulate_ucn.$(SrcSuf)
SIMULATE_UCNO = $(ProgPath)/simulate_ucn.$(ObjSuf)
SIMULATE_UCN = bin/simulate_ucn$(ExeSuf)
GENERATE_UCNS = $(ProgPath)/generate_ucn.$(SrcSuf)
GENERATE_UCNO = $(ProgPath)/generate_ucn.$(ObjSuf)
GENERATE_UCN = bin/generate_ucn$(ExeSuf)
SANDBOXS = $(ProgPath)/sandbox.$(SrcSuf)
SANDBOXO = $(ProgPath)/sandbox.$(ObjSuf)
SANDBOX = bin/sandbox$(ExeSuf)
PLOT_DATAS = $(ProgPath)/plot_data.$(SrcSuf)
PLOT_DATAO = $(ProgPath)/plot_data.$(ObjSuf)
PLOT_DATA = bin/plot_data$(ExeSuf)
PLOT_T2S = $(ProgPath)/plot_T2.$(SrcSuf)
PLOT_T2O = $(ProgPath)/plot_T2.$(ObjSuf)
PLOT_T2 = bin/plot_T2$(ExeSuf)
DRAW_TRACKSS = $(ProgPath)/draw_tracks.$(SrcSuf)
DRAW_TRACKSO = $(ProgPath)/draw_tracks.$(ObjSuf)
DRAW_TRACKS = bin/draw_tracks$(ExeSuf)
TEST_KDTREES = $(ProgPath)/test_kdtree.$(SrcSuf)
TEST_KDTREEO = $(ProgPath)/test_kdtree.$(ObjSuf)
TEST_KDTREE = bin/test_kdtree$(ExeSuf)

#------------------------------------------------------------------------------
# my classes
#------------------------------------------------------------------------------
ClassPath = src/classes
CONFIGFILEO = $(ClassPath)/ConfigFile.$(ObjSuf) UCNDict.$(ObjSuf)
CONFIGFILES = $(ClassPath)/ConfigFile.$(SrcSuf) UCNDict.$(SrcSuf)
RUNO = $(ClassPath)/Run.$(ObjSuf)  UCNDict.$(ObjSuf)
RUNS = $(ClassPath)/Run.$(SrcSuf)  UCNDict.$(SrcSuf)
EXPERIMENTO = $(ClassPath)/Experiment.$(ObjSuf)  UCNDict.$(ObjSuf)
EXPERIMENTS = $(ClassPath)/Experiment.$(SrcSuf)  UCNDict.$(SrcSuf)
FIELDMANAGERO = $(ClassPath)/FieldManager.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDMANAGERS = $(ClassPath)/FieldManager.$(SrcSuf) UCNDict.$(SrcSuf)
BOXO = $(ClassPath)/Box.$(ObjSuf) UCNDict.$(ObjSuf)
BOXS = $(ClassPath)/Box.$(SrcSuf) UCNDict.$(SrcSuf)
TUBEO = $(ClassPath)/Tube.$(ObjSuf) UCNDict.$(ObjSuf)
TUBES = $(ClassPath)/Tube.$(SrcSuf) UCNDict.$(SrcSuf)
MATERIALO = $(ClassPath)/Material.$(ObjSuf) UCNDict.$(ObjSuf)
MATERIALS = $(ClassPath)/Material.$(SrcSuf) UCNDict.$(SrcSuf)
GRAVFIELDO = $(ClassPath)/GravField.$(ObjSuf) UCNDict.$(ObjSuf)
GRAVFIELDS = $(ClassPath)/GravField.$(SrcSuf) UCNDict.$(SrcSuf)
PARTICLEO = $(ClassPath)/Particle.$(ObjSuf) UCNDict.$(ObjSuf)
PARTICLES = $(ClassPath)/Particle.$(SrcSuf) UCNDict.$(SrcSuf)
STATEO = $(ClassPath)/State.$(ObjSuf) UCNDict.$(ObjSuf)
STATES = $(ClassPath)/State.$(SrcSuf) UCNDict.$(SrcSuf)
SPINO = $(ClassPath)/Spin.$(ObjSuf) UCNDict.$(ObjSuf)
SPINS = $(ClassPath)/Spin.$(SrcSuf) UCNDict.$(SrcSuf)
FILEPARSERO = $(ClassPath)/FileParser.$(ObjSuf) UCNDict.$(ObjSuf)
FILEPARSERS = $(ClassPath)/FileParser.$(SrcSuf) UCNDict.$(SrcSuf)
PARABOLAO = $(ClassPath)/Parabola.$(ObjSuf) UCNDict.$(ObjSuf)
PARABOLAS = $(ClassPath)/Parabola.$(SrcSuf) UCNDict.$(SrcSuf)
POLYNOMIALO = $(ClassPath)/Polynomial.$(ObjSuf) UCNDict.$(ObjSuf)
POLYNOMIALS = $(ClassPath)/Polynomial.$(SrcSuf) UCNDict.$(SrcSuf)
DATAO = $(ClassPath)/Data.$(ObjSuf) UCNDict.$(ObjSuf)
DATAS = $(ClassPath)/Data.$(SrcSuf) UCNDict.$(SrcSuf)
MAGFIELDO = $(ClassPath)/MagField.$(ObjSuf) UCNDict.$(ObjSuf)
MAGFIELDS = $(ClassPath)/MagField.$(SrcSuf) UCNDict.$(SrcSuf)
UNIFORMMAGFIELDO = $(ClassPath)/UniformMagField.$(ObjSuf) UCNDict.$(ObjSuf)
UNIFORMMAGFIELDS = $(ClassPath)/UniformMagField.$(SrcSuf) UCNDict.$(SrcSuf)
PARABOLICMAGFIELDO = $(ClassPath)/ParabolicMagField.$(ObjSuf) UCNDict.$(ObjSuf)
PARABOLICMAGFIELDS = $(ClassPath)/ParabolicMagField.$(SrcSuf) UCNDict.$(SrcSuf)
COMPOSITESHAPEO = $(ClassPath)/CompositeShape.$(ObjSuf) UCNDict.$(ObjSuf)
COMPOSITESHAPES = $(ClassPath)/CompositeShape.$(SrcSuf) UCNDict.$(SrcSuf)
BOOLNODEO = $(ClassPath)/BoolNode.$(ObjSuf) UCNDict.$(ObjSuf)
BOOLNODES = $(ClassPath)/BoolNode.$(SrcSuf) UCNDict.$(SrcSuf)
ELEMENTO = $(ClassPath)/Element.$(ObjSuf) UCNDict.$(ObjSuf)
ELEMENTS = $(ClassPath)/Element.$(SrcSuf) UCNDict.$(SrcSuf)
VOLUMEO = $(ClassPath)/Volume.$(ObjSuf) UCNDict.$(ObjSuf)
VOLUMES = $(ClassPath)/Volume.$(SrcSuf) UCNDict.$(SrcSuf)
MAGFIELDMANAGERO = $(ClassPath)/MagFieldManager.$(ObjSuf) UCNDict.$(ObjSuf)
MAGFIELDMANAGERS = $(ClassPath)/MagFieldManager.$(SrcSuf) UCNDict.$(SrcSuf)
INITIALCONFIGO = $(ClassPath)/InitialConfig.$(ObjSuf) UCNDict.$(ObjSuf)
INITIALCONFIGS = $(ClassPath)/InitialConfig.$(SrcSuf) UCNDict.$(SrcSuf)
RUNCONFIGO = $(ClassPath)/RunConfig.$(ObjSuf) UCNDict.$(ObjSuf)
RUNCONFIGS = $(ClassPath)/RunConfig.$(SrcSuf) UCNDict.$(SrcSuf)
OBSERVERO = $(ClassPath)/Observer.$(ObjSuf) UCNDict.$(ObjSuf)
OBSERVERS = $(ClassPath)/Observer.$(SrcSuf) UCNDict.$(SrcSuf)
SPINDATAO = $(ClassPath)/SpinData.$(ObjSuf) UCNDict.$(ObjSuf)
SPINDATAS = $(ClassPath)/SpinData.$(SrcSuf) UCNDict.$(SrcSuf)
BOUNCEDATAO = $(ClassPath)/BounceData.$(ObjSuf) UCNDict.$(ObjSuf)
BOUNCEDATAS = $(ClassPath)/BounceData.$(SrcSuf) UCNDict.$(SrcSuf)
FIELDDATAO = $(ClassPath)/FieldData.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDDATAS = $(ClassPath)/FieldData.$(SrcSuf) UCNDict.$(SrcSuf)
TRACKO = $(ClassPath)/Track.$(ObjSuf) UCNDict.$(ObjSuf)
TRACKS = $(ClassPath)/Track.$(SrcSuf) UCNDict.$(SrcSuf)
FIELDMAPO = $(ClassPath)/FieldMap.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDMAPS = $(ClassPath)/FieldMap.$(SrcSuf) UCNDict.$(SrcSuf)
KDTREEO = $(ClassPath)/KDTree.$(ObjSuf) UCNDict.$(ObjSuf)
KDTREES = $(ClassPath)/KDTree.$(SrcSuf) UCNDict.$(SrcSuf)
KDTREENODEO = $(ClassPath)/KDTreeNode.$(ObjSuf) UCNDict.$(ObjSuf)
KDTREENODES = $(ClassPath)/KDTreeNode.$(SrcSuf) UCNDict.$(SrcSuf)
FIELDVERTEXO = $(ClassPath)/FieldVertex.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDVERTEXS = $(ClassPath)/FieldVertex.$(SrcSuf) UCNDict.$(SrcSuf)
VERTEXSTACKO = $(ClassPath)/VertexStack.$(ObjSuf) UCNDict.$(ObjSuf)
VERTEXSTACKS = $(ClassPath)/VertexStack.$(SrcSuf) UCNDict.$(SrcSuf)
POINTO = $(ClassPath)/Point.$(ObjSuf) UCNDict.$(ObjSuf)
POINTS = $(ClassPath)/Point.$(SrcSuf) UCNDict.$(SrcSuf)
OBSERVABLEO = $(ClassPath)/Observable.$(ObjSuf) UCNDict.$(ObjSuf)
OBSERVABLES = $(ClassPath)/Observable.$(SrcSuf) UCNDict.$(SrcSuf)
CLOCKO = $(ClassPath)/Clock.$(ObjSuf) UCNDict.$(ObjSuf)
CLOCKS = $(ClassPath)/Clock.$(SrcSuf) UCNDict.$(SrcSuf)
EVENTO = $(ClassPath)/Event.$(ObjSuf) UCNDict.$(ObjSuf)
EVENTS = $(ClassPath)/Event.$(SrcSuf) UCNDict.$(SrcSuf)

#------------------------------------------------------------------------------
# my library with my classes
#------------------------------------------------------------------------------
LINKDEF = UCNLinkDef.h
UCNSO = lib/libUCN.$(DllSuf)
UCNLIB = -L$(UCN_DIR)/lib -lUCN  -L$(ROOTSYS)/lib -L$(GSL)/lib -lgsl -lgslcblas -lGeom -lEG -lm -lMathMore -lGui -lRGL -lGed
#------------------------------------------------------------------------------
OBJS = $(RUNO) $(BOXO) $(TUBEO) $(MATERIALO) $(GRAVFIELDO) $(PARTICLEO) \
$(STATEO) $(SPINO) $(FILEPARSERO) $(PARABOLAO) $(POLYNOMIALO) $(EXPERIMENTO) $(DATAO) \
$(MAGFIELDO) $(UNIFORMMAGFIELDO)  $(PARABOLICMAGFIELDO) $(FIELDMANAGERO) $(CONFIGFILEO) \
$(COMPOSITESHAPEO) $(BOOLNODEO) $(VOLUMEO) $(ELEMENTO) $(MAGFIELDMANAGERO) $(INITIALCONFIGO) \
$(RUNCONFIGO) $(OBSERVERO) $(SPINDATAO) $(BOUNCEDATAO) $(FIELDDATAO) $(TRACKO) $(FIELDMAPO) \
$(KDTREEO) $(KDTREENODEO) $(FIELDVERTEXO) $(VERTEXSTACKO) $(POINTO) $(OBSERVABLEO) $(CLOCKO) \
$(EVENTO)

PROGRAMS = $(UCNSO) $(SIMULATE_UCN) $(GENERATE_UCN) $(PLOT_DATA) $(SANDBOX) \
$(DRAW_TRACKS) $(TEST_KDTREE) $(PLOT_T2)
#------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY: simulate_ucn sandbox buildtest fitdata generate_ucn plot_data draw_tracks plot_T2

all: $(PROGRAMS)

clean:
		@rm -f $(OBJS) $(PROGRAMS) $(ProgPath)/*.o ./lib/* UCNDict.* core *Dict* ./*.txt

simulate_ucn:			$(SIMULATE_UCN)
$(SIMULATE_UCN):		$(SIMULATE_UCNO) $(UCNSO)
							$(LD) $(LDFLAGS) $(SIMULATE_UCNO) $(UCNLIB) $(LIBS)  \
							$(OutPutOpt)$@
							$(MT_EXE)
							@echo "$@ done"

generate_ucn:			$(GENERATE_UCN)
$(GENERATE_UCN):		$(GENERATE_UCNO) $(UCNSO)
							$(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
							$(OutPutOpt)$@
							$(MT_EXE)
							@echo "$@ done"


sandbox:			$(SANDBOX)
$(SANDBOX):		$(SANDBOXO) $(UCNSO)
					$(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

plot_data:		$(PLOT_DATA)
$(PLOT_DATA):	$(PLOT_DATAO) $(UCNSO)
					$(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

plot_T2:		   $(PLOT_T2)
$(PLOT_T2):	   $(PLOT_T2O) $(UCNSO)
					$(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

draw_tracks:	 $(DRAW_TRACKS)
$(DRAW_TRACKS): $(DRAW_TRACKSO) $(UCNSO)
					 $(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					 $(OutPutOpt)$@
					 $(MT_EXE)
					 @echo "$@ done"

test_kdtree:	 $(TEST_KDTREE)
$(TEST_KDTREE): $(TEST_KDTREEO) $(UCNSO)
					 $(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					 $(OutPutOpt)$@
					 $(MT_EXE)
					 @echo "$@ done"


$(UCNSO):		$(OBJS)

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

BOXO: 						include/Box.h
TUBEO: 						include/Tube.h
MATERIALO: 					include/Material.h
GRAVFIELDO: 				include/GravField.h
PARTICLEO: 					include/Particle.h
STATEO: 						include/State.h
SPINO: 						include/Spin.h
FILEPARSERO: 				include/FileParser.h
PARABOLAO: 					include/Parabola.h
POLYNOMIALO: 				include/Polynomial.h
EXPERIMENTO: 				include/Experiment.h
DATAO: 						include/Data.h
MAGFIELDO: 					include/MagField.h
UNIFORMMAGFIELDO: 		include/UniformMagField.h
RUNO: 						include/Run.h
PARABOLICMAGFIELDO: 		include/ParabolicMagField.h
FIELDMANAGERO: 			include/FieldManager.h
CONFIGFILEO: 				include/ConfigFile.h
COMPOSITESHAPEO: 			include/CompositeShape.h
BOOLNODEO: 					include/BoolNode.h
VOLUMEO: 					include/Volume.h
ELEMENTO: 					include/Element.h
MAGFIELDMANAGERO: 		include/MagFieldManager.h
INITIALCONFIGO: 			include/InitialConfig.h
RUNCONFIGO: 				include/RunConfig.h
OBSERVERO:					include/Observer.h
SPINDATAO:					include/SpinData.h
BOUNCEDATAO:				include/BounceData.h
FIELDDATAO: 				include/FieldData.h
TRACKO:						include/Track.h
FIELDMAPO:					include/FieldMap.h
KDTREEO:						include/KDTree.h
KDTREENODEO:				include/KDTreeNode.h
FIELDVERTEXO:				include/FieldVertex.h
VERTEXSTACKO:				include/VertexStack.h
POINTO:						include/Point.h
OBSERVABLEO:				include/Observable.h
CLOCKO:						include/Clock.h
EVENTO:						include/Event.h

UCNDict.$(SrcSuf):		include/Box.h include/Tube.h \
								include/Material.h include/GravField.h include/Particle.h \
								include/State.h include/Spin.h include/FileParser.h \
								include/Polynomial.h include/Parabola.h \
								include/Experiment.h include/Data.h include/MagField.h \
								include/UniformMagField.h include/ParabolicMagField.h \
								include/Run.h include/FieldManager.h include/ConfigFile.h \
								include/CompositeShape.h include/BoolNode.h \
								include/Volume.h include/Element.h include/MagFieldManager.h \
								include/InitialConfig.h include/RunConfig.h include/Observer.h \
								include/Observables.h include/Track.h include/FieldMap.h \
								include/KDTree.h include/KDTreeNode.h include/FieldVertex.h \
								include/VertexStack.h include/Point.h include/Observable.h \
								include/Clock.h include/Event.h $(LINKDEF)
								@echo "Generating dictionary $@..."
								$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -I$(UCN_DIR)/include -I$(GSL)/include -c $< -o $@
