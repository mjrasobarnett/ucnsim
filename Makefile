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
MAKE_PLOTSS = $(ProgPath)/make_plots.$(SrcSuf)
MAKE_PLOTSO = $(ProgPath)/make_plots.$(ObjSuf)
MAKE_PLOTS = bin/make_plots$(ExeSuf)
MAKE_T2PLOTS = $(ProgPath)/make_T2plot.$(SrcSuf)
MAKE_T2PLOTO = $(ProgPath)/make_T2plot.$(ObjSuf)
MAKE_T2PLOT = bin/make_T2plot$(ExeSuf)
MAKE_DENSITYS = $(ProgPath)/make_density_snapshots.$(SrcSuf)
MAKE_DENSITYO = $(ProgPath)/make_density_snapshots.$(ObjSuf)
MAKE_DENSITY = bin/make_density_snapshots$(ExeSuf)
DRAW_TRACKSS = $(ProgPath)/draw_tracks.$(SrcSuf)
DRAW_TRACKSO = $(ProgPath)/draw_tracks.$(ObjSuf)
DRAW_TRACKS = bin/draw_tracks$(ExeSuf)
DRAW_PLOTSS = $(ProgPath)/draw_plots.$(SrcSuf)
DRAW_PLOTSO = $(ProgPath)/draw_plots.$(ObjSuf)
DRAW_PLOTS = bin/draw_plots$(ExeSuf)
SANDBOXS = $(ProgPath)/sandbox.$(SrcSuf)
SANDBOXO = $(ProgPath)/sandbox.$(ObjSuf)
SANDBOX = bin/sandbox$(ExeSuf)
TEST_KDTREES = $(ProgPath)/test_kdtree.$(SrcSuf)
TEST_KDTREEO = $(ProgPath)/test_kdtree.$(ObjSuf)
TEST_KDTREE = bin/test_kdtree$(ExeSuf)
BATCH_SIMULATES = $(ProgPath)/batch_simulate.$(SrcSuf)
BATCH_SIMULATEO = $(ProgPath)/batch_simulate.$(ObjSuf)
BATCH_SIMULATE = bin/batch_simulate$(ExeSuf)


#------------------------------------------------------------------------------
# my classes
#------------------------------------------------------------------------------
ClassPath = src/classes
ClassIncPath = include/classes
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
FIELDO = $(ClassPath)/Field.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDS = $(ClassPath)/Field.$(SrcSuf) UCNDict.$(SrcSuf)
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
FIELDARRAYO = $(ClassPath)/FieldArray.$(ObjSuf) UCNDict.$(ObjSuf)
FIELDARRAYS = $(ClassPath)/FieldArray.$(SrcSuf) UCNDict.$(SrcSuf)
MAGFIELDARRAYO = $(ClassPath)/MagFieldArray.$(ObjSuf) UCNDict.$(ObjSuf)
MAGFIELDARRAYS = $(ClassPath)/MagFieldArray.$(SrcSuf) UCNDict.$(SrcSuf)
ELECFIELDO = $(ClassPath)/ElecField.$(ObjSuf) UCNDict.$(ObjSuf)
ELECFIELDS = $(ClassPath)/ElecField.$(SrcSuf) UCNDict.$(SrcSuf)
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
UNIFORMELECFIELDO = $(ClassPath)/UniformElecField.$(ObjSuf) UCNDict.$(ObjSuf)
UNIFORMELECFIELDS = $(ClassPath)/UniformElecField.$(SrcSuf) UCNDict.$(SrcSuf)
ELECFIELDARRAYO = $(ClassPath)/ElecFieldArray.$(ObjSuf) UCNDict.$(ObjSuf)
ELECFIELDARRAYS = $(ClassPath)/ElecFieldArray.$(SrcSuf) UCNDict.$(SrcSuf)
ALGORITHMSO = src/Algorithms.$(ObjSuf) UCNDict.$(ObjSuf)
ALGORITHMSS = src/Algorithms.$(SrcSuf) UCNDict.$(SrcSuf)
MATERIALSO = src/Materials.$(ObjSuf) UCNDict.$(ObjSuf)
MATERIALSS = src/Materials.$(SrcSuf) UCNDict.$(SrcSuf)
DATAANALYSISO = src/DataAnalysis.$(ObjSuf) UCNDict.$(ObjSuf)
DATAANALYSISS = src/DataAnalysis.$(SrcSuf) UCNDict.$(SrcSuf)


#------------------------------------------------------------------------------
# my library with my classes
#------------------------------------------------------------------------------
LINKDEF = UCNLinkDef.h
UCNSO = lib/libUCN.$(DllSuf)
UCNLIB = -L$(UCN_DIR)/lib -lUCN  -L$(ROOTSYS)/lib -L$(GSL)/lib -lgsl -lgslcblas -lGeom -lEG -lm -lGui -lRGL -lGed
#------------------------------------------------------------------------------
OBJS = $(RUNO) $(BOXO) $(TUBEO) $(MATERIALO) $(GRAVFIELDO) $(PARTICLEO) \
$(STATEO) $(SPINO) $(FILEPARSERO) $(PARABOLAO) $(POLYNOMIALO) $(EXPERIMENTO) $(DATAO) \
$(FIELDO) $(MAGFIELDO) $(UNIFORMMAGFIELDO)  $(PARABOLICMAGFIELDO) $(FIELDMANAGERO) $(CONFIGFILEO) \
$(COMPOSITESHAPEO) $(BOOLNODEO) $(VOLUMEO) $(ELEMENTO) $(FIELDARRAYO) $(MAGFIELDARRAYO) \
$(ELECFIELDO) $(INITIALCONFIGO) $(RUNCONFIGO) $(OBSERVERO) $(SPINDATAO) \
$(BOUNCEDATAO) $(FIELDDATAO) $(TRACKO) \
$(FIELDMAPO) $(KDTREEO) $(KDTREENODEO) $(FIELDVERTEXO) $(VERTEXSTACKO) $(POINTO) $(OBSERVABLEO) \
$(CLOCKO) $(EVENTO) $(ALGORITHMSO) $(UNIFORMELECFIELDO) $(ELECFIELDARRAYO) $(MATERIALSO) $(DATAANALYSISO)

PROGRAMS = $(UCNSO) $(SIMULATE_UCN) $(GENERATE_UCN) $(MAKE_PLOTS) $(SANDBOX) \
$(DRAW_TRACKS) $(TEST_KDTREE) $(MAKE_T2PLOT) $(DRAW_PLOTS) $(MAKE_DENSITY) $(BATCH_SIMULATE)
#------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY: simulate_ucn sandbox buildtest fitdata generate_ucn make_plots draw_tracks make_T2plot  draw_plots

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

make_plots:		$(MAKE_PLOTS)
$(MAKE_PLOTS):	$(MAKE_PLOTSO) $(UCNSO)
					$(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					$(OutPutOpt)$@
					$(MT_EXE)
					@echo "$@ done"

make_T2plot:	   $(MAKE_T2PLOT)
$(MAKE_T2PLOT):   $(MAKE_T2PLOTO) $(UCNSO)
					   $(LD) $(LDFLAGS) $< $(UCNLIB) $(LIBS)  \
					   $(OutPutOpt)$@
					   $(MT_EXE)
					   @echo "$@ done"

make_density:		$(MAKE_DENSITY)
$(MAKE_DENSITY):	$(MAKE_DENSITYO) $(UCNSO)
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

draw_plots: 	 $(DRAW_PLOTS)
$(DRAW_PLOTS):  $(DRAW_PLOTSO) $(UCNSO)
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

batch_simulate:	 $(BATCH_SIMULATE)
$(BATCH_SIMULATE): $(BATCH_SIMULATEO) $(UCNSO)
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

BOXO: 						$(ClassIncPath)/Box.h
TUBEO: 						$(ClassIncPath)/Tube.h
MATERIALO: 					$(ClassIncPath)/Material.h
GRAVFIELDO: 				$(ClassIncPath)/GravField.h
PARTICLEO: 					$(ClassIncPath)/Particle.h
STATEO: 						$(ClassIncPath)/State.h
SPINO: 						$(ClassIncPath)/Spin.h
FILEPARSERO: 				$(ClassIncPath)/FileParser.h
PARABOLAO: 					$(ClassIncPath)/Parabola.h
POLYNOMIALO: 				$(ClassIncPath)/Polynomial.h
EXPERIMENTO: 				$(ClassIncPath)/Experiment.h
DATAO: 						$(ClassIncPath)/Data.h
FIELDO:						$(ClassIncPath)/Field.h
MAGFIELDO: 					$(ClassIncPath)/MagField.h
UNIFORMMAGFIELDO: 		$(ClassIncPath)/UniformMagField.h
RUNO: 						$(ClassIncPath)/Run.h
PARABOLICMAGFIELDO: 		$(ClassIncPath)/ParabolicMagField.h
FIELDMANAGERO: 			$(ClassIncPath)/FieldManager.h
CONFIGFILEO: 				$(ClassIncPath)/ConfigFile.h
COMPOSITESHAPEO: 			$(ClassIncPath)/CompositeShape.h
BOOLNODEO: 					$(ClassIncPath)/BoolNode.h
VOLUMEO: 					$(ClassIncPath)/Volume.h
ELEMENTO: 					$(ClassIncPath)/Element.h
FIELDARRAYO:				$(ClassIncPath)/FieldArray.h
MAGFIELDARRAYO: 			$(ClassIncPath)/MagFieldArray.h
ELECFIELDO:					$(ClassIncPath)/ElecField.h
INITIALCONFIGO: 			$(ClassIncPath)/InitialConfig.h
RUNCONFIGO: 				$(ClassIncPath)/RunConfig.h
OBSERVERO:					$(ClassIncPath)/Observer.h
SPINDATAO:					$(ClassIncPath)/SpinData.h
BOUNCEDATAO:				$(ClassIncPath)/BounceData.h
FIELDDATAO: 				$(ClassIncPath)/FieldData.h
TRACKO:						$(ClassIncPath)/Track.h
FIELDMAPO:					$(ClassIncPath)/FieldMap.h
KDTREEO:						$(ClassIncPath)/KDTree.h
KDTREENODEO:				$(ClassIncPath)/KDTreeNode.h
FIELDVERTEXO:				$(ClassIncPath)/FieldVertex.h
VERTEXSTACKO:				$(ClassIncPath)/VertexStack.h
POINTO:						$(ClassIncPath)/Point.h
OBSERVABLEO:				$(ClassIncPath)/Observable.h
CLOCKO:						$(ClassIncPath)/Clock.h
ALGORITHMSO:				include/Algorithms.h
MATERIALSO:					include/Materials.h
DATAANALYSISO:				include/DataAnalysis.h

UCNDict.$(SrcSuf):		$(ClassIncPath)/Box.h $(ClassIncPath)/Tube.h \
								$(ClassIncPath)/Material.h $(ClassIncPath)/GravField.h $(ClassIncPath)/Particle.h \
								$(ClassIncPath)/State.h $(ClassIncPath)/Spin.h $(ClassIncPath)/FileParser.h \
								$(ClassIncPath)/Polynomial.h $(ClassIncPath)/Parabola.h \
								$(ClassIncPath)/Experiment.h $(ClassIncPath)/Data.h $(ClassIncPath)/Field.h \
								$(ClassIncPath)/MagField.h \
								$(ClassIncPath)/UniformMagField.h $(ClassIncPath)/ParabolicMagField.h \
								$(ClassIncPath)/Run.h $(ClassIncPath)/FieldManager.h $(ClassIncPath)/ConfigFile.h \
								$(ClassIncPath)/CompositeShape.h $(ClassIncPath)/BoolNode.h $(ClassIncPath)/Volume.h \
								$(ClassIncPath)/Element.h $(ClassIncPath)/FieldArray.h \
								$(ClassIncPath)/MagFieldArray.h $(ClassIncPath)/ElecField.h \
								$(ClassIncPath)/InitialConfig.h $(ClassIncPath)/RunConfig.h \
								$(ClassIncPath)/Observer.h $(ClassIncPath)/Track.h $(ClassIncPath)/FieldMap.h \
								$(ClassIncPath)/KDTree.h $(ClassIncPath)/KDTreeNode.h $(ClassIncPath)/FieldVertex.h \
								$(ClassIncPath)/VertexStack.h $(ClassIncPath)/Point.h $(ClassIncPath)/Observable.h \
								$(ClassIncPath)/Clock.h $(ClassIncPath)/UniformElecField.h \
								$(ClassIncPath)/ElecFieldArray.h include/Algorithms.h include/Materials.h \
								include/DataAnalysis.h $(LINKDEF)
								@echo "Generating dictionary $@..."
								$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -I$(UCN_DIR)/include -I$(UCN_DIR)/include/classes -I$(GSL)/include -I$(BOOST) -c $< -o $@
