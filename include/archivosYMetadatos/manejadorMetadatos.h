#ifndef UDRIVE_MANEJADORMETADATOS_H
#define UDRIVE_MANEJADORMETADATOS_H

#include "validador.h"

class ManejadorMetadatos {

public:
	ManejadorMetadatos(BD* dbMetadatos, Validador* validador);

	string actualizarUsuarioFechaModificacion(string jsonMetadatos, string usernameModificacion);

private:
	BD* dbMetadatos;
	Validador* validador;
};


#endif //UDRIVE_MANEJADORMETADATOS_H
