wget https://github.com/facebook/rocksdb/archive/master.zip
unzip master.zip
rm master.zip
cd rocksdb-master/
make && make install
cd ..