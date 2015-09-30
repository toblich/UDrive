#ifndef INCLUDE_MANEJADORDEUSUARIOS_H_
#define INCLUDE_MANEJADORDEUSUARIOS_H_

#include <time.h>
#include <string>
#include <iostream>
#include <functional>
#include <unistd.h>
#include <regex>
#include "bd.h"
#include "parserJson.h"

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

private:
	BD* perfiles;
	BD* sesiones;
	BD* passwords;
	string generarToken(string username, time_t timestamp);
	bool esCaracterInvalido(char c);
	bool esPerfilValido(const string& perfil);
	bool esUsernameValido(const string& username);
	bool esPasswordValida(const string& password);

	const string CHARS_INVALIDOS = "/ ~#?=&";
	const regex REGEX_NOMBRE = regex("^[ [:alpha:]]+$", std::regex_constants::ECMAScript);	 //TODO: ver si requiere aceptar tildes
	const regex REGEX_EMAIL = regex("^[-_[:alnum:]]+@[[:alpha:]]+\\.([.[:alpha:]]+)+$");
};

#endif /* INCLUDE_MANEJADORDEUSUARIOS_H_ */
