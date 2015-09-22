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

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathComun) {
	std::string path = "hola/pablo/como/estas";

	vector<string> directorios = manejador->parsearDirectorios(path);
	vector<string>::iterator itDir = directorios.begin();
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) EXPECT_EQ("hola", (*itDir));
		else {
			if (i==1) EXPECT_EQ("pablo", (*itDir));
			else {
				if (i==2) EXPECT_EQ("como", (*itDir));
				else if (i==3) EXPECT_EQ("estas", (*itDir));
					else EXPECT_TRUE(false);
			}
		}
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathConEspacios) {
	std::string path = "hola/pablo/como estas";

	vector<string> directorios = manejador->parsearDirectorios(path);
	vector<string>::iterator itDir = directorios.begin();
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) EXPECT_EQ("hola", (*itDir));
		if (i==1) EXPECT_EQ("pablo", (*itDir));
		if (i==2) EXPECT_EQ("como estas", (*itDir));
	}
}
