#ifndef INCLUDE_REALIZADORDEEVENTOS_H_
#define INCLUDE_REALIZADORDEEVENTOS_H_

#include <string>
#include <iostream>
#include <fstream>
#include "mongoose.h"
#include "manejadorDeUsuarios.h"
#include "parserURI.h"

using namespace std;

class RealizadorDeEventos {
public:
	RealizadorDeEventos();
	virtual ~RealizadorDeEventos();
	mg_result handler(mg_connection* connection);

protected:
	void getMultipartData(mg_connection* connection);
	string getVar(mg_connection* connection, string varName);
	virtual mg_result POSTHandler(mg_connection* connection);
	virtual mg_result GETHandler(mg_connection* connection);
	virtual mg_result PUTHandler(mg_connection* connection);
	virtual mg_result DELETEHandler(mg_connection* connection);
	void unsupportedMethod(mg_connection* connection);

};

#endif /* INCLUDE_REALIZADORDEEVENTOS_H_ */
