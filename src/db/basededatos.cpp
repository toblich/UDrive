#include "basededatos.h"

using std::string;

BaseDeDatos::BaseDeDatos() : BaseDeDatos(defaultPath, DEFAULT_DB) {}

BaseDeDatos::BaseDeDatos(string path, string carpeta) {
	this->path = carpeta + "/" + path;
	// Para optimizar RocksDB.
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	options.create_if_missing = true;

	Status s = DB::Open(options, this->path, &db);
	if(!s.ok()) {
		Logger::logError("No se pudo crear la Base de Datos con path " + this->path + ". " + s.ToString());
		throw runtime_error("No se pudo crear la base de datos con path: " + this->path);
	} else {
		Logger::logInfo("Se creo satisfactoriamente la Base de Datos con path " + this->path);
	}
}

BaseDeDatos::~BaseDeDatos(){
	delete db;
	Logger::logInfo("Se elimino la Base de Datos.");
}

bool BaseDeDatos::put(string key, string value) {
	string valor;
	Status s = db->Get(ReadOptions(), key, &valor);
	if(s.IsNotFound()){
		s = db->Put(WriteOptions(), key, value);
		if(s.ok()){
			Logger::logTrace("Se inserto satisfactoriamente la key (" + key + ") con el dato (" + value + ") en la base de datos.");
			return true;
		} else {
			Logger::logError("No se pudo insertar " + key + " en la base de datos");
			throw runtime_error("No se pudo insertar " + key + " en la base de datos");
		}
	}
	Logger::logTrace("Se quiso insertar la key (" + key + ") con el dato (" + value + ") en la base de datos, pero ya se encontraba una key igual.");
	return false;
}

string BaseDeDatos::get(string key) {
	string value;
	Status s = db->Get(ReadOptions(), key, &value);
	if (s.IsNotFound()) {
		Logger::logTrace("No se encontro la clave (" + key + ") en la base de datos.");
		throw KeyNotFound("get: " + key);
	} else if (not s.ok()) {
		Logger::logError("No se pudo obtener la clave " + key + " de la base de datos");
		throw runtime_error("No se pudo obtener la clave " + key + " de la base de datos");
	}
	Logger::logTrace("Se obtuvo el valor: (" + value + ") de la base de datos, correspondiente a la clave (" + key + ").");
	return value;
}

void BaseDeDatos::erase(string key) {
	string value;
	Status s = db->Get(ReadOptions(), key, &value);
	if (s.IsNotFound()) {
		Logger::logWarn("Se quiso borrar la clave \"" + key + "\" inexistente en la base de datos");
		throw KeyNotFound("erase: " + key);
	}
	s = db->Delete(WriteOptions(), key);
	if (!s.ok()) {
		std::cout << "Erase: " << s.ToString() << std::endl;
		Logger::logError("No se pudo borrar la clave \"" + key + "\" de la base de datos");
		throw runtime_error("erase " + key);
	}
	Logger::logTrace("Se borro la clave (" + key + ") de la base de datos.");
}

void BaseDeDatos::modify(string key, string value) {
	string valor;
	Status s = db->Get(ReadOptions(), key, &valor);
	if(s.IsNotFound()) {
		Logger::logWarn("No se encuentra la clave (" + key + ") en la base de datos. Se intentaba modificar el valor (" + valor + ").");
		throw KeyNotFound("modify: " + key);
	} else if (not s.ok()){
		Logger::logError("No se pudo modificar el valor en la clave " + key + " de la base de datos");
		throw runtime_error("modify: " + key);
	}
	s = db->Put(WriteOptions(), key, value);
	Logger::logTrace("Se modifico la clave (" + key + "). Se cambio el valor (" + valor + ") por el valor: value " + value + ".");
}

bool BaseDeDatos::contains(string key) {
	string value;
	Status s = db->Get(ReadOptions(), key, &value);
	return not s.IsNotFound();
}

bool BaseDeDatos::addActionToBatch(const DBAction& action, rocksdb::WriteBatch& writeBatch) {
	switch (action.action) {
	case PUT:
		if (contains(action.key)) return false;
		writeBatch.Put(action.key, action.value);
		break;
	case MODIFY:
		if (not contains(action.key)) return false;
		writeBatch.Put(action.key, action.value);
		break;
	case ERASE:
		if (not contains(action.key)) return false;
		writeBatch.Delete(action.key);
		break;
	default:
		throw new InvalidDBAction("Accion inexistente (existen PUT, MODIFY y ERASE)");
	}
	return true;
}

bool BaseDeDatos::writeBatch(const Batch& batch) {
	WriteBatch writeBatch;
	for (auto &dbAction : batch.getAcciones()) {
		if (not addActionToBatch(dbAction, writeBatch))
			return false;
	}
	Status s = db->Write(WriteOptions(), &writeBatch);
	return s.ok();
}

void BaseDeDatos::deleteBD() {
	string command = "exec rm -r " + path;
	system(command.c_str());
	Logger::logTrace("Se eliminaron los datos persistentes de la base de datos.");
}
