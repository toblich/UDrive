#!/bin/bash

#Parámetros:
#-t      Compila tests
#-r      Incluye rocksdb
#-c      Evalúa coverage (implica -t)
#-x      Ejecuta el servidor (implica -r)



export INCLUDE=include
export LIB=/usr/lib

ARGS=$(getopt -o hrtcx -l "help,rocksdb,test,coverage,execute" -- "$@")
eval set -- "$ARGS"

rocksdb=OFF
test=OFF
coverage=OFF
execute=OFF

while true; do
	case "$1" in 
		-h|--help)
			echo "
-h, --help	Muestra esta ayuda y finaliza
-r, --rocksdb	Compila udrive con RocksDB
-t, --test	Compila y ejecuta tests unitarios (de C++)
-x, --execute	Compila y ejecuta udrive [implica --rocksdb]
-c, --coverage	Compila tests y udrive sin RocksDB y ejecuta tests
		(C++ y Python), generando reporte de coverage
		[anula --rocksdb e implica --test]
";
			exit 0;
			;;
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

# if [[ ! -d files ]]; then
# 	mkdir files
# fi

# if [[ ! -d db ]]; then
# 	mkdir db
# fi

if [[ -d build ]]; then
	rm -r build
fi

mkdir build
cp "default.jpg" "build"
cd build
mkdir db
mkdir files
cmake -Dtest=$test -Drocksdb=$rocksdb -Dcoverage=$coverage ../

if [[ $coverage == 'ON' ]]; then
	make coverage
else if [[ $test == 'ON' ]]; then	# coverage de por si incluye y corre test
		make unitTest
		# cd ..
		# ./build/unitTest
		# cd -
		./unitTest
	fi
fi

if [[ $rocksdb == 'ON' ]]; then
	make udrive
	if [[ $execute == 'ON' ]]; then
		# cd ..
		# ./build/udrive
		# cd -
		./udrive
	fi
fi

cd ..	# sale de /build
