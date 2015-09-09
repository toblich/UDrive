#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_

#include <string>
#include <iostream>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>



std::string kDBPath = "DB/basededatos";

class BaseDeDatos {

private:
	  rocksdb::DB* db;
	  rocksdb::Options options;

public:
	BaseDeDatos();
//	{
//		  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
//		  options.IncreaseParallelism();
//		  options.OptimizeLevelStyleCompaction();
//		  // create the DB if it's not already present
//		  options.create_if_missing = true;
//
//		  // open DB
//		  Status s = DB::Open(options, kDBPath, &db);
//		  std::cout << s.ToString() << std::endl;
//		  assert(s.ok());
//	}
	~BaseDeDatos();

};

#endif /* BASEDEDATOS_H_ */
