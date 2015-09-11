//#include <fstream>
//#include "parserJson.h"
//
//
//
//	ParserJson parser;
//
//
//
//
//
//	//PRUEBA SERIALIZACION ARCHIVO
//	MetadatoArchivo metaArch;
//	metaArch.nombre = "sol";
//	metaArch.extension = "jpg";
//	metaArch.fechaUltimaModificacion = "09/09/2015";
//	metaArch.usuarioUltimaModificacion = "Pepe";
//	metaArch.propietario = "Pancheitor";
//
//	list<string> etiquetas;
//	etiquetas.push_back("lindo");
//	etiquetas.push_back("grupo");
//	etiquetas.push_back("juan");
//	metaArch.etiquetas = etiquetas;
//
//	list<string> usuarios;
//	usuarios.push_back("Pancheitor");
//	usuarios.push_back("Juan");
//	usuarios.push_back("Pepe");
//	usuarios.push_back("Santi");
//	metaArch.usuariosHabilitados = usuarios;
//
//	string jsonArch = parser.serializarMetadatoArchivo(metaArch);
//
//
//
//
//
//	// PRUEBA DESERIALIZACION ARCHIVO
//	string jsonArch = "{\n\t\"etiquetas\" : [ \"23\", true, \"juan\" ],\n\t\"extension\" : \"jpg\",\n\t\"fecha ultima modificacion\" : \"09/09/2015\",\n\t\"nombre\" : \"sol\",\n\t\"propietario\" : \"Pancheitor\",\n\t\"usuario ultima modificacion\" : \"Pepe\",\n\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n}";
//	cout << jsonArch << endl;
//	MetadatoArchivo pruebaArch = parser.deserializarMetadatoArchivo(jsonArch);
//
//	cout << pruebaArch.nombre << endl;
//	cout << pruebaArch.extension << endl;
//	cout << pruebaArch.fechaUltimaModificacion << endl;
//	cout << pruebaArch.usuarioUltimaModificacion << endl;
//	cout << pruebaArch.propietario << endl;
//
//	list<string> usu = pruebaArch.usuariosHabilitados;
//	list<string>::iterator itUsu = usu.begin();
//	for ( ; itUsu != usu.end() ; itUsu++){
//		cout << (*itUsu) << " ";
//	}
//	cout << endl;
//
//	list<string> etiq = pruebaArch.etiquetas;
//	list<string>::iterator itEtiq = etiq.begin();
//	for ( ; itEtiq != etiq.end() ; itEtiq++){
//		cout << (*itEtiq) << " ";
//	}
//	cout << endl << endl << endl << endl;
//
//
//
//
//
//	//PRUEBA SERIALIZACION USUARIO
//	MetadatoUsuario metaUsu;
//	metaUsu.nombre = "Pancheitor";
//	metaUsu.email = "panch@eitor.com";
//	metaUsu.pathFotoPerfil = "fotos/pancheitor.jpg";
//
//	Ubicacion ubicacion;
//	ubicacion.latitud = 45.0123;
//	ubicacion.longitud = -37.1293;
//	metaUsu.ultimaUbicacion = ubicacion;
//
//	string jsonUsu = parser.serializarMetadatoUsuario(metaUsu);
//
//
//
//
//
//	// PRUEBA DESERIALIZACION USUARIO
//	string jsonUsu = "{\n\t\"email\" : \"panch@eitor.com\",\n\t\"nombre\" : \"Pancheitor\",\n\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n\t\"ultima ubicacion\" : {\n\t \t\"latitud\" : 45.0123,\n\t \t\"longitud\" : -37.1293\n\t}\n}";
//	cout << jsonUsu << endl;
//	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsu);
//
//	cout << pruebaUsu.nombre << endl;
//	cout << pruebaUsu.email << endl;
//	cout << pruebaUsu.pathFotoPerfil << endl;
//	cout << pruebaUsu.ultimaUbicacion.latitud << endl;
//	cout << pruebaUsu.ultimaUbicacion.longitud << endl;
//
//
//
//
//
//	//PRUEBA PARSEADO DE ARCHIVO Y STRING
//	string hola;
//	const char* rutaArchivo = "hola.json";
//	ifstream entrada(rutaArchivo, ios::binary );
//	if (entrada.good()){
//		Json::Value raiz;
//		Json::Features f = Json::Features::strictMode();
//		Json::Reader reader(f);
//		bool parseadoExitoso = reader.parse(entrada, raiz);
//		if (parseadoExitoso){
//			hola = raiz.toStyledString();
//			cout << hola << endl;
//		}
//	}
//
//	Json::Value raiz2;
//	Json::Features f = Json::Features::strictMode();
//	Json::Reader reader(f);
//	bool parseadoExitoso = reader.parse(hola, raiz2);
//	if (parseadoExitoso){
//		string chau = raiz2.toStyledString();
//		cout << chau << endl;
//	}
