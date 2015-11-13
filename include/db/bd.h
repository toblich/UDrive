#ifndef INCLUDE_BD_H_
#define INCLUDE_BD_H_

#include <string>
#include "excepciones.h"
#include "batch.h"

using std::string;

/**
 * @brief Clase abstracta de la que heredan las clases BaseDeDatos y MapDB.
 */
class BD {
public:
	virtual ~BD() {};
	virtual bool contains(string key) = 0;
	virtual bool put(string key, string value) = 0;
	virtual string get(string key) = 0;
	virtual void erase(string key) = 0;
	virtual void modify(string key, string value) = 0;
	virtual bool writeBatch(const Batch& batch) = 0;
	virtual void deleteBD() = 0;
};

#endif /* INCLUDE_BD_H_ */
