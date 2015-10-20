#include "parserJson.h"
#include <gtest/gtest.h>

using namespace std;

const string jsonArchOK = "{\n"
		"\t\"etiquetas\" : [ \"23\", true, \"juan\" ],\n"
		"\t\"extension\" : \"jpg\",\n"
		"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
		"\t\"nombre\" : \"sol\",\n"
		"\t\"propietario\" : \"Pancheitor\",\n"
		"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
		"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n"
		"}";

const string jsonUsuOK = "{\n"
		"\t\"email\" : \"panch@eitor.com\",\n"
		"\t\"nombre\" : \"Pancheitor\",\n"
		"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
		"\t\"cuota\" : 1024,\n"
		"\t\"ultima ubicacion\" : {\n"
			"\t \t\"latitud\" : 45.0123,\n"
			"\t \t\"longitud\" : -37.1293\n"
		"\t}\n"
		"}";

const string jsonSesOK = "{\n"
		"\t\"username\" : \"pancheitor\",\n"
		"\t\"password\" : \"pancho123\",\n"
		"\t\"token\" : \"asg371ns812ssk\"\n"
		"}";

bool sonListasIguales(const list<string>& expected, const list<string>& actual) {
	list<string>::const_iterator exp = expected.cbegin();
	list<string>::const_iterator act = actual.cbegin();
	const list<string>::const_iterator ENDexp = expected.cend();
	const list<string>::const_iterator ENDact = actual.cend();

	for (; exp != ENDexp and act != ENDact; exp++, act++) {
		if (*exp != *act) return false;
	}

	return (exp == ENDexp and act == ENDact);
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreCorrectoMetadatoArchivo) {
	MetadatoArchivo pruebaArch = ParserJson::deserializarMetadatoArchivo(jsonArchOK);
	EXPECT_EQ("sol", pruebaArch.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreInexistenteMetadatoArchivo){
	string jsonArch = "{\n"
			"\t\"etiquetas\" : [ \"23\", true, \"juan\" ],\n"
			"\t\"extension\" : \"jpg\",\n"
			"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
			"\t\"propietario\" : \"Pancheitor\",\n"
			"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n"
			"}";
	MetadatoArchivo pruebaArch = ParserJson::deserializarMetadatoArchivo(jsonArch);

	EXPECT_EQ("", pruebaArch.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienEtiquetasCorrectasMetadatoArchivo){
	MetadatoArchivo pruebaArch = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	list<string> etiq = pruebaArch.etiquetas;
	list<string>::iterator itEtiq = etiq.begin();
	int i = 0;
	for ( ; itEtiq != etiq.end() ; itEtiq++, i++){
		switch (i) {
		case 0:
			EXPECT_EQ("23", (*itEtiq));
			break;
		case 1:
			EXPECT_EQ("true", (*itEtiq));
			break;
		case 2:
			EXPECT_EQ("juan", (*itEtiq));
			break;
		default:
			FAIL();
		}
	}
}

TEST(ParserJsonTest, deberiaDeserializarBienEtiquetasInexistenteMetadatoArchivo){
	string jsonArch = "{\n"
			"\t\"extension\" : \"jpg\",\n"
			"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
			"\t\"nombre\" : \"sol\",\n"
			"\t\"propietario\" : \"Pancheitor\",\n"
			"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n"
			"}";
	MetadatoArchivo pruebaArch = ParserJson::deserializarMetadatoArchivo(jsonArch);

	list<string> etiq = pruebaArch.etiquetas;
	EXPECT_EQ(true,etiq.empty());
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreCorrectoMetadatoUsuario){
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsuOK);
	EXPECT_EQ("Pancheitor", pruebaUsu.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreInexistenteMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"cuota\" : 1024,\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : 45.0123,\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_EQ("", pruebaUsu.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienCuotaCorrectaMetadatoUsuario){
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsuOK);
	EXPECT_EQ(1024, pruebaUsu.cuota);
}

TEST(ParserJsonTest, deberiaDeserializarBienCuotaIncorrectaMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"cuota\" : \"hola\",\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : 45.0123,\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_EQ(CUOTA, pruebaUsu.cuota);
}

TEST(ParserJsonTest, deberiaDeserializarBienCuotaMenorACeroMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"cuota\" : -3,\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : 45.0123,\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_EQ(CUOTA, pruebaUsu.cuota);
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionCorrectaMetadatoUsuario){
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsuOK);
	EXPECT_FLOAT_EQ(45.0123, pruebaUsu.ultimaUbicacion.latitud);
	EXPECT_FLOAT_EQ(-37.1293, pruebaUsu.ultimaUbicacion.longitud);
}

TEST(ParserJsonTest, deberiaDeserializarBienCuotaInexistenteMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : 45.0123,\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_EQ(CUOTA, pruebaUsu.cuota);
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionLatitudInvalidaMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"cuota\" : 1024,\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : \"hola\",\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.latitud); //0 es el valor que se le asigna por defecto
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionLatitudInexistenteMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"cuota\" : 1024,\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.latitud); //0 es el valor que se le asigna por defecto
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionInexistenteMetadatoUsuario){
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"cuota\" : 1024\n"
			"}";
	MetadatoUsuario pruebaUsu = ParserJson::deserializarMetadatoUsuario(jsonUsu);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.latitud);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.longitud);
}

TEST(ParserJsonTest, deberiaDeserializarBienUsernameCorrectoMetadatoSesion){
	MetadatosSesion pruebaSes = ParserJson::deserializarMetadatoSesion(jsonSesOK);
	EXPECT_EQ("pancheitor", pruebaSes.username);
}

TEST(ParserJsonTest, deberiaDeserializarBienUsernameInexistenteMetadatoSesion){
	string jsonSes = "{\n"
			"\t\"password\" : \"pancho123\",\n"
			"\t\"token\" : \"asg371ns812ssk\"\n"
			"}";
	MetadatosSesion pruebaSes = ParserJson::deserializarMetadatoSesion(jsonSes);
	EXPECT_EQ("", pruebaSes.username);
}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMetadatoArchivo){
	list<string> etiquetas;
	etiquetas.push_back("etiA");
	etiquetas.push_back("#etiqueta B");
	list<string> usuariosHabilitados;
	usuariosHabilitados.push_back("pablo");
	usuariosHabilitados.push_back("santi");

	MetadatoArchivo original = {
			"nombreArchivo",
			"jpg",
			"29/09/2015",
			"pablo",
			"santi",
			etiquetas,
			usuariosHabilitados
	};

	string json = ParserJson::serializarMetadatoArchivo(original);
	MetadatoArchivo deserializado = ParserJson::deserializarMetadatoArchivo(json);

	EXPECT_EQ(original.nombre, deserializado.nombre);
	EXPECT_EQ(original.extension, deserializado.extension);
	EXPECT_EQ(original.fechaUltimaModificacion, deserializado.fechaUltimaModificacion);
	EXPECT_EQ(original.usuarioUltimaModificacion, deserializado.usuarioUltimaModificacion);
	EXPECT_EQ(original.propietario, deserializado.propietario);
	EXPECT_TRUE(sonListasIguales(original.etiquetas, deserializado.etiquetas));
	EXPECT_TRUE(sonListasIguales(original.usuariosHabilitados, deserializado.usuariosHabilitados));
}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMetadatoUsuario){
	MetadatoUsuario original = {
			"tobi",
			"u@drive.com",
			"<pathFotoPerfil>",
			1024,
			{ 1.0, 1.0 }
	};

	string json = ParserJson::serializarMetadatoUsuario(original);
	MetadatoUsuario deserializado = ParserJson::deserializarMetadatoUsuario(json);

	EXPECT_EQ(original.nombre, deserializado.nombre);
	EXPECT_EQ(original.email, deserializado.email);
	EXPECT_EQ(original.pathFotoPerfil, deserializado.pathFotoPerfil);
	EXPECT_EQ(original.cuota, deserializado.cuota);
	EXPECT_FLOAT_EQ(original.ultimaUbicacion.latitud, deserializado.ultimaUbicacion.latitud);
	EXPECT_FLOAT_EQ(original.ultimaUbicacion.longitud, deserializado.ultimaUbicacion.longitud);
}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMetadatoSesion){
	MetadatosSesion original = {
			"pancho",
			"tallerdos",
			"902430791097619746"
	};

	string json = ParserJson::serializarMetadatoSesion(original);
	MetadatosSesion deserializado = ParserJson::deserializarMetadatoSesion(json);

	EXPECT_EQ(original.username, deserializado.username);
	EXPECT_EQ(original.password, deserializado.password);
	EXPECT_EQ(original.token, deserializado.token);
}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMapa){
	map<string, string> original;
	original.insert(pair<string, string>("chau", "como estas"));
	original.insert(pair<string, string>("hola", "pepe"));
	original.insert(pair<string, string>("juan", "cito"));

	string json = ParserJson::serializarMapa(original);
	map<string, string> deserializado = ParserJson::deserializarMapa(json);

	EXPECT_EQ(original.at("chau"), deserializado.at("chau"));
	EXPECT_EQ(original.at("hola"), deserializado.at("hola"));
	EXPECT_EQ(original.at("juan"), deserializado.at("juan"));
}
