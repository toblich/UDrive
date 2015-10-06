#include "manejadorDeUsuarios.h"
#include "mapDB.h"
#include <gtest/gtest.h>

using namespace std;

class ManejadorDeUsuariosTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
	  perfiles = new MapDB();
	  sesiones = new MapDB();
	  passwords = new MapDB();
	  manejador = new ManejadorDeUsuarios(perfiles, sesiones, passwords);
	  username = "tobi";
	  unregisteredUsername = "nombreSinRegistrar";
	  password = "pancheitor";
	  perfil = "{\n"
				"\t\"email\" : \"panch@eitor.com.ar\",\n"
				"\t\"nombre\" : \"Pancheitor\",\n"
				"\t\"path foto de perfil\" : \"fotos/pancheitor.jpg\",\n"
				"\t\"ultima ubicacion\" : {\n"
					"\t \t\"latitud\" : 45.0123,\n"
					"\t \t\"longitud\" : -37.1293\n"
				"\t}\n"
				"}";
	  manejador->registrarUsuario(username, password, perfil);
  }

   virtual void TearDown() {
	   delete manejador;
	   perfiles->deleteBD();
	   sesiones->deleteBD();
	   passwords->deleteBD();
	   delete perfiles;
	   delete sesiones;
	   delete passwords;
   }

   string username;
   string unregisteredUsername;
   string password;
   string perfil;
   ManejadorDeUsuarios* manejador;
   BD* perfiles;
   BD* sesiones;
   BD* passwords;
};

TEST_F(ManejadorDeUsuariosTest, previo_testsuite_validacion_datos_testfile) {
	EXPECT_TRUE(manejador->registrarUsuario(unregisteredUsername, password, perfil));
}

TEST_F(ManejadorDeUsuariosTest, deberiaValidarLoginDeUsuarioRecienRegistrado) {
	EXPECT_TRUE(manejador->validarLogin(username, password));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaValidarLoginConPasswordErronea) {
	EXPECT_FALSE(manejador->validarLogin(username, "basura"));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaValidarLoginDeUsuarioInexistente){
	EXPECT_FALSE(manejador->validarLogin("inexistente", password));
}

TEST_F(ManejadorDeUsuariosTest, deberiaCerrarSesionRecienIniciada) {
	string token = manejador->iniciarSesion(username);
	EXPECT_TRUE(manejador->cerrarSesion(token, username));
}

TEST_F(ManejadorDeUsuariosTest, deberiaAutenticarTokenCorrecto) {
	string token = manejador->iniciarSesion(username);
	EXPECT_TRUE(manejador->autenticarToken(token, username));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaAutenticarTokenErroneo) {
	string token = manejador->iniciarSesion(username);
	EXPECT_FALSE(manejador->autenticarToken("espero que sea distinto", username));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaCerrarSesionDeTokenErroneo) {
	string token = manejador->iniciarSesion(username);
	EXPECT_FALSE(manejador->cerrarSesion("erroneo", username));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaCerrarSesionConTokenDeOtroUsuario) {
	string token = manejador->iniciarSesion(username);
	EXPECT_FALSE(manejador->cerrarSesion(token, "otro"));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaPermitirCrearUsuarioConUsernameConCaracteresInvalidos) {
	string uname1 = "santi/tobi";
	string uname2 = "#";
	string uname3 = "hola~";
	string uname4 = "&chau";
	string uname5 = "what?";
	EXPECT_FALSE(manejador->registrarUsuario(uname1, password, perfil));
	EXPECT_FALSE(manejador->registrarUsuario(uname2, password, perfil));
	EXPECT_FALSE(manejador->registrarUsuario(uname3, password, perfil));
	EXPECT_FALSE(manejador->registrarUsuario(uname4, password, perfil));
	EXPECT_FALSE(manejador->registrarUsuario(uname5, password, perfil));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaPermitirCrearUsuarioConPasswordCorta) {
	string passw = "corta";
	EXPECT_FALSE(manejador->registrarUsuario(username, passw, perfil));
}

inline string perfilConNombre(const string& nombre) {
	return "{ \"nombre\": \"" + nombre + "\"}";
};

TEST_F(ManejadorDeUsuariosTest, noDeberiaCrearUsuariosConNombresEnPerfilInvalidos) {
	list<string> invalidos;
	invalidos.push_back("Ju4n");
	invalidos.push_back("/");
	invalidos.push_back("Hola.");
	invalidos.push_back("á");

	list<string>::const_iterator it = invalidos.cbegin();
	const list<string>::const_iterator END = invalidos.cend();
	for (; it != END; it++) {
		string perfilInvalido = perfilConNombre(*it);
		EXPECT_FALSE(manejador->registrarUsuario(unregisteredUsername, password, perfilInvalido));
	}
}

inline string perfilConEmail(const string& email) {
	return "{ \"email\": \"" + email + "\"}";
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaCrearUsuariosConEmailsInvalidos) {
	list<string> invalidos;
	invalidos.push_back("con espacio@gmail.com");
	invalidos.push_back("sindominio.gmail.com");
	invalidos.push_back("sin.com@mailinator");
	invalidos.push_back("con_caracter_invÁlido@yahoo.com.ar");

	list<string>::const_iterator it = invalidos.cbegin();
	const list<string>::const_iterator END = invalidos.cend();
	for (; it != END; it++) {
		string perfilInvalido = perfilConEmail(*it);
		EXPECT_FALSE(manejador->registrarUsuario(unregisteredUsername, password, perfilInvalido));
	}
}

