#include "basededatos.h"

using namespace rocksdb;

BaseDeDatos::BaseDeDatos(){
	  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
	  options.IncreaseParallelism();
	  options.OptimizeLevelStyleCompaction();
	  // create the DB if it's not already present
	  options.create_if_missing = true;

	  // open DB
	  Status s = DB::Open(options, kDBPath, &db);
	  std::cout << s.ToString() << std::endl;
	  assert(s.ok());
}

BaseDeDatos::~BaseDeDatos(){
	  delete db;
}
