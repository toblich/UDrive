#include "mapDB.h"

using namespace std;

MapDB::MapDB() : mapa() {
	// TODO Auto-generated constructor stub
}

MapDB::~MapDB() {
	// TODO Auto-generated destructor stub
}

bool MapDB::put(string key, string value) {
	try {
		mapa.at(key);
	} catch (out_of_range& e) {	// No habia valor con esa clave
		mapa.insert(pair<string, string> (key, value));
		return true;
	}
	return false;
}

string MapDB::get(string key) {
	try {
		return mapa.at(key);
	} catch (out_of_range& e) {
		throw KeyNotFound("mapDB.get: " + key);
	}
}

void MapDB::erase(string key) {
	try {
		mapa.at(key); //aca podria lanzar la excepcion si no existe
		mapa.erase(key);
	} catch (out_of_range& e) {
		throw KeyNotFound("mapDB.erase: " + key);
	}
}

void MapDB::modify(string key, string value) {
	try {
		mapa.at(key) = value;
	} catch (out_of_range& e) {
		throw KeyNotFound("mapDB.modify: " + key);
	}
}

bool MapDB::writeBatch(const Batch& batch) {
	list<DBAction>::const_iterator it = batch.getAcciones().cbegin();
	list<DBAction>::const_iterator end = batch.getAcciones().cend();
	bool ok = true;
	for (; it != end and ok; it++) {
		ok = checkAction(*it);
	}
	if (ok){
		it = batch.getAcciones().cbegin(); //reinicia
		for (; it != end; it++) {
			executeAction(*it);
		}
	}
	return ok;
}

bool MapDB::contains(string key) {
	return mapa.count(key);
}

bool MapDB::checkAction(const DBAction& action) {
	switch (action.action) {
	case PUT:
		return not contains(action.key);
	case MODIFY: case ERASE:
		return contains(action.key);
	default:
		throw new InvalidDBAction("Accion inexistente (existen PUT, MODIFY y ERASE)");
	}
}

bool MapDB::executeAction(const DBAction& action) {
	switch (action.action) {
	case PUT:
		mapa.insert(pair<string, string> (action.key, action.value));
		break;
	case MODIFY:
		mapa.at(action.key) = action.value;
		break;
	case ERASE:
		mapa.erase(action.key);
		break;
	default:
		throw new InvalidDBAction("Accion inexistente (existen PUT, MODIFY y ERASE)");
	}
	return true;
}

void MapDB::deleteBD() {
	mapa.clear();
}
