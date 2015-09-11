#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_

#include "logger.h"

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

//using namespace rocksdb;

using rocksdb::DB;
using rocksdb::Options;
using rocksdb::Status;
using rocksdb::WriteOptions;
using rocksdb::ReadOptions;

const std::string path = "DB";

class BaseDeDatos {

private:
	  DB* db;
	  Options options;

public:
	BaseDeDatos();
	~BaseDeDatos();
	bool put(std::string key, std::string value);
	std::string get(std::string key);
	void erase(std::string key);
	void modify(std::string key, std::string value);
	void deleteBD();

};

#endif /* BASEDEDATOS_H_ */
