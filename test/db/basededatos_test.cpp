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

TEST_F(DBTest, deberiaPoderModificarElValueDeUnaKeyExistente) {
	db->put("key", "value");
	db->modify("key", "hola");
	EXPECT_EQ("hola", db->get("key"));
}


TEST_F(DBTest, deberiaEjecutarUnBatchDePutsCorrectoCompleto) {
	Batch batch;
	batch.put("key", "value");
	batch.put("key2", "value2");
	db->writeBatch(batch);
	EXPECT_EQ("value", db->get("key"));
	EXPECT_EQ("value2", db->get("key2"));
}

TEST_F(DBTest, deberiaEjecutarUnBatchModifyEraseCorrectoCompleto) {
	db->put("key", "value");
	db->put("key2", "value2");
	Batch batch;
	batch.modify("key", "otro");
	batch.erase("key2");
	EXPECT_TRUE(db->writeBatch(batch));
	EXPECT_EQ("otro", db->get("key"));
	EXPECT_FALSE(db->contains("key2"));
}

TEST_F(DBTest, noDeberiaEjecutarBatchEnQueFallaraUnaAccion) {
	Batch batch;
	batch.put("key", "value");
	batch.modify("key2", "value2"); // No existe key2, este falla
	EXPECT_FALSE(db->writeBatch(batch));
	EXPECT_THROW(db->get("key"), KeyNotFound);
}

TEST_F(DBTest, deberiaLanzarKeyNotFoundAlBuscarClaveInexistente) {
	EXPECT_THROW(db->get("key"), KeyNotFound);
}

TEST_F(DBTest, deberiaLanzarKeyNotFoundAlEliminarClaveInexistente) {
	EXPECT_THROW(db->erase("key"), KeyNotFound);
}

TEST_F(DBTest, deberiaLanzarKeyNotFoundAlModificarClaveInexistente) {
	EXPECT_THROW(db->modify("key", "value2"), KeyNotFound);
}
