#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_

#include <string>
#include <iostream>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

using namespace rocksdb;

std::string DBPath = "DB/basededatos";

class BaseDeDatos {

private:
	  DB* db;
	  Options options;

public:
	BaseDeDatos();
	~BaseDeDatos();
	void put(std::string key, std::string value);
	std::string get(std::string key);

};

#endif /* BASEDEDATOS_H_ */
