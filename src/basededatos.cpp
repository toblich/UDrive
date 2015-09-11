#include "basededatos.h"

BaseDeDatos::BaseDeDatos(){
	Logger logger;
	// Para optimizar RocksDB.
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	options.create_if_missing = true;

	Status s = DB::Open(options, path, &db);
	if(!s.ok()) {
		logger.loggear("No se pudo crear la Base de Datos." + s.ToString(), ERROR);
	} else {
		logger.loggear("Se creo satisfactoriamente la Base de Datos.", INFO);
	}
}

BaseDeDatos::~BaseDeDatos(){
	Logger logger;
	delete db;
	logger.loggear("Se elimino la Base de Datos.", INFO);
}

bool BaseDeDatos::put(std::string key, std::string value) {
	Logger logger;
	std::string valor;
	Status s = db->Get(ReadOptions(), key, &valor);
	if(s.IsNotFound()){
		s = db->Put(WriteOptions(), key, value);
		if(s.ok()){
			logger.loggear("Se inserto satisfactoriamente la key (" + key + ") con el dato (" + value + ") en la base de datos." , TRACE);
			return true;
		} else {
			return false;
		}
	} else {
		logger.loggear("Se quiso insertar la key (" + key + ") con el dato (" + value + ") en la base de datos, pero ya se encontraba una key igual." , TRACE);
//		std::cout << "La clave " << key << " ya existe, por favor use el metodo modify" << std::endl;
	}
	return false;

}

std::string BaseDeDatos::get(std::string key) {
	Logger logger;
	std::string value;
	Status s = db->Get(ReadOptions(), key, &value);
//	if (!s.ok()) std::cout << "Get: " << s.ToString() << std::endl;
	if (s.IsNotFound()) {
		logger.loggear("No se encontro la clave (" + key + ") en la base de datos." , TRACE);
//		std::cout << "No se encontro la clave " << key << " en la base de datos." << std::endl;
		return "";
	} else {
		logger.loggear("Se obtuvo el valor: (" + value + ") de la base de datos, correspondiente a la clave (" + key + ")." , TRACE);
		return value;
	}
}

void BaseDeDatos::erase(std::string key) {
	Logger logger;
	Status s = db->Delete(WriteOptions(), key);
	logger.loggear("Se borro la clave (" + key + ") de la base de datos." , TRACE);
//	if (!s.ok()) std::cout << "Erase: " << s.ToString() << std::endl;
}

void BaseDeDatos::modify(std::string key, std::string value) {
	Logger logger;
	std::string valor;
	Status s = db->Get(ReadOptions(), key, &valor);
//	if(!s.ok()) std::cout << s.ToString() << std::endl;
	if(s.IsNotFound()) {
		logger.loggear("No se encontra la clave (" + key + ") en la base de dattos. Se intentaba modificar el valor (" + valor + ")." , TRACE);
//		std::cout << "No se encuentra la key " << key << ", por favor use el metodo put." << std::endl;
	} else {
		s = db->Put(WriteOptions(), key, value);
		logger.loggear("Se modifico la clave (" + key + "). Se cambio el valor (" + valor + ") por el valor: value " + value + "." , TRACE);
	}
}

void BaseDeDatos::deleteBD() {
	system("exec rm -r DB");
	Logger logger;
	logger.loggear("Se eliminaron los datos persistentes de la base de datos." , TRACE);

}
