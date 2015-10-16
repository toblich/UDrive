#ifndef INCLUDE_METADATOSYCONSTANTE_H_
#define INCLUDE_METADATOSYCONSTANTE_H_

#include <list>
#include <iostream>

#define RESERVED_CHAR '^'	// Ojo, mover los tres juntos
const std::string RESERVED_STR = "^";
const std::string RESERVED_REGEX = "\\^";

const std::string TRASH = RESERVED_STR + "trash";
const std::string PERMISOS = RESERVED_STR + "permisos";
const std::string FOLDER = RESERVED_STR + "folder";

const std::string DEFAULT_FS = "FileSystem";

typedef struct{
	std::string nombre;
	std::string extension;
	std::string fechaUltimaModificacion;
	std::string usuarioUltimaModificacion;
	std::string propietario;
	std::list<std::string> etiquetas;
	std::list<std::string> usuariosHabilitados; //TODO: Seria correcto que aca figurara el propietario tambien
}MetadatoArchivo;

typedef struct{
	float latitud, longitud;
}Ubicacion;

typedef struct{
	std::string nombre;
	std::string email;
	std::string pathFotoPerfil;
	Ubicacion ultimaUbicacion;
}MetadatoUsuario;

typedef struct{
	std::string username;
	std::string password;
	std::string token;
}MetadatosSesion;

#endif /* INCLUDE_METADATOSYCONSTANTE_H_ */
