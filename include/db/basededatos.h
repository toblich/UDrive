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

const std::string defaultPath = "DB";

/**
 * @brief Se encarga del manejo de la base de datos del servidor.
 */
class BaseDeDatos : public BD {
private:
	DB* db;
	std::string path;
	Options options;

	bool addActionToBatch(const DBAction& action,rocksdb::WriteBatch& writeBatch);

public:
	BaseDeDatos();									/* Throws: runtime_error */
	/**
	 * @brief Constructor: Crea una nueva base de datos
	 *
	 * @param path 		string con el path donde se guardaran los datos de la base de datos.
	 */
	BaseDeDatos(std::string path);					/* Throws: runtime_error */
	~BaseDeDatos();
	/**
	 * @brief Función para ver si una key se encuentra en la base de datos.
	 */
	bool contains(std::string key);
	/**
	 * @brief Función para agregar una key con un value a la base de datos.
	 */
	bool put(std::string key, std::string value);	/* Throws: runtime_error */
	/**
	 * @brief Función para obtener el value de una key de la base de datos.
	 */
	std::string get(std::string key);				/* Throws: runtime_error, KeyNotFound */
	/**
	 * @brief Función para eliminar una key de la base de datos.
	 */
	void erase(std::string key);					/* Throws: runtime_error, KeyNotFound */
	/**
	 * @brief Función para modificar el value de una key en la base de datos.
	 */
	void modify(std::string key, std::string value); /* Throws: runtime_error, KeyNotFound */
	/**
	 * @brief Función para escribir un batch en la base de datos.
	 */
	bool writeBatch(const Batch& batch);			/* No lanza excepciones */
	/**
	 * @brief Elimina tanto la base de datos como los datos guardados en ella.
	 */
	void deleteBD();
};

#endif /* BASEDEDATOS_H_ */
