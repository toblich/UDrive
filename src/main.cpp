#include <iostream>
#include "Server.h"

using namespace std;

int main(int argc, char** argv) {
	cout << "Hola Mundo!" << endl;

	string puerto = "8080";
	Server server(puerto);
	cout << "Lanzando servidor en el puerto " << server.getListeningPort() << endl;
	for (;;) {
		server.pollServer(1000);
	}

	return 0;
}
