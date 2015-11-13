#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_

#include "bd.h"
#include "logger.h"

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <rocksdb/write_batch.h>

using rocksdb::DB;
using rocksdb::Options;
using rocksdb::Status;
using rocksdb::WriteOptions;
using rocksdb::ReadOptions;
using rocksdb::WriteBatch;

using std::string;
const string defaultPath = "DB";

/**
 * @brief Se encarga del manejo de la base de datos del servidor.
 */
class BaseDeDatos : public BD {
private:
	DB* db;
	string path;
	Options options;

	bool addActionToBatch(const DBAction& action,rocksdb::WriteBatch& writeBatch);

public:
	/**
	 * @brief Constructor: Crea una nueva base de datos.
	 *
	 * Los datos de la base de datos se guardaran en el path por default.
	 *
	 * @throw runtime_error
	 */
	BaseDeDatos();
	/**
	 * @brief Constructor: Crea una nueva base de datos
	 *
	 * @param path 		string con el path donde se guardaran los datos de la base de datos.
	 * @throw runtime_error
	 */
	BaseDeDatos(string path);
	~BaseDeDatos();
	/**
	 * @brief Función para ver si una key se encuentra en la base de datos.
	 */
	bool contains(string key);
	/**
	 * @brief Función para agregar una key con un value a la base de datos.
	 * @throw runtime_error
	 */
	bool put(string key, string value);
	/**
	 * @brief Función para obtener el value de una key de la base de datos.
	 * @throw runtime_error
	 * @throw KeyNotFound
	 */
	string get(string key);
	/**
	 * @brief Función para eliminar una key de la base de datos.
	 * @throw runtime_error
	 * @throw KeyNotFound
	 */
	void erase(string key);
	/**
	 * @brief Función para modificar el value de una key en la base de datos.
	 * @throw runtime_error
	 * @throw KeyNotFound
	 */
	void modify(string key, string value);
	/**
	 * @brief Función para escribir un batch en la base de datos.
	 */
	bool writeBatch(const Batch& batch);
	/**
	 * @brief Elimina tanto la base de datos como los datos guardados en ella.
	 */
	void deleteBD();
};

#endif /* BASEDEDATOS_H_ */
