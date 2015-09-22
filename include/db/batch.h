#ifndef INCLUDE_BATCH_H_
#define INCLUDE_BATCH_H_

#include <list>
#include <string>

using namespace std;


enum Action {
	PUT, MODIFY, ERASE
};

struct DBAction {
	Action action;
	string key;
	string value;
};


class Batch {
private:
	list<DBAction> acciones;
public:
	Batch();
	virtual ~Batch();

	void put(string key, string value);
	void modify(string key, string value);
	void erase(string key);

	const list<DBAction>& getAcciones() const {
		return acciones;
	}
};

#endif /* INCLUDE_BATCH_H_ */
