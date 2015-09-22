
#ifndef INCLUDE_BD_H_
#define INCLUDE_BD_H_

#include <string>
#include "excepciones.h"
#include "batch.h"

class BD {
public:
	virtual ~BD() {};
	virtual bool contains(std::string key) = 0;
	virtual bool put(std::string key, std::string value) = 0;
	virtual std::string get(std::string key) = 0;
	virtual void erase(std::string key) = 0;
	virtual void modify(std::string key, std::string value) = 0;
	virtual bool writeBatch(const Batch& batch) = 0;
	virtual void deleteBD() = 0;
};

#endif /* INCLUDE_BD_H_ */
