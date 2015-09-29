#ifndef SESSION_H_
#define SESSION_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

class Session : public RealizadorDeEventos {
public:
	Session(ManejadorDeUsuarios* manejadorUsuarios);
	virtual ~Session();

protected:
	ManejadorDeUsuarios* manejadorUs;

	mg_result POSTHandler(mg_connection* connection);
	mg_result DELETEHandler(mg_connection* connection);
};

#endif /* SESSION_H_ */
