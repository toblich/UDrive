#ifndef INCLUDE_MANEJADORDEUSUARIOS_H_
#define INCLUDE_MANEJADORDEUSUARIOS_H_

#include <time.h>
#include <string>
#include <iostream>
#include <functional>
#include <unistd.h>
#include "bd.h"

using std::string;

class ManejadorDeUsuarios {
public:
	ManejadorDeUsuarios(BD* perfiles, BD* sesiones, BD* passwords);
	virtual ~ManejadorDeUsuarios();

	bool validarLogin(string username, string password);
	string iniciarSesion(string username);

	bool autenticarToken(string token, string username);

	bool cerrarSesion(string token, string username);

	bool registrarUsuario(string username, string password, string perfil);

	//bool eliminarUsuario();

	string getPerfil(string username);
	void modifyPerfil(string username, string perfil);

private:
	BD* perfiles;
	BD* sesiones;
	BD* passwords;
	string generarToken(string username, time_t timestamp);
	bool esCaracterInvalido(char c);
	bool esPerfilValido(string perfil);
	bool esUsernameValido(string username);
	bool esPasswordValida(string password);
};

#endif /* INCLUDE_MANEJADORDEUSUARIOS_H_ */
