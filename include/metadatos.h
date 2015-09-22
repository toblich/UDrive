#ifndef INCLUDE_METADATOS_H_
#define INCLUDE_METADATOS_H_

#include <list>
#include <iostream>

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

#endif /* INCLUDE_METADATOS_H_ */
