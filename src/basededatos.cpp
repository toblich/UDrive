#include "basededatos.h"

using std::string;

BaseDeDatos::BaseDeDatos(){
	Logger logger;
	// Para optimizar RocksDB.
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	options.create_if_missing = true;

	Status s = DB::Open(options, path, &db);
	if(!s.ok()) {
		logger.loggear("No se pudo crear la Base de Datos con path " + path + ". " + s.ToString(), ERROR);
		throw runtime_error("No se pudo crear la base de datos con path: " + path);
	} else {
		logger.loggear("Se creo satisfactoriamente la Base de Datos con path " + path, INFO);
	}
}

BaseDeDatos::~BaseDeDatos(){
	Logger logger;
	delete db;
	logger.loggear("Se elimino la Base de Datos.", INFO);
}

bool BaseDeDatos::put(string key, string value) {
	Logger logger;
	string valor;
	Status s = db->Get(ReadOptions(), key, &valor);
	if(s.IsNotFound()){
		s = db->Put(WriteOptions(), key, value);
		if(s.ok()){
			logger.loggear("Se inserto satisfactoriamente la key (" + key + ") con el dato (" + value + ") en la base de datos." , TRACE);
			return true;
		} else {
			logger.loggear("No se pudo insertar " + key + " en la base de datos", ERROR);
			throw runtime_error("No se pudo insertar " + key + " en la base de datos");
		}
	}
	logger.loggear("Se quiso insertar la key (" + key + ") con el dato (" + value + ") en la base de datos, pero ya se encontraba una key igual." , TRACE);
	return false;
}

string BaseDeDatos::get(string key) {
	Logger logger;
	string value;
	Status s = db->Get(ReadOptions(), key, &value);
	if (s.IsNotFound()) {
		logger.loggear("No se encontro la clave (" + key + ") en la base de datos." , TRACE);
		throw KeyNotFound("get: " + key);
	} else if (not s.ok()) {
		logger.loggear("No se pudo obtener la clave " + key + " de la base de datos", ERROR);
		throw runtime_error("No se pudo obtener la clave " + key + " de la base de datos");
	}
	logger.loggear("Se obtuvo el valor: (" + value + ") de la base de datos, correspondiente a la clave (" + key + ")." , TRACE);
	return value;
}

void BaseDeDatos::erase(string key) {
	Logger logger;
	string value;
	Status s = db->Get(ReadOptions(), key, &value);
	if (s.IsNotFound()) {
		logger.loggear("Se quiso borrar la clave \"" + key + "\" inexistente en la base de datos", WARN);
		throw KeyNotFound("erase: " + key);
	}
	s = db->Delete(WriteOptions(), key);
	if (!s.ok()) {
		std::cout << "Erase: " << s.ToString() << std::endl;
		logger.loggear("No se pudo borrar la clave \"" + key + "\" de la base de datos", ERROR);
		throw runtime_error("erase " + key);
	}
	logger.loggear("Se borro la clave (" + key + ") de la base de datos." , TRACE);
}

void BaseDeDatos::modify(string key, string value) {
	Logger logger;
	string valor;
	Status s = db->Get(ReadOptions(), key, &valor);
	if(s.IsNotFound()) {
		logger.loggear("No se encuentra la clave (" + key + ") en la base de datos. Se intentaba modificar el valor (" + valor + ")." , WARN);
		throw KeyNotFound("modifiy: " + key);
	} else if (not s.ok()){
		logger.loggear("No se pudo modificar el valor en la clave " + key + " de la base de datos", ERROR);
		throw new runtime_error("modify: " + key);
	}
	s = db->Put(WriteOptions(), key, value);
	logger.loggear("Se modifico la clave (" + key + "). Se cambio el valor (" + valor + ") por el valor: value " + value + "." , TRACE);
}

void BaseDeDatos::deleteBD() {
	system("exec rm -r DB");
	Logger logger;
	logger.loggear("Se eliminaron los datos persistentes de la base de datos." , TRACE);

}
