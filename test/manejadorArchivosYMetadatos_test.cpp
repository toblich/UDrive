#include "manejadorArchivosYMetadatos.h"
#include "basededatos.h"
#include <gtest/gtest.h>

using namespace std;

class ManejadorArchivosYMetadatosTest : public ::testing::Test {
	protected:
		virtual void SetUp() {
			db = new BaseDeDatos(); //TODO cuando mergiemos habria que ponerle un nombre pirulo para que no joda
			manejador = new ManejadorArchivosYMetadatos(db, "FileSystem_test");
		}

		virtual void TearDown() {
			delete manejador;
			db->deleteBD();
			delete db;
		}

		BaseDeDatos* db;
		ManejadorArchivosYMetadatos* manejador;
};

TEST_F(ManejadorArchivosYMetadatosTest, asd) {
	EXPECT_TRUE(true);
}
