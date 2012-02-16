CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project (ucnsim)

# Override the output location for all executables and libs
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

# Root and GSL do not come with CMake, use our local copies
set(CMAKE_MODULE_PATH "$ENV{UCN_DIR}/cmake_modules")

# Set Build type
set(CMAKE_BUILD_TYPE debug)

# Set Boost options
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# Find packages required by thi project
find_package(ROOT REQUIRED)
find_package(GSL REQUIRED)
find_package(Boost 1.46 REQUIRED COMPONENTS program_options)

# Store the path to the include directory
set(UCNLIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

# Set the compiler inclusion directories
set(INCLUDE_DIRECTORIES ${UCNLIB_INCLUDE_DIR}
                        "${UCNLIB_INCLUDE_DIR}/classes"
                        ${ROOT_INCLUDE_DIR}
                        ${GSL_INCLUDE_DIR}
                        ${Boost_INCLUDE_DIRS})
include_directories( ${INCLUDE_DIRECTORIES} )
link_directories( ${ROOT_LIBRARY_DIR} ${GSL_LINK_DIRECTORIES} ${Boost_LIBRARY_DIRS})

# Save the location of LinkDef.h;
set(UCNLIB_LINKDEF "${CMAKE_CURRENT_LIST_DIR}/include/LinkDef.h")

# Add the CMakeLists.txt from the src directory
add_subdirectory(src)
