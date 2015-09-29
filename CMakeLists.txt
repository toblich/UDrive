cmake_minimum_required(VERSION 2.8)

# Optiones. Pasar valor con 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.
option(rocksdb "Build with RocksDB." ON) 
option(coverage "Build coverage." OFF)  

project(UDrive)

#Incluyo las librerias de headers
include_directories(include/ include/db/)

#Agrego todos los archivos de SOURCE
file(GLOB SOURCE "src/*" "src/db/*")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -pthread")


if(coverage)
	set(test ON)
	set(rocksdb OFF)
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-arcs -ftest-coverage")

	FIND_PROGRAM( GCOV_PATH gcov )
	FIND_PROGRAM( LCOV_PATH lcov )
	FIND_PROGRAM( GENHTML_PATH genhtml )
	FIND_PROGRAM( GCOVR_PATH gcovr PATHS ${CMAKE_SOURCE_DIR}/test)

	set(_outputname coverage)
	set(_targetname coverage)

	ADD_CUSTOM_TARGET(${_targetname}

		# Cleanup lcov
		${LCOV_PATH} --directory . --zerocounters

		# Run tests
		COMMAND unitTest

		# Capturing lcov counters and generating report
		COMMAND ${LCOV_PATH} --directory . --capture --output-file ${_outputname}.info
		COMMAND ${LCOV_PATH} --remove ${_outputname}.info 'test/*' '/usr/*' 'src/mongoose.c' '*.h' 'src/jsoncpp.cpp' 'src/logger.cpp' --output-file ${_outputname}.info.cleaned
		COMMAND ${GENHTML_PATH} -o ${_outputname} ${_outputname}.info.cleaned
		COMMAND ${CMAKE_COMMAND} -E remove ${_outputname}.info ${_outputname}.info.cleaned
	)
endif()


if (rocksdb)
	add_executable(udrive ${SOURCE})
	
	#Agrego librerias externas
	include_directories(librocksdb)
	add_library(librocksdb STATIC IMPORTED)
	set_property(TARGET librocksdb PROPERTY IMPORTED_LOCATION /usr/local/lib/librocksdb.a)

	include_directories(libz)
	add_library(libz STATIC IMPORTED)
	set_property(TARGET libz PROPERTY IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libz.a)

	include_directories(libsnappy)
	add_library(libsnappy STATIC IMPORTED)
	set_property(TARGET libsnappy PROPERTY IMPORTED_LOCATION /usr/lib/libsnappy.a)

	include_directories(libbz2)
	add_library(libbz2 STATIC IMPORTED)
	set_property(TARGET libbz2 PROPERTY IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libbz2.a)

	include_directories(libgflags)
	add_library(libgflags STATIC IMPORTED)
	set_property(TARGET libgflags PROPERTY IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libgflags.a)

	#Linkeo librerias externas al ejecutable
	target_link_libraries(udrive librocksdb libz libsnappy libbz2 libgflags)
endif()


if(test)
	enable_testing()

	#Incluyo las librerias de headers
	include_directories(include/ include/db/)

	set(PROJ ${CMAKE_CURRENT_LIST_DIR})
	
	#Agrego todos los archivos de TEST
	file(GLOB SOURCE_TEST "test/*" "test/db/*")

	file(GLOB SOURCE2 "src/*" "src/db/*")
	list(REMOVE_ITEM SOURCE2 ${PROJ}/src/main.cpp)

	if (NOT rocksdb)
		list(REMOVE_ITEM SOURCE2 ${PROJ}/src/db/basededatos.cpp)
		list(REMOVE_ITEM SOURCE_TEST ${PROJ}/test/db/basededatos_test.cpp)
	endif()

	add_executable(unitTest ${SOURCE2} ${SOURCE_TEST})

	#Agrego librerias externas
	find_package(GTest REQUIRED)
	include_directories($ENV{INCLUDE}/gtest)

	include_directories(libgtest_main)
	add_library(libgtest_main STATIC IMPORTED)
	set_property(TARGET libgtest_main PROPERTY IMPORTED_LOCATION $ENV{LIB}/libgtest_main.a)

	include_directories(libgtest)
	add_library(libgtest STATIC IMPORTED)
	set_property(TARGET libgtest PROPERTY IMPORTED_LOCATION $ENV{LIB}/libgtest.a)

	#Linkeo librerias externas al ejecutable de test
	if (NOT rocksdb)
		target_link_libraries(unitTest libgtest libgtest_main)
	else()
		target_link_libraries(unitTest libgtest libgtest_main librocksdb libz libsnappy libbz2 libgflags)
	endif()
endif()
