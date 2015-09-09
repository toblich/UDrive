#include <iostream>
#include "basededatos.h"

using namespace std;

int main(int argc, char* argv[]) {
	BaseDeDatos bd;

	bd.put("key1", "value");
	std::string value = bd.get("key1");
	if (value != "value") cout << "Mal get" << endl;

	bd.erase("key1");
	bd.put("key2", value);

	bd.get("key1");
	value = bd.get("key2");
	if (value != "value") cout << "Mal get 2" << endl;

	return 0;
}
