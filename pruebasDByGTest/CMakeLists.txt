cmake_minimum_required(VERSION 2.8)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(testRocks "Build all tests." OFF) # Makes boolean 'test' available.

project(hello_world)

set(SOURCE_EXE main.cpp)
add_executable(main ${SOURCE_EXE})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAG} -g -std=c++11 -pthread")

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

target_link_libraries(main librocksdb libz libsnappy libbz2 libgflags)

if(test)
	enable_testing()

	set (SOURCE_TEST Addition.cpp Multiply.cpp Addition_Test.cpp Multiply_Test.cpp)
	add_executable(unitTest ${SOURCE_TEST})

	find_package(GTest REQUIRED)
	include_directories(/usr/include/gtest)

	include_directories(libgtest_main)
	add_library(libgtest_main STATIC IMPORTED)
	set_property(TARGET libgtest_main PROPERTY IMPORTED_LOCATION /usr/lib/libgtest_main.a)

	include_directories(libgtest)
	add_library(libgtest STATIC IMPORTED)
	set_property(TARGET libgtest PROPERTY IMPORTED_LOCATION /usr/lib/libgtest.a)

	target_link_libraries(unitTest libgtest libgtest_main libsnappy)
endif()
