
# add_subdirectory(classes)

set(UCNLIB_SOURCES  Algorithms.cxx classes/BoolNode.cxx
                    classes/BounceData.cxx classes/Box.cxx
                    classes/Clock.cxx classes/CompositeShape.cxx
                    classes/ConfigFile.cxx classes/Data.cxx
                    classes/ElecField.cxx classes/ElecFieldArray.cxx
                    classes/Element.cxx classes/Experiment.cxx
                    classes/Field.cxx classes/FieldArray.cxx
                    classes/FieldData.cxx classes/FieldManager.cxx
                    classes/FieldMap.cxx classes/FieldVertex.cxx
                    classes/FileParser.cxx classes/GravField.cxx
                    classes/InitialConfig.cxx classes/KDTree.cxx
                    classes/KDTreeNode.cxx classes/MagField.cxx
                    classes/MagFieldArray.cxx classes/Material.cxx
                    classes/Observable.cxx classes/Observer.cxx
                    classes/Parabola.cxx classes/ParabolicMagField.cxx
                    classes/Particle.cxx classes/Point.cxx
                    classes/Polynomial.cxx classes/Run.cxx
                    classes/RunConfig.cxx classes/Spin.cxx
                    classes/SpinData.cxx classes/State.cxx
                    classes/Track.cxx classes/Tube.cxx
                    classes/UniformElecField.cxx
                    classes/UniformMagField.cxx classes/VertexStack.cxx
                    classes/Volume.cxx DataAnalysis.cxx Materials.cxx )

set(UCNLIB_HEADER_NAMES   Algorithms.h classes/BoolNode.h
                          classes/BounceData.h classes/Box.h
                          classes/Clock.h classes/CompositeShape.h
                          classes/ConfigFile.h classes/Data.h
                          classes/ElecField.h classes/ElecFieldArray.h
                          classes/Element.h classes/Experiment.h
                          classes/Field.h classes/FieldArray.h
                          classes/FieldData.h classes/FieldManager.h
                          classes/FieldMap.h classes/FieldVertex.h
                          classes/FileParser.h classes/GravField.h
                          classes/InitialConfig.h classes/KDTree.h
                          classes/KDTreeNode.h classes/MagField.h
                          classes/MagFieldArray.h classes/Material.h
                          classes/Observable.h classes/Observer.h
                          classes/Parabola.h classes/ParabolicMagField.h
                          classes/Particle.h classes/Point.h
                          classes/Polynomial.h classes/ProgressBar.h
                          classes/Run.h classes/RunConfig.h
                          classes/Spin.h classes/SpinData.h
                          classes/State.h classes/Track.h classes/Tube.h
                          classes/UniformElecField.h
                          classes/UniformMagField.h classes/VertexStack.h
                          classes/Volume.h Constants.h DataAnalysis.h
                          DataFileHierarchy.h GeomParameters.h
                          Materials.h Units.h )

set(UCNLIB_DICT_HEADERS   classes/Box.h classes/Tube.h classes/Material.h
                          classes/GravField.h classes/Particle.h
                          classes/State.h classes/Spin.h
                          classes/FileParser.h classes/Polynomial.h
                          classes/Parabola.h classes/Experiment.h
                          classes/Data.h classes/Field.h
                          classes/MagField.h classes/UniformMagField.h
                          classes/ParabolicMagField.h classes/Run.h
                          classes/FieldManager.h classes/ConfigFile.h
                          classes/CompositeShape.h classes/BoolNode.h
                          classes/Volume.h classes/Element.h
                          classes/FieldArray.h classes/MagFieldArray.h
                          classes/ElecField.h classes/InitialConfig.h
                          classes/RunConfig.h classes/Observer.h
                          classes/Track.h classes/FieldMap.h
                          classes/KDTree.h classes/KDTreeNode.h
                          classes/FieldVertex.h classes/VertexStack.h
                          classes/Point.h classes/Observable.h
                          classes/Clock.h classes/UniformElecField.h
                          classes/ElecFieldArray.h Algorithms.h
                          Materials.h DataAnalysis.h )
                        
# Append the source directory to each of these, so we have a direct link
# This is because the project layout is a bit weird and I don't know the
# best practices to fix it
foreach(HEADERFILE ${UCNLIB_DICT_HEADERS})
  list(APPEND UCNLIB_DICT_HEADERS_LONG
              ${UCNLIB_INCLUDE_DIR}/${HEADERFILE} )
endforeach()

foreach(HEADERFILE ${UCNLIB_HEADER_NAMES})
  list(APPEND UCNLIB_HEADER_NAMES_LONG
              ${UCNLIB_INCLUDE_DIR}/${HEADERFILE} )
endforeach()
# # The same for sources. This is probably a horrible way of doing it
# foreach(SOURCEFILE ${UCNLIB_SOURCES})
#   list(APPEND UCNLIB_SOURCES_SRC
#               "classes/${SOURCEFILE}" )
# endforeach()

message(STATUS "Cur SD: ${CMAKE_CURRENT_SOURCE_DIR}")
# 
# # Generate the root dictionary for all of these
set( UCNLIB_DICT UCNLibDict.cxx )
ROOT_GENERATE_DICTIONARY("${UCNLIB_DICT_HEADERS_LONG}"
                        "${UCNLIB_LINKDEF}"
                        "${UCNLIB_DICT}"
                        "${INCLUDE_DIRECTORIES}")

# Add the library. The header files aren't required, however doing so
# lets the IDE's know that they are related
add_library(UCNLib  SHARED
                    ${UCNLIB_SOURCES}
                    ${UCNLIB_DICT}
                    ${UCNLIB_HEADER_NAMES_LONG} )

set (ROOT_CUSTOM_LIBRARIES Geom RGL Ged)
set (UCNLIB_LIBRARIES ${GSL_LIBRARIES} ${ROOT_CUSTOM_LIBRARIES} ${ROOT_LIBRARIES})

target_link_libraries(UCNLib ${UCNLIB_LIBRARIES} )
target_link_libraries(UCNLib  )

add_subdirectory(programs)