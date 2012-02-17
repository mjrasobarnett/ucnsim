
# add_subdirectory(classes)

set(UCNLIB_SOURCES  Algorithms.cxx
                    BoolNode.cxx
                    BounceData.cxx
                    Box.cxx
                    Clock.cxx
                    CompositeShape.cxx
                    ConfigFile.cxx
                    Data.cxx
                    ElecField.cxx
                    ElecFieldArray.cxx
                    Element.cxx
                    Experiment.cxx
                    Field.cxx
                    FieldArray.cxx
                    FieldData.cxx
                    FieldManager.cxx
                    FieldMap.cxx
                    FieldVertex.cxx
                    FileParser.cxx
                    GravField.cxx
                    InitialConfig.cxx
                    KDTree.cxx
                    KDTreeNode.cxx
                    MagField.cxx
                    MagFieldArray.cxx
                    Material.cxx
                    Observable.cxx
                    Observer.cxx
                    Parabola.cxx
                    ParabolicMagField.cxx
                    Particle.cxx
                    Point.cxx
                    Polynomial.cxx
                    Run.cxx
                    RunConfig.cxx
                    Spin.cxx
                    SpinData.cxx
                    State.cxx
                    Track.cxx
                    Tube.cxx
                    UniformElecField.cxx
                    UniformMagField.cxx
                    VertexStack.cxx
                    Volume.cxx
                    ParticleManifest.cxx 
                    TRandom3a.cxx
                    PopulationData.cxx
                    MagFieldDipole.cxx
                    MagFieldLoop.cxx
                    DataAnalysis.cxx
                    Materials.cxx )

set(UCNLIB_HEADER_NAMES   Algorithms.h
                          BoolNode.h
                          BounceData.h
                          Box.h
                          Clock.h
                          CompositeShape.h
                          ConfigFile.h
                          Data.h
                          ElecField.h
                          ElecFieldArray.h
                          Element.h
                          Experiment.h
                          Field.h
                          FieldArray.h
                          FieldData.h
                          FieldManager.h
                          FieldMap.h
                          FieldVertex.h
                          FileParser.h
                          GravField.h
                          InitialConfig.h
                          KDTree.h
                          KDTreeNode.h
                          MagField.h
                          MagFieldArray.h
                          Material.h
                          Observable.h
                          Observer.h
                          Parabola.h
                          ParabolicMagField.h
                          Particle.h
                          Point.h
                          Polynomial.h
                          Run.h
                          RunConfig.h
                          Spin.h
                          SpinData.h
                          State.h
                          Track.h
                          Tube.h
                          UniformElecField.h
                          UniformMagField.h
                          VertexStack.h
                          Volume.h
                          ParticleManifest.h 
                          TRandom3a.h
                          PopulationData.h
                          MagFieldDipole.h
                          MagFieldLoop.h
                          Constants.h
                          DataAnalysis.h
                          ValidStates.h
                          GeomParameters.h
                          Materials.h
                          Units.h)

set(UCNLIB_DICT_HEADERS   ${UCNLIB_HEADER_NAMES})
                        
# Generate the root dictionary for all of these
set(UCNLIB_DICT UCNLibDict.cxx)
ROOT_GENERATE_DICTIONARY("${UCNLIB_DICT_HEADERS}"
                         "${UCNLIB_LINKDEF}"
                         "${UCNLIB_DICT}"
                         "${UCNLIB_INCLUDE_DIR}")

# Add the library. The header files aren't required, however doing so
# lets the IDE's know that they are related
add_library(UCNLib  SHARED
                    ${UCNLIB_SOURCES}
                    ${UCNLIB_DICT}
                    ${UCNLIB_HEADER_NAMES} )

set (ROOT_CUSTOM_LIBRARIES Geom RGL Ged)
set (UCNLIB_LIBRARIES ${GSL_LIBRARIES} ${ROOT_CUSTOM_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(UCNLib ${UCNLIB_LIBRARIES} )

add_subdirectory(programs)
