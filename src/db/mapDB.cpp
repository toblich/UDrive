#include "mapDB.h"

using namespace std;

MapDB::MapDB() : mapa() {}

MapDB::~MapDB() {}

bool MapDB::put(string key, string value) {
	if (not contains(key)) {
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
	if (contains(key))
		mapa.erase(key);
	else
		throw KeyNotFound("mapDB.erase: " + key);
}

void MapDB::modify(string key, string value) {
	if (contains(key))
		mapa.at(key) = value;
	else
		throw KeyNotFound("mapDB.modify: " + key);
}

bool MapDB::writeBatch(const Batch& batch) {
	list<DBAction>::const_iterator it = batch.getAcciones().cbegin();
	const list<DBAction>::const_iterator END = batch.getAcciones().cend();
	bool ok = true;
	for (; it != END and ok; it++) {
		ok = checkAction(*it);
	}
	if (ok) {
		it = batch.getAcciones().cbegin(); //reinicia
		for (; it != END; it++) {
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
