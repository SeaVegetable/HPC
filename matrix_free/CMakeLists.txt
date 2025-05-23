cmake_minimum_required(VERSION 3.10)

project(EigenvalueProblem)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0074 NEW)
ENDIF(COMMAND cmake_policy)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# OpenMP
set(OpenMP_ROOT "/opt/homebrew/Cellar/libomp/17.0.3/")
find_package(OpenMP REQUIRED)

# Add your source files here
set(EIGEN3_INCLUDE_DIR "$ENV{HOME}/lib/eigen-3.4.0")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

message(STATUS "EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
message(STATUS "INCLUDE_DIR: ${INCLUDE_DIR}")
message(STATUS "SOURCE_DIR: ${SOURCE_DIR}")

# Include directories
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${INCLUDE_DIR})

# Add source files
set(SOURCES ${SOURCE_DIR}/BSplineBasis.cpp
            ${SOURCE_DIR}/ControlPointGenerator.cpp
            ${SOURCE_DIR}/IENGenerator.cpp
            ${SOURCE_DIR}/IDGenerator.cpp
            ${SOURCE_DIR}/NURBSExtractionGenerator.cpp
            ${SOURCE_DIR}/Partition.cpp
            ${SOURCE_DIR}/FileManager.cpp
            ${SOURCE_DIR}/BernsteinBasis.cpp
            ${SOURCE_DIR}/Element.cpp
            ${SOURCE_DIR}/RefElement.cpp
            ${SOURCE_DIR}/QuadraturePoint.cpp
            ${SOURCE_DIR}/LocalAssembly.cpp
            ${SOURCE_DIR}/GlobalAssembly.cpp
            ${SOURCE_DIR}/AbscissaeGenerator.cpp
            ${SOURCE_DIR}/ElementFEM.cpp
            ${SOURCE_DIR}/GlobalAssemblyMF.cpp
            ${SOURCE_DIR}/LocalAssemblyMF.cpp
            ${SOURCE_DIR}/LocalAssemblyMFSF.cpp
            ${SOURCE_DIR}/ElementMF.cpp
            ${SOURCE_DIR}/ElementMFSF.cpp)

# Find PETSc
set(PETSC_DIR $ENV{HOME}/lib/petsc-3.18.6-opt)
set(PETSC_ARCH .)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/module")
find_package(PETSc REQUIRED)
message(STATUS "PETSC_INC: ${PETSC_INC}")
message(STATUS "PETSC_LIB: ${PETSC_LIB}")

set(SLEPC_DIR $ENV{HOME}/lib/slepc-3.18.3-opt)
find_package(SLEPc REQUIRED)

set(CMAKE_C_COMPILER  /opt/homebrew/opt/mpich/bin/mpicc)
set(CMAKE_CXX_COMPILER /opt/homebrew/opt/mpich/bin/mpicxx)

include_directories(${PETSC_INC})
include_directories(${SLEPC_INC})
message(STATUS "SLEPC_INC: ${SLEPC_INC}")
message(STATUS "SLEPC_LIB: ${SLEPC_LIB}")

add_library(eigenvalue_problem_lib ${SOURCES})

add_executable(preprocess preprocess.cpp)
target_link_libraries(preprocess eigenvalue_problem_lib)

add_executable(preprocess_fem preprocess_fem.cpp)
target_link_libraries(preprocess_fem eigenvalue_problem_lib)

add_executable(iter_num_fem iteration_number_fempc.cpp)
target_link_libraries(iter_num_fem eigenvalue_problem_lib ${PETSC_LIB} ${SLEPC_LIB})

add_executable(iter_num_fem_mf iteration_number_fempc_mf.cpp)
target_link_libraries(iter_num_fem_mf eigenvalue_problem_lib ${PETSC_LIB})

add_executable(iter_num_fem_mfsf iteration_number_fempc_mfsf.cpp)
target_link_libraries(iter_num_fem_mfsf eigenvalue_problem_lib ${PETSC_LIB})

if(OPENMP_CXX_FOUND)
  set_target_properties( eigenvalue_problem_lib PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP" )
  set_target_properties( preprocess PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP" )
  set_target_properties( preprocess_fem PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP" )
  target_include_directories( eigenvalue_problem_lib PRIVATE ${OpenMP_CXX_INCLUDE_DIR} )
  target_include_directories( preprocess PRIVATE ${OpenMP_CXX_INCLUDE_DIR} )
  target_include_directories( preprocess_fem PRIVATE ${OpenMP_CXX_INCLUDE_DIR} )
  target_link_libraries( preprocess ${OpenMP_CXX_LIBRARIES} )
  target_link_libraries( preprocess_fem ${OpenMP_CXX_LIBRARIES} )
endif()