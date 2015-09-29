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


TEST(ParserJsonTest, deberiaDeserializarBienNombreCorrectoMetadatoArchivo) {
	ParserJson parser;
	MetadatoArchivo pruebaArch = parser.deserializarMetadatoArchivo(jsonArchOK);

	EXPECT_EQ("sol", pruebaArch.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreInexistenteMetadatoArchivo){
	ParserJson parser;
	string jsonArch = "{\n"
			"\t\"etiquetas\" : [ \"23\", true, \"juan\" ],\n"
			"\t\"extension\" : \"jpg\",\n"
			"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
			"\t\"propietario\" : \"Pancheitor\",\n"
			"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n"
			"}";
	MetadatoArchivo pruebaArch = parser.deserializarMetadatoArchivo(jsonArch);

	EXPECT_EQ("", pruebaArch.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienEtiquetasCorrectasMetadatoArchivo){
	ParserJson parser;

	MetadatoArchivo pruebaArch = parser.deserializarMetadatoArchivo(jsonArchOK);

	list<string> etiq = pruebaArch.etiquetas;
	list<string>::iterator itEtiq = etiq.begin();
	int i = 0;
	for ( ; itEtiq != etiq.end() ; itEtiq++, i++){
		if (i==0) EXPECT_EQ("23", (*itEtiq));
		if (i==1) EXPECT_EQ("true", (*itEtiq));
		if (i==2) EXPECT_EQ("juan", (*itEtiq));
	}
}

TEST(ParserJsonTest, deberiaDeserializarBienEtiquetasInexistenteMetadatoArchivo){
	ParserJson parser;
	string jsonArch = "{\n"
			"\t\"extension\" : \"jpg\",\n"
			"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
			"\t\"nombre\" : \"sol\",\n"
			"\t\"propietario\" : \"Pancheitor\",\n"
			"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n"
			"}";
	MetadatoArchivo pruebaArch = parser.deserializarMetadatoArchivo(jsonArch);

	list<string> etiq = pruebaArch.etiquetas;
	EXPECT_EQ(true,etiq.empty());
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreCorrectoMetadatoUsuario){
	ParserJson parser;

	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsuOK);
	EXPECT_EQ("Pancheitor", pruebaUsu.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienNombreInexistenteMetadatoUsuario){
	ParserJson parser;
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : 45.0123,\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsu);
	EXPECT_EQ("", pruebaUsu.nombre);
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionCorrectaMetadatoUsuario){
	ParserJson parser;
	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsuOK);
	EXPECT_FLOAT_EQ(45.0123, pruebaUsu.ultimaUbicacion.latitud);
	EXPECT_FLOAT_EQ(-37.1293, pruebaUsu.ultimaUbicacion.longitud);
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionLatitudInvalidaMetadatoUsuario){
	ParserJson parser;
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"latitud\" : \"hola\",\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsu);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.latitud); //0 es el valor que se le asigna por defecto
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionLatitudInexistenteMetadatoUsuario){
	ParserJson parser;
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
			"\t\"ultima ubicacion\" : {\n"
				"\t \t\"longitud\" : -37.1293\n"
			"\t}\n"
			"}";
	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsu);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.latitud); //0 es el valor que se le asigna por defecto
}

TEST(ParserJsonTest, deberiaDeserializarBienUltimaUbicacionInexistenteMetadatoUsuario){
	ParserJson parser;
	string jsonUsu = "{\n"
			"\t\"email\" : \"panch@eitor.com\",\n"
			"\t\"nombre\" : \"Pancheitor\",\n"
			"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\"\n"
			"}";
	MetadatoUsuario pruebaUsu = parser.deserializarMetadatoUsuario(jsonUsu);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.latitud);
	EXPECT_FLOAT_EQ(0, pruebaUsu.ultimaUbicacion.longitud);
}

TEST(ParserJsonTest, deberiaDeserializarBienUsernameCorrectoMetadatoSesion){
	ParserJson parser;
	MetadatosSesion pruebaSes = parser.deserializarMetadatoSesion(jsonSesOK);
	EXPECT_EQ("pancheitor", pruebaSes.username);
}

TEST(ParserJsonTest, deberiaDeserializarBienUsernameInexistenteMetadatoSesion){
	ParserJson parser;
	string jsonSes = "{\n"
			"\t\"password\" : \"pancho123\",\n"
			"\t\"token\" : \"asg371ns812ssk\"\n"
			"}";
	MetadatosSesion pruebaSes = parser.deserializarMetadatoSesion(jsonSes);
	EXPECT_EQ("", pruebaSes.username);
}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMetadatoArchivo){

}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMetadatoUsuario){

}

TEST(ParserJsonTest, deberiaObtenerLoMismoAlSerializarYDeserializarMetadatoSesion){

}
