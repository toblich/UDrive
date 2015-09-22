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
	  password = "pancheitor";
	  perfil = "{\"pablo\": \"santi\"}";	// TODO: cambiar a perfil completo
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
   string password;
   string perfil;
   ManejadorDeUsuarios* manejador;
   MapDB* perfiles;
   MapDB* sesiones;
   MapDB* passwords;
};

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

TEST_F(ManejadorDeUsuariosTest, noDeberiaPermitirCrearUsuarioConUsernameConBarra) {
	string uname1 = "santi/tobi";
	string uname2 = "/";
	EXPECT_FALSE(manejador->registrarUsuario(uname1, password, perfil));
	EXPECT_FALSE(manejador->registrarUsuario(uname2, password, perfil));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaPermitirCrearUsuarioConUsernameConEspacio) {
	string uname1 = "santi tobi";
	string uname2 = " ";
	EXPECT_FALSE(manejador->registrarUsuario(uname1, password, perfil));
	EXPECT_FALSE(manejador->registrarUsuario(uname2, password, perfil));
}

TEST_F(ManejadorDeUsuariosTest, noDeberiaPermitirCrearUsuarioConPasswordCorta) {
	string passw = "corta";
	EXPECT_FALSE(manejador->registrarUsuario(username, passw, perfil));
}
