#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_

#include "logger.h"
#include "excepciones.h"

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
	BaseDeDatos(string nombre);
	~BaseDeDatos();
	bool put(std::string key, std::string value);	/* Throws: runtime_error */
	std::string get(std::string key);				/* Throws: runtime_error, KeyNotFound */
	void erase(std::string key);					/* Throws: runtime_error, KeyNotFound */
	void modify(std::string key, std::string value);/* Throws: runtime_error, KeyNotFound */
	void deleteBD();

};

#endif /* BASEDEDATOS_H_ */
