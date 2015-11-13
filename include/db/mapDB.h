
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
	/**
	 * @brief Función para ver si una key se encuentra en el mapa.
	 */
	bool contains(string key);
	/**
	 * @brief Función para agregar una key con un value al mapa.
	 */
	bool put(string key, string value);
	/**
	 * @brief Función para obtener el value de una key del mapa.
	 * @throw KeyNotFound
	 */
	string get(string key);
	/**
	 * @brief Función para eliminar una key del mapa.
	 * @throw KeyNotFound
	 */
	void erase(string key);
	/**
	 * @brief Función para modificar el value de una key en el mapa.
	 * @throw KeyNotFound
	 */
	void modify(string key, string value);
	/**
	 * @brief Función para escribir un batch en el mapa.
	 */
	bool writeBatch(const Batch& batch);
	/**
	 * @brief Elimina todos los datos guardados en el mapa.
	 */
	void deleteBD();
};

#endif /* INCLUDE_MAPDB_H_ */
