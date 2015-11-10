
#ifndef INCLUDE_MAPDB_H_
#define INCLUDE_MAPDB_H_

#include "bd.h"
#include "logger.h"
#include <map>

using namespace std;

/**
 * @brief Clase que se utiliza como un Mock de la BaseDeDatos.
 *
 * Esta clase no brinda persistencia pero si brinda velocidad para los
 * test que utilizan o prueban los métodos de la BaseDeDatos.
 */
class MapDB: public BD {
private:
	map<string, string> mapa;

	bool checkAction(const DBAction& action);
	bool executeAction(const DBAction& action);
public:
	MapDB();
	virtual ~MapDB();
	bool contains(string key);
	bool put(string key, string value);
	string get(string key);					/* Throws: KeyNotFound */
	void erase(string key);					/* Throws: KeyNotFound */
	void modify(string key, string value);	/* Throws: KeyNotFound */
	bool writeBatch(const Batch& batch);
	void deleteBD();
};

#endif /* INCLUDE_MAPDB_H_ */
