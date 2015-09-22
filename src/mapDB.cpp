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

void MapDB::deleteBD() {
	mapa.clear();
}
