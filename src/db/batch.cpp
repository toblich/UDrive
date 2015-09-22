#include "batch.h"

Batch::Batch() : acciones() {}

Batch::~Batch() {}

void Batch::put(string key, string value) {
	DBAction put = {PUT, key, value};
	acciones.push_back(put);
}

void Batch::modify(string key, string value) {
	DBAction modify = {MODIFY, key, value};
	acciones.push_back(modify);
}

void Batch::erase(string key) {
	DBAction erase = {ERASE, key, ""};
	acciones.push_back(erase);
}
