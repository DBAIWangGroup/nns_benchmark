##########################################################################################
# CMake build script for NearestSearch.
#
##########################################################################################

#let all libraries be static, not shared
OPTION(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

##########################################################################################
# lets start describing our project.
project (NearestSearch CXX C)
cmake_minimum_required(VERSION 3.2)

find_package(Boost 1.58.0 COMPONENTS program_options serialization system filesystem thread REQUIRED)

IF (UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -DMKL_ILP64 -m64")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fopenmp -DMKL_ILP64 -m64")
ENDIF (UNIX)

IF (NOT DEFINED CMAKE_CURRENT_LIST_DIR)
    SET(CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
ENDIF (NOT DEFINED CMAKE_CURRENT_LIST_DIR)

MESSAGE("current dir: ${CMAKE_CURRENT_LIST_DIR}")
SET (Source_Path ${CMAKE_CURRENT_LIST_DIR})

MESSAGE ("PROJECT_BINARY_DIR " ${PROJECT_BINARY_DIR})

# ADD BOOSTDIR
IF (UNIX)
SET(BOOST_DIR /home/yzhang4/HD/home/liwen/Downloads/software/boost_1_58_0)
SET(BOOST_LIB /home/yzhang4/HD/home/liwen/Downloads/software/boost1.58/lib)
ENDIF (UNIX)
IF(WIN32)
SET(BOOST_DIR C:/Users/arbabenko/Soft/boost_1_47_0)
SET(BOOST_LIB C:/Users/arbabenko/Soft/boost_1_47_0/lib/x64/lib)
ENDIF(WIN32)

# ADD BLAS
IF (UNIX)
SET(BLAS_DIR /opt/intel/composer_xe_2013/mkl/include)
SET(BLAS_LIB /opt/intel/composer_xe_2013/mkl/lib/intel64 
             /opt/intel/composer_xe_2013/compiler/lib/intel64)
ENDIF (UNIX)
IF(WIN32)
SET(BLAS_DIR "C:/Program Files (x86)/Intel/Composer XE 2011 SP1/mkl/include")
SET(BLAS_LIB "C:/Program Files (x86)/Intel/Composer XE 2011 SP1/mkl/lib/intel64"
             "C:/Program Files (x86)/Intel/Composer XE 2011 SP1/compiler/lib/intel64")
ENDIF(WIN32)



##########################################################################################
INCLUDE (${Source_Path}/NearestSearch.cmake NO_POLICY_SCOPE)
INCLUDE_DIRECTORIES(${BOOST_DIR})
INCLUDE_DIRECTORIES(${BLAS_DIR})
LINK_DIRECTORIES(${BOOST_LIB})
LINK_DIRECTORIES(${BLAS_LIB})
INCLUDE_DIRECTORIES(${Source_Path})
ADD_LIBRARY (nearest_search_lib STATIC ${NEAREST_ALL_CC})

##########################################################################################
INCLUDE (${Source_Path}/IndexerLauncher.cmake NO_POLICY_SCOPE)
INCLUDE_DIRECTORIES(${BOOST_DIR})
INCLUDE_DIRECTORIES(${BLAS_DIR})
LINK_DIRECTORIES(${BOOST_LIB})
LINK_DIRECTORIES(${BLAS_LIB})
INCLUDE_DIRECTORIES(${Source_Path})
ADD_EXECUTABLE (indexer_launcher ${IndexerLauncher})
TARGET_LINK_LIBRARIES (indexer_launcher nearest_search_lib)
target_link_libraries( indexer_launcher ${Boost_LIBRARIES} )
IF (UNIX)
TARGET_LINK_LIBRARIES (indexer_launcher  libmkl_intel_ilp64.a libmkl_gnu_thread.a libmkl_core.a dl pthread m)
ENDIF (UNIX)
IF(WIN32)
TARGET_LINK_LIBRARIES (indexer_launcher  mkl_intel_lp64.lib mkl_intel_thread.lib mkl_core.lib libiomp5md.lib)
ENDIF(WIN32)
  
##########################################################################################
INCLUDE (${Source_Path}/SearcherTester.cmake NO_POLICY_SCOPE)
INCLUDE_DIRECTORIES(${BOOST_DIR})
INCLUDE_DIRECTORIES(${BLAS_DIR})
LINK_DIRECTORIES(${BOOST_LIB})
LINK_DIRECTORIES(${BLAS_LIB})
INCLUDE_DIRECTORIES(${Source_Path})
ADD_EXECUTABLE (searcher_tester ${SearchTester})
TARGET_LINK_LIBRARIES (searcher_tester  nearest_search_lib)
target_link_libraries( searcher_tester ${Boost_LIBRARIES} )
IF (UNIX)
TARGET_LINK_LIBRARIES (searcher_tester libmkl_intel_ilp64.a libmkl_gnu_thread.a libmkl_core.a dl pthread m)
ENDIF (UNIX)
IF(WIN32)
TARGET_LINK_LIBRARIES (searcher_tester mkl_intel_lp64.lib mkl_intel_thread.lib mkl_core.lib libiomp5md.lib)
ENDIF(WIN32)


