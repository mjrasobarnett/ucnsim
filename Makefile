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
SIMULATE_UCNS = src/simulate_ucn.$(SrcSuf)
SIMULATE_UCNO = src/simulate_ucn.$(ObjSuf)
SIMULATE_UCN = bin/simulate_ucn$(ExeSuf)
GENERATE_UCNS = src/generate_ucn.$(SrcSuf)
GENERATE_UCNO = src/generate_ucn.$(ObjSuf)
GENERATE_UCN = bin/generate_ucn$(ExeSuf)
SANDBOXS = src/sandbox.$(SrcSuf)
SANDBOXO = src/sandbox.$(ObjSuf)
SANDBOX = bin/sandbox$(ExeSuf)
PLOT_DATAS = src/plot_data.$(SrcSuf)
PLOT_DATAO = src/plot_data.$(ObjSuf)
PLOT_DATA = bin/plot_data$(ExeSuf)
DRAW_TRACKSS = src/draw_tracks.$(SrcSuf)
DRAW_TRACKSO = src/draw_tracks.$(ObjSuf)
DRAW_TRACKS = bin/draw_tracks$(ExeSuf)
TEST_KDTREES = src/test_kdtree.$(SrcSuf)
TEST_KDTREEO = src/test_kdtree.$(ObjSuf)
TEST_KDTREE = bin/test_kdtree$(ExeSuf)

#------------------------------------------------------------------------------
# my classes
#------------------------------------------------------------------------------
CONFIGFILEO = src/ConfigFile.$(ObjSuf) UCNDict.$(ObjSuf)
CONFIGFILES = src/ConfigFile.$(SrcSuf) UCNDict.$(SrcSuf)
RUNO = src/Run.$(ObjSuf)  UCNDict.$(ObjSuf)
RUNS = src/Run.$(SrcSuf)  UCNDict.$(SrcSuf)
EXPERIMENTO = src/Experiment.$(ObjSuf)  UCNDict.$(ObjSuf)
EXPERIMENTS = src/Experiment.$(SrcSuf)  UCNDict.$(SrcSuf)
FIELDMANAGERO = src/FieldManager.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDMANAGERS = src/FieldManager.$(SrcSuf) UCNDict.$(SrcSuf)
BOXO = src/Box.$(ObjSuf) UCNDict.$(ObjSuf)
BOXS = src/Box.$(SrcSuf) UCNDict.$(SrcSuf)
TUBEO = src/Tube.$(ObjSuf) UCNDict.$(ObjSuf)
TUBES = src/Tube.$(SrcSuf) UCNDict.$(SrcSuf)
MATERIALO = src/Material.$(ObjSuf) UCNDict.$(ObjSuf)
MATERIALS = src/Material.$(SrcSuf) UCNDict.$(SrcSuf)
GRAVFIELDO = src/GravField.$(ObjSuf) UCNDict.$(ObjSuf)
GRAVFIELDS = src/GravField.$(SrcSuf) UCNDict.$(SrcSuf)
PARTICLEO = src/Particle.$(ObjSuf) UCNDict.$(ObjSuf)
PARTICLES = src/Particle.$(SrcSuf) UCNDict.$(SrcSuf)
STATEO = src/State.$(ObjSuf) UCNDict.$(ObjSuf)
STATES = src/State.$(SrcSuf) UCNDict.$(SrcSuf)
SPINO = src/Spin.$(ObjSuf) UCNDict.$(ObjSuf)
SPINS = src/Spin.$(SrcSuf) UCNDict.$(SrcSuf)
FILEPARSERO = src/FileParser.$(ObjSuf) UCNDict.$(ObjSuf)
FILEPARSERS = src/FileParser.$(SrcSuf) UCNDict.$(SrcSuf)
PARABOLAO = src/Parabola.$(ObjSuf) UCNDict.$(ObjSuf)
PARABOLAS = src/Parabola.$(SrcSuf) UCNDict.$(SrcSuf)
POLYNOMIALO = src/Polynomial.$(ObjSuf) UCNDict.$(ObjSuf)
POLYNOMIALS = src/Polynomial.$(SrcSuf) UCNDict.$(SrcSuf)
DATAO = src/Data.$(ObjSuf) UCNDict.$(ObjSuf)
DATAS = src/Data.$(SrcSuf) UCNDict.$(SrcSuf)
MAGFIELDO = src/MagField.$(ObjSuf) UCNDict.$(ObjSuf)
MAGFIELDS = src/MagField.$(SrcSuf) UCNDict.$(SrcSuf)
UNIFORMMAGFIELDO = src/UniformMagField.$(ObjSuf) UCNDict.$(ObjSuf)
UNIFORMMAGFIELDS = src/UniformMagField.$(SrcSuf) UCNDict.$(SrcSuf)
PARABOLICMAGFIELDO = src/ParabolicMagField.$(ObjSuf) UCNDict.$(ObjSuf)
PARABOLICMAGFIELDS = src/ParabolicMagField.$(SrcSuf) UCNDict.$(SrcSuf)
COMPOSITESHAPEO = src/CompositeShape.$(ObjSuf) UCNDict.$(ObjSuf)
COMPOSITESHAPES = src/CompositeShape.$(SrcSuf) UCNDict.$(SrcSuf)
BOOLNODEO = src/BoolNode.$(ObjSuf) UCNDict.$(ObjSuf)
BOOLNODES = src/BoolNode.$(SrcSuf) UCNDict.$(SrcSuf)
ELEMENTO = src/Element.$(ObjSuf) UCNDict.$(ObjSuf)
ELEMENTS = src/Element.$(SrcSuf) UCNDict.$(SrcSuf)
VOLUMEO = src/Volume.$(ObjSuf) UCNDict.$(ObjSuf)
VOLUMES = src/Volume.$(SrcSuf) UCNDict.$(SrcSuf)
MAGFIELDMANAGERO = src/MagFieldManager.$(ObjSuf) UCNDict.$(ObjSuf)
MAGFIELDMANAGERS = src/MagFieldManager.$(SrcSuf) UCNDict.$(SrcSuf)
INITIALCONFIGO = src/InitialConfig.$(ObjSuf) UCNDict.$(ObjSuf)
INITIALCONFIGS = src/InitialConfig.$(SrcSuf) UCNDict.$(SrcSuf)
RUNCONFIGO = src/RunConfig.$(ObjSuf) UCNDict.$(ObjSuf)
RUNCONFIGS = src/RunConfig.$(SrcSuf) UCNDict.$(SrcSuf)
OBSERVERO = src/Observer.$(ObjSuf) UCNDict.$(ObjSuf)
OBSERVERS = src/Observer.$(SrcSuf) UCNDict.$(SrcSuf)
SPINDATAO = src/SpinData.$(ObjSuf) UCNDict.$(ObjSuf)
SPINDATAS = src/SpinData.$(SrcSuf) UCNDict.$(SrcSuf)
BOUNCEDATAO = src/BounceData.$(ObjSuf) UCNDict.$(ObjSuf)
BOUNCEDATAS = src/BounceData.$(SrcSuf) UCNDict.$(SrcSuf)
FIELDDATAO = src/FieldData.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDDATAS = src/FieldData.$(SrcSuf) UCNDict.$(SrcSuf)
TRACKO = src/Track.$(ObjSuf) UCNDict.$(ObjSuf)
TRACKS = src/Track.$(SrcSuf) UCNDict.$(SrcSuf)
FIELDMAPO = src/FieldMap.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDMAPS = src/FieldMap.$(SrcSuf) UCNDict.$(SrcSuf)
KDTREEO = src/KDTree.$(ObjSuf) UCNDict.$(ObjSuf)
KDTREES = src/KDTree.$(SrcSuf) UCNDict.$(SrcSuf)
KDTREENODEO = src/KDTreeNode.$(ObjSuf) UCNDict.$(ObjSuf)
KDTREENODES = src/KDTreeNode.$(SrcSuf) UCNDict.$(SrcSuf)
FIELDVERTEXO = src/FieldVertex.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDVERTEXS = src/FieldVertex.$(SrcSuf) UCNDict.$(SrcSuf)
VERTEXSTACKO = src/VertexStack.$(ObjSuf) UCNDict.$(ObjSuf)
VERTEXSTACKS = src/VertexStack.$(SrcSuf) UCNDict.$(SrcSuf)
POINTO = src/Point.$(ObjSuf) UCNDict.$(ObjSuf)
POINTS = src/Point.$(SrcSuf) UCNDict.$(SrcSuf)
OBSERVABLEO = src/Observable.$(ObjSuf) UCNDict.$(ObjSuf)
OBSERVABLES = src/Observable.$(SrcSuf) UCNDict.$(SrcSuf)

#------------------------------------------------------------------------------
# my library with my classes
#------------------------------------------------------------------------------
LINKDEF = UCNLinkDef.h
UCNSO = lib/libUCN.$(DllSuf)
UCNLIB = -L$(UCN_DIR)/lib -lUCN  -L$(ROOTSYS)/lib -lGeom -lEG -lm -lMathMore -lGui -lRGL -lGed
#------------------------------------------------------------------------------
OBJS = $(RUNO) $(BOXO) $(TUBEO) $(MATERIALO) $(GRAVFIELDO) $(PARTICLEO) \
$(STATEO) $(SPINO) $(FILEPARSERO) $(PARABOLAO) $(POLYNOMIALO) $(EXPERIMENTO) $(DATAO) \
$(MAGFIELDO) $(UNIFORMMAGFIELDO)  $(PARABOLICMAGFIELDO) $(FIELDMANAGERO) $(CONFIGFILEO) \
$(COMPOSITESHAPEO) $(BOOLNODEO) $(VOLUMEO) $(ELEMENTO) $(MAGFIELDMANAGERO) $(INITIALCONFIGO) \
$(RUNCONFIGO) $(OBSERVERO) $(SPINDATAO) $(BOUNCEDATAO) $(FIELDDATAO) $(TRACKO) $(FIELDMAPO) \
$(KDTREEO) $(KDTREENODEO) $(FIELDVERTEXO) $(VERTEXSTACKO) $(POINTO) $(OBSERVABLEO)

PROGRAMS = $(UCNSO) $(SIMULATE_UCN) $(GENERATE_UCN) $(PLOT_DATA) $(SANDBOX) \
$(DRAW_TRACKS) $(TEST_KDTREE)
#------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY: simulate_ucn sandbox buildtest fitdata generate_ucn plot_data draw_tracks

all: $(PROGRAMS)

clean:
		@rm -f $(OBJS) $(PROGRAMS) ./src/*.o ./lib/* UCNDict.* core *Dict* ./*.txt

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
								include/VertexStack.h include/Point.h include/Observable.h $(LINKDEF)
								@echo "Generating dictionary $@..."
								$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -I$(UCN_DIR)/include -c $< -o $@