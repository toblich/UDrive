#include <iostream>
#include "basededatos.h"

#include "basededatos.h"

using namespace std;

int main(int argc, char* argv[]) {
	BaseDeDatos *bd = new BaseDeDatos();

	bd->put("key1", "value");
	std::string value = bd->get("key1");
	if (value != "value") cout << "Mal get" << endl;

	bd->erase("key1");
	bd->put("key2", value);

	bd->get("key1");
	value = bd->get("key2");
	if (value != "value") cout << "Mal get 2" << endl;

	delete bd;

	cout << "----------------------------" << endl;

	BaseDeDatos *BD = new BaseDeDatos();
	BD->put("uno", "pancho");
	BD->put("dos", "santi");

	string valorUno = BD->get("uno");
	string valorDos = BD->get("dos");

	cout << "Uno: " << valorUno << " | Dos: " << valorDos << endl;

	BD->put("uno", "pablo");

	valorUno = BD->get("uno");
	valorDos = BD->get("tres");
	cout << "Uno: " << valorUno << " | Dos: " << valorDos << endl;
	BD->deleteBD();
	delete BD;

	return 0;
}
