#include "basededatos.h"
#include <gtest/gtest.h>

using namespace std;

class DBTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
	  db = new BaseDeDatos();
  }

   virtual void TearDown() {
	   db->deleteBD();
	   delete db;
   }

  BaseDeDatos* db;
};

// Prueba de GTest, como test es trivial (y solo prueba RocksDB)
TEST_F(DBTest, deberiaRecuperarElMismoValorQueGuarda) {
	db->put("key", "value");
	EXPECT_EQ("value", db->get("key"));
}

TEST_F(DBTest, deberiaDevolverFalseAlRepetirPutDeMismaClave) {
	db->put("key", "1");
	EXPECT_FALSE(db->put("key", "2"));
}


/* Tests de Excepciones */

TEST_F(DBTest, deberiaLanzarKeyNotFoundAlBuscarClaveInexistente) {
	EXPECT_THROW(db->get("key"), KeyNotFound);
}

TEST_F(DBTest, deberiaLanzarKeyNotFoundAlEliminarClaveInexistente) {
	EXPECT_THROW(db->erase("key"), KeyNotFound);
}

TEST_F(DBTest, deberiaLanzarKeyNotFoundAlModificarClaveInexistente) {
	EXPECT_THROW(db->modify("key", "value2"), KeyNotFound);
}


