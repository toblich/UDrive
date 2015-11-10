#ifndef INCLUDE_BATCH_H_
#define INCLUDE_BATCH_H_

#include <list>
#include <string>

using namespace std;

/**
 * @brief Enumerado que agrupa las acciones posibles sobre la BD.
 */
enum Action {
	PUT, MODIFY, ERASE
};

/**
 * @brief Struct que agrupa una determinada acción sobre un key con un value en la BD.
 */
struct DBAction {
	Action action;
	string key;
	string value;
};

/**
 * @brief Clase que representa un Batch o una transacción en la BD.
 *
 * Esta clase se utiliza para agrupar un conjunto de acciones que se quiere realizar sobre la BD.
 * Este conjunto se tiene que realizar en su totalidad y en caso de que algo falle ninguna de las acciones
 * tiene efecto sobre la BD.
 */
class Batch {
private:
	list<DBAction> acciones;
public:
	Batch();
	virtual ~Batch();
	/**
	 * @brief Agrega una nueva key con un determinado value a la BD.
	 */
	void put(string key, string value);
	/**
	 * @brief Modifica el value de una determinada key en la BD.
	 */
	void modify(string key, string value);
	/**
	 * @brief Elimina una key de la BD.
	 */
	void erase(string key);
	/**
	 * @brief Devuelve la lista de acciones que tiene el Batch.
	 */
	const list<DBAction>& getAcciones() const {
		return acciones;
	}
};

#endif /* INCLUDE_BATCH_H_ */
