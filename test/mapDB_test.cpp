#include "mapDB.h"

#include <gtest/gtest.h>

using namespace std;

class MapaDBTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
	  db = new MapDB();
  }

   virtual void TearDown() {
	   db->deleteBD();
	   delete db;
   }

  BD* db;
};

// Prueba de GTest, como test es trivial (y solo prueba RocksDB)
TEST_F(MapaDBTest, deberiaRecuperarElMismoValorQueGuarda) {
	db->put("key", "value");
	EXPECT_EQ("value", db->get("key"));
}

TEST_F(MapaDBTest, deberiaDevolverFalseAlRepetirPutDeMismaClave) {
	db->put("key", "1");
	EXPECT_FALSE(db->put("key", "2"));
}

TEST_F(MapaDBTest, deberiaPoderModificarElValueDeUnaKeyExistente) {
	db->put("key", "value");
	db->modify("key", "hola");
	EXPECT_EQ("hola", db->get("key"));
}


/* Tests de Excepciones */

TEST_F(MapaDBTest, deberiaLanzarKeyNotFoundAlBuscarClaveInexistente) {
	EXPECT_THROW(db->get("key"), KeyNotFound);
}

TEST_F(MapaDBTest, deberiaLanzarKeyNotFoundAlEliminarClaveInexistente) {
	EXPECT_THROW(db->erase("key"), KeyNotFound);
}

TEST_F(MapaDBTest, deberiaLanzarKeyNotFoundAlModificarClaveInexistente) {
	EXPECT_THROW(db->modify("key", "value2"), KeyNotFound);
}
