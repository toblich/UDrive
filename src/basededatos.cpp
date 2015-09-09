#include "basededatos.h"

BaseDeDatos::BaseDeDatos(){
	// Optimize RocksDB. This is the easiest way to get RocksDB to perform well
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	// create the DB if it's not already present
	options.create_if_missing = true;

	// open DB
	Status s = DB::Open(options, kDBPath, &db);
	if(!s.ok()) std::cout << s.ToString() << std::endl;
}

BaseDeDatos::~BaseDeDatos(){
	delete db;
}

void BaseDeDatos::put(std::string key, std::string value) {
	Status s = db->Put(WriteOptions(), key, value);
	if(!s.ok()) std::cout << s.ToString() << std::endl;
}

std::string BaseDeDatos::get(std::string key) {
	std::string value;
	Status s = db->Get(ReadOptions(), key, &value);
	if(!s.ok()) std::cout << s.ToString() << std::endl;
	return value;
}
