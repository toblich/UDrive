#ifndef UDRIVE_BUSCADOR_H
#define UDRIVE_BUSCADOR_H

#include <functional>
#include "validador.h"
#include "bd.h"
#include "metadatosYConstantes.h"

class Buscador {
public:
	Buscador(BD* dbMetadatos, string pathFileSystem);
	virtual ~Buscador();

	string obtenerEstructuraCarpeta(string path, bool esRecursivo, function<bool(MetadatoArchivo&)> predicate);
	string obtenerEstructuraCompartidos(string path);

	string buscarPorExtension(string username, string extension);
	string buscarPorEtiqueta(string username, string etiqueta);
	string buscarPorNombre(string username, string nombre);
	//Esta busqueda implica mostrar todos los archivos que me compartio ese propietario
	string buscarPorPropietario(string username, string propietario);

private:

	map<string, string> buscar(string username, function<bool(MetadatoArchivo&)> predicate);
	BD* dbMetadatos;
	Validador validador;
	const string pathFileSystem;
};


#endif //UDRIVE_BUSCADOR_H
