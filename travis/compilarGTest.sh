if [ ! -d googletest-master ]; then
	wget https://github.com/google/googletest/archive/master.zip
	unzip master.zip
	rm master.zip
	cd googletest-master
	cd googletest

	mkdir build
	cd build
	cmake ../ #-DBUILD_SHARED_LIBS=ON -Dgtest_build_samples=ON -G"Unix Makefiles" ..
	make
	echo GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGg
	pwd
	#cp -r ../include/gtest ~/usr/gtest/include/
	#cp lib*.so ~/usr/gtest/lib
	#cp lib*.a ~/usr/gtest/lib

	cd ..	# sale de mybuild
	cd ..	# sale de googletest
	cd ..	# sale de googletest-master
else
	echo Usa GTest cacheado
fi
