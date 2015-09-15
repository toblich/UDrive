#ifndef INCLUDE_MANEJADORDEUSUARIOS_H_
#define INCLUDE_MANEJADORDEUSUARIOS_H_

#include <time.h>
#include <string>
#include <iostream>
#include <functional>
#include <unistd.h>
#include "basededatos.h"

class ManejadorDeUsuarios {
public:
	ManejadorDeUsuarios(BaseDeDatos* perfiles, BaseDeDatos* sesiones, BaseDeDatos* passwords);
	virtual ~ManejadorDeUsuarios();

	bool validarLogin(string username, string password);
	string iniciarSesion(string username);

	bool autenticarToken(string token, string username);

	bool cerrarSesion(string token, string username);

	bool registrarUsuario(string username, string password, string perfil);

	//bool eliminarUsuario();

private:
	BaseDeDatos* perfiles;
	BaseDeDatos* sesiones;
	BaseDeDatos* passwords;
	string generarToken(string username, time_t timestamp);
	bool esCaracterInvalido(char c);
	bool esPerfilValido(string perfil);
	bool esUsernameValido(string username);
	bool esPasswordValida(string password);
};

#endif /* INCLUDE_MANEJADORDEUSUARIOS_H_ */
