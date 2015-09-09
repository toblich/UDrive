#include "basededatos.h"

BaseDeDatos::BaseDeDatos(){
	// Para optimizar RocksDB.
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();

	options.create_if_missing = true;

	Status s = DB::Open(options, path, &db);
	if(!s.ok()) std::cout << "Constructor: " << s.ToString() << std::endl;
}

BaseDeDatos::~BaseDeDatos(){
	delete db;
}

void BaseDeDatos::put(std::string key, std::string value) {
	Status s = db->Put(WriteOptions(), key, value);
	if (!s.ok()) std::cout << "Put: " << s.ToString() << std::endl;
}

std::string BaseDeDatos::get(std::string key) {
	std::string value;
	Status s = db->Get(ReadOptions(), key, &value);
	if (!s.ok()) std::cout << "Get: " << s.ToString() << std::endl;
	if (s.IsNotFound()) {
		std::cout << "No se encontro la clave " << key << " en la base de datos." << std::endl;
		return "";
	} else return value;
}

void BaseDeDatos::erase(std::string key) {
	Status s = db->Delete(WriteOptions(), key);
	if (!s.ok()) std::cout << "Erase: " << s.ToString() << std::endl;
}
