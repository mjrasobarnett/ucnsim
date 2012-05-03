set(UCNLIB_SOURCES  BoolNode.cpp
                    BounceData.cpp
                    Box.cpp
                    Clock.cpp
                    CompositeShape.cpp
                    ConfigFile.cpp
                    Data.cpp
                    ElecField.cpp
                    ElecFieldArray.cpp
                    Element.cpp
                    Experiment.cpp
                    Field.cpp
                    FieldArray.cpp
                    FieldData.cpp
                    FieldManager.cpp
                    FieldMap.cpp
                    FieldVertex.cpp
                    FileParser.cpp
                    GravField.cpp
                    InitialConfig.cpp
                    KDTree.cpp
                    KDTreeNode.cpp
                    MagField.cpp
                    MagFieldArray.cpp
                    Material.cpp
                    Observable.cpp
                    Observer.cpp
                    Parabola.cpp
                    ParabolicMagField.cpp
                    Particle.cpp
                    Point.cpp
                    Polynomial.cpp
                    Run.cpp
                    RunConfig.cpp
                    Spin.cpp
                    SpinData.cpp
                    State.cpp
                    Track.cpp
                    Tube.cpp
                    UniformElecField.cpp
                    UniformMagField.cpp
                    VertexStack.cpp
                    Volume.cpp
                    ParticleManifest.cpp 
                    TRandom3a.cpp
                    PopulationData.cpp
                    MagFieldDipole.cpp
                    MagFieldLoop.cpp
                    DataAnalysis.cpp
                    Materials.cpp )

set(UCNLIB_HEADER_NAMES   BoolNode.hpp
                          BounceData.hpp
                          Box.hpp
                          Clock.hpp
                          CompositeShape.hpp
                          ConfigFile.hpp
                          Data.hpp
                          ElecField.hpp
                          ElecFieldArray.hpp
                          Element.hpp
                          Experiment.hpp
                          Field.hpp
                          FieldArray.hpp
                          FieldData.hpp
                          FieldManager.hpp
                          FieldMap.hpp
                          FieldVertex.hpp
                          FileParser.hpp
                          GravField.hpp
                          InitialConfig.hpp
                          KDTree.hpp
                          KDTreeNode.hpp
                          MagField.hpp
                          MagFieldArray.hpp
                          Material.hpp
                          Observable.hpp
                          Observer.hpp
                          Parabola.hpp
                          ParabolicMagField.hpp
                          Particle.hpp
                          Point.hpp
                          Polynomial.hpp
                          Run.hpp
                          RunConfig.hpp
                          Spin.hpp
                          SpinData.hpp
                          State.hpp
                          Track.hpp
                          Tube.hpp
                          UniformElecField.hpp
                          UniformMagField.hpp
                          VertexStack.hpp
                          Volume.hpp
                          ParticleManifest.hpp 
                          TRandom3a.hpp
                          PopulationData.hpp
                          MagFieldDipole.hpp
                          MagFieldLoop.hpp
                          Constants.hpp
                          DataAnalysis.hpp
                          ValidStates.hpp
                          GeomParameters.hpp
                          Materials.hpp
                          Units.hpp)

set(UCNLIB_DICT_HEADERS   ${UCNLIB_HEADER_NAMES}
                          "${ALGORITHMS_INCLUDE_DIR}/Algorithms.hpp")
                        
# Generate the root dictionary for all of these
set(UCNLIB_DICT UCNLibDict.cpp)
ROOT_GENERATE_DICTIONARY("${UCNLIB_DICT_HEADERS}"
                         "${UCNLIB_LINKDEF}"
                         "${UCNLIB_DICT}"
                         "${UCNLIB_INCLUDE_DIR}")

# Add the library. The header files aren't required, however doing so
# lets the IDE's know that they are related
add_library(UCN SHARED ${UCNLIB_SOURCES}
                       ${UCNLIB_DICT}
                       ${UCNLIB_HEADER_NAMES} )

set (ROOT_CUSTOM_LIBRARIES Geom RGL Ged)
set (UCNLIB_LIBRARIES ${GSL_LIBRARIES}
                      ${ROOT_CUSTOM_LIBRARIES}
                      ${ROOT_LIBRARIES}
                      ${Boost_LIBRARIES}
                      ${ALGORITHMS_LIBRARY})

target_link_libraries(UCN ${UCNLIB_LIBRARIES} )

add_subdirectory(programs)
