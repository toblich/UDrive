#ifndef MANEJADORDEUSUARIOS_H_
#define MANEJADORDEUSUARIOS_H_

#include <time.h>
#include <string>
#include <iostream>
#include <functional>
#include <unistd.h>
#include <regex>
#include "bd.h"
#include "parserURI.h"
#include "parserJson.h"

using std::string;

/**
 * @brief Se encarga del manejo de usuarios en el servidor.
 *
 * Se encarga de crear usuarios, manejar sus perfiles, sus sesiones
 * y la autenticación de los mismos.
 * Permite además buscar usuarios por (parte de) su username.
 */
class ManejadorDeUsuarios {
public:
	ManejadorDeUsuarios(BD* perfiles, BD* sesiones, BD* passwords);
	virtual ~ManejadorDeUsuarios();

	/**
	 * @brief Valida que la contraseña provista coincida con la real.
	 */
	bool validarLogin(string username, string password);

	/**
	 * @brief Crea una nueva sesión del usuario.
	 * Deben validarse los datos de login previamente.
	 *
	 * @retval string con el token de sesión.
	 */
	string iniciarSesion(string username);

	/**
	 * @brief Valida que el token coincida con una sesión de ese usuario.
	 */
	bool autenticarToken(string token, string username);

	/**
	 * @brief Cierra la sesión del usuario correspondiente al token indicado.
	 * Dicho token queda invalidado.
	 *
	 * @retval bool indicando si se pudo cerrar la sesión. Ante tokens inválidos, devuelve false.
	 */
	bool cerrarSesion(string token, string username);

	/**
	 * @brief Crea un usuario con los datos indicados, si son válidos.
	 *
	 * @retval bool indicando si pudo registrarse el usuario. Ante usernames ya existentes
	 * o passwords/perfiles inválidos, devuelve false.
	 *
	 * @param string perfil es un JSON.
	 */
	bool registrarUsuario(string username, string password, string perfil);

	/**
	 * @brief Busca aquellos usuarios cuyo username contenga el substring indicado.
	 *
	 * @retval string con un json de la forma '{"usuarios" : "user1!user2!user3"}'
	 */
	string buscarUsuariosCon(const string& substring);

	string getPerfil(string username);
	bool modifyPerfil(string username, string perfil);
	bool esPerfilValido(const string& perfil);

private:
	BD* perfiles;	///< Base de datos de perfiles de usuario (y la lista de usuarios bajo la clave del atributo USERLIST)
	BD* sesiones;	///< Base de datos de sesiones activas
	BD* passwords;	///< Base de datos de contraseñas de usuarios registrados
	string generarToken(string username, time_t timestamp);
	bool esCaracterInvalido(char c);
	bool esUsernameValido(const string& username);
	bool esPasswordValida(const string& password);
	string usuariosConSubstring(const string& substring);

	const string CHARS_INVALIDOS = "/ ~#?=&" + RESERVED_STR;	///< string que contiene todos los caracteres prohibidos en los usernames
	const string USERLIST = RESERVED_STR + "users";	///< clave reservada para la lista de usuarios en la base de datos de perfiles
	const regex REGEX_NOMBRE = regex("^[ [:alpha:]]+$", std::regex_constants::ECMAScript);	 ///< Regex de usernames validos
	const regex REGEX_EMAIL = regex("^[-_[:alnum:]]+@[[:alpha:]]+\\.([.[:alpha:]]+)+$");	///< Regex de emails validos

};

#endif /* MANEJADORDEUSUARIOS_H_ */
