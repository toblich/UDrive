#ifndef INCLUDE_METADATOSYCONSTANTES_H_
#define INCLUDE_METADATOSYCONSTANTES_H_

#include <list>
#include <iostream>

#define RESERVED_CHAR '!'	// Ojo, mover los tres juntos
const std::string RESERVED_STR = "!";
const std::string RESERVED_REGEX = "\\!";

#define FIRST 1
#define FIRST_STR "1"
#define RESERVED_FIRST (RESERVED_STR + FIRST_STR)
#define LATEST -1
#define LATEST_STR "-1"
#define RESERVED_LATEST (RESERVED_STR + LATEST_STR)

const std::string TRASH = RESERVED_STR + "trash";
const std::string PERMISOS = RESERVED_STR + "permisos";
const std::string FOLDER = RESERVED_STR + "folder";
const std::string FOTOS = RESERVED_STR + "fotos";
const std::string PATH_DEFAULT_FOTO_PERFIL = "default.jpg";

const std::string DEFAULT_FS = "FileSystem";
const std::string DEFAULT_DB = "db";
const int NIVEL_LOG_DEFAULT = 0; // TRACE
const int TAM_MAX_LOG_DEFAULT = 1024; // En KBytes -> Un MB
const int CANT_THREADS_DEFAULT = 10;
const int POLL_SERVER_DEFAULT = 1; // En milisegundos

const int CUOTA_DEFAULT = 2048;

/**
 * @brief Struct para guardar los metadatos de un archivo
 *
 */
typedef struct{
	std::string nombre;
	std::string extension;
	std::string fechaUltimaModificacion;
	std::string usuarioUltimaModificacion;
	std::string propietario;
	int ultimaVersion;
	std::list<std::string> etiquetas;
	std::list<std::string> usuariosHabilitados; // Seria correcto que aca figurara el propietario tambien
}MetadatoArchivo;

/**
 * @brief Struct para guardar la ultima ubicación de un usuario
 *
 */
typedef struct{
	double latitud, longitud;
}Ubicacion;

/**
 * @brief Struct para guardar el perfil de un usuario
 *
 */
typedef struct{
	std::string nombre;
	std::string email;
	std::string pathFotoPerfil;
	int cuota; // En MB
	Ubicacion ultimaUbicacion;
}MetadatoUsuario;

/**
 * @brief Struct para guardar los datos de la sesión de un usuario
 *
 */
typedef struct{
	std::string username;
	std::string password;
	std::string token;
}MetadatoSesion;

/**
 * @brief Struct para guardar los datos leidos del archivo de configuración
 *
 */
typedef struct{
	int nivelLog;
	int tamMaxLog;
	std::string pathFS;
	std::string pathDB;
	int cantThreads;
	int tiempoPollServer;
}Configuracion;

#endif /* INCLUDE_METADATOSYCONSTANTES_H_ */
