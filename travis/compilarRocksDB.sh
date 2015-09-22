if [ ! -d rocksdb-master ]; then
	wget https://github.com/facebook/rocksdb/archive/master.zip
	unzip master.zip
	rm master.zip
	cd rocksdb-master/
	make static_lib
	#echo RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
	#pwd
	#sudo mkdir /rocksdb 
	#sudo cp librocksdb.a /rocksdb/librocksdb.a
	#sudo cp -r include /rocksdb/include
	cd ..
else
	echo Usa rocksdb cacheado
fi
