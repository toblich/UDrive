#!/bin/bash

#Parámetros:
#-t      Compila tests
#-r      Incluye rocksdb
#-c      Evalúa coverage (implica -t)
#-x      Ejecuta el servidor (implica -r)



export INCLUDE=include
export LIB=/usr/lib

ARGS=$(getopt -o rtcx -l "rocksdb,test,coverage,execute" -- "$@")
eval set -- "$ARGS"

rocksdb=OFF
test=OFF
coverage=OFF
execute=OFF

while true; do
	case "$1" in 
		-r|--rocksdb)
			rocksdb=ON;
			;;
		-t|--test)
			test=ON;
			;;
		-c|--coverage)
			coverage=ON;
			test=ON;
			;;
		-x|--execute)
			rocksdb=ON
			execute=ON;
			;;
		--)
			break;
			;;
	esac
	shift	# avanza en la lista de parametros
done	 

if [[ ! -d files ]]; then
	mkdir files
fi

if [[ ! -d db ]]; then
	mkdir db
fi

if [[ -d build ]]; then
	rm -r build
fi

mkdir build
cd build
mkdir db
mkdir files
cmake -Dtest=$test -Drocksdb=$rocksdb -Dcoverage=$coverage ../

if [[ $coverage == 'ON' ]]; then
	make coverage
else if [[ $test == 'ON' ]]; then	# coverage de por si incluye y corre test
		make unitTest
		cd ..
		./build/unitTest
		cd -
	fi
fi

if [[ $rocksdb == 'ON' ]]; then
	make udrive
	if [[ $execute == 'ON' ]]; then
		cd ..
		./build/udrive
		cd -
	fi
fi

cd ..	# sale de /build
