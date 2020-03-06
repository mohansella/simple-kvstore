cmake_minimum_required(VERSION 3.11)

project(SimpleKVStore)

#------------- compiler adjustments ---------------
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  #useful in linux environment when combining multiple libs to single so

if(MSVC)
	set(CMAKE_C_FLAGS_DEBUG				"/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_C_FLAGS_MINSIZEREL		"/MT /O1 /Ob1 /D NDEBUG")
	set(CMAKE_C_FLAGS_RELEASE			"/MT /O2 /Ob2 /D NDEBUG")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO	"/MT /Zi /O2 /Ob1 /D NDEBUG")

	set(CMAKE_CXX_FLAGS_DEBUG 			"/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_CXX_FLAGS_MINSIZEREL 		"/MT /O1 /Ob1 /D NDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE 		"/MT /O2 /Ob2 /D NDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"/MT /Zi /O2 /Ob1 /D NDEBUG")
endif()
#------------- compiler adjustments end ---------------


#--------- Package Manager Adjustment -------------
#prepare conan
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
				 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

#resolve dependencies
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD missing BUILD_TYPE ${CONAN_BUILD_TYPE})

#find locations
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
#--------- Package Manager Adjustment --------------


#--------------- CMAKE install configuration --------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#local dev install to install/ directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH
		"Install path prefix, prepended onto install directories." FORCE )
endif()
#--------------- CMAKE install configuration end -------------------


#--------------- BOOST configuration -----------------------
if(MSVC)
	#boost process is used for set/get environment variables which needs below def
	add_definitions(-DBOOST_USE_WINDOWS_H)

	#another environment audjustment
	add_definitions(-DBOOST_NO_ANSI_APIS)
endif()
#-------------- BOOST configuration end --------------------


add_subdirectory(source/lib)
add_subdirectory(source/test)
add_subdirectory(source/app)