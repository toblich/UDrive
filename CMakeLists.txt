cmake_minimum_required(VERSION 2.8)

# Optiones. Pasar valor con 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.
option(rocksdb "Build with RocksDB." ON) 

project(UDrive)

#Incluyo las librerias de headers
include_directories(include/)
file(GLOB INCLUDES "include/*" )

#Agrego todos los archivos de SOURCE
file(GLOB SOURCE "src/*" )



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAG} -g -std=c++11 -pthread")

if (rocksdb)
	add_executable(udrive ${SOURCE} ${INCLUDES})
	
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
	include_directories(include/)

	set(PROJ ${CMAKE_CURRENT_LIST_DIR})
	
	#Agrego todos los archivos de TEST
	file(GLOB SOURCE_TEST "test/*" )

	file(GLOB SOURCE2 "src/*" )
	list(REMOVE_ITEM SOURCE2 ${PROJ}/src/main.cpp)

	if (NOT rocksdb)
		list(REMOVE_ITEM SOURCE2 ${PROJ}/src/basededatos.cpp)
		list(REMOVE_ITEM SOURCE_TEST ${PROJ}/test/basededatos_test.cpp)
	endif()

	add_executable(unitTest ${SOURCE2} ${SOURCE_TEST} ${INCLUDES})

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
