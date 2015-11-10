import unittest
import requests
import json
from subprocess import Popen
from time import sleep
from shutil import rmtree
from os.path import exists
from ast import literal_eval


def definirConstantesGlobales():

	global RESERVED_STR
	RESERVED_STR = "!"

	global RESERVED_FIRST
	RESERVED_FIRST = RESERVED_STR + "1"

	global RESERVED_SECOND
	RESERVED_SECOND = RESERVED_STR + "2"

	global FOLDER_TYPE
	FOLDER_TYPE = RESERVED_STR + "folder"

	global TRASH_TYPE
	TRASH_TYPE = RESERVED_STR + "trash/"

	global FOTOS_TYPE
	FOTOS_TYPE = RESERVED_STR + "fotos/"

	global BASE 
	BASE = "http://localhost:8080/"
	
	global PROFILE
	PROFILE = BASE + "profile/"
	
	global SESSION 
	SESSION = BASE + "session/"
	
	global FILE
	FILE = BASE + "file/"

	global METADATA
	METADATA = BASE + "metadata/"

	global FOLDER
	FOLDER = BASE + "folder/"

	global PERFIL 
	PERFIL = '{ "nombre": "Tobias", "email": "santi@pancho.pablo"}' # TODO: completar
	
	global USER 
	USER = {"user":"tobi", "pass" : "masdeocholetras", "profile": PERFIL}
	
	global LOG_USER 
	LOG_USER = USER.copy()
	del LOG_USER["profile"]

	global FILENAME
	FILENAME = "Makefile"

	global FILENAME2
	FILENAME2 = "CMakeCache.txt"

	global PATH
	PATH = USER["user"] + "/" + FILENAME

	global BAD_REQUEST
	BAD_REQUEST = 400

	global NOT_FOUND
	NOT_FOUND = 404

	global UNAUTHORIZED
	UNAUTHORIZED = 401

	global RESOURCE_CREATED
	RESOURCE_CREATED = 201

	global SUCCESS
	SUCCESS = 200

	global UNSUPPORTED_METHOD
	UNSUPPORTED_METHOD = 405


def registrarYLoguear(username, password, profile):
	requests.post(PROFILE, data={"user": username, "pass": password, "profile": profile})
	r = requests.post(SESSION, data={"user": username, "pass": password})
	token = r.json().get("token")
	return str(token)

def registrarYLoguearUser(dictUser):
	return registrarYLoguear(dictUser["user"], dictUser["pass"], dictUser["profile"])

def logTest(testname):
	with open('files/log.txt', 'a') as file:
		file.write("TEST: " + testname + "\n")

def iniciar():
	token = registrarYLoguearUser(USER)
	uri = FILE + PATH
	requests.put(uri + RESERVED_FIRST, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER["user"]})	# sube Makefile!1
	requests.put(uri + RESERVED_SECOND, files={'file': open(FILENAME2, 'rb'), "token": token, "user": USER["user"]})	# sube Makefile!2
	return token



class ServerTest(unittest.TestCase):
	def setUp(self):
		print
		if exists("FileSystem"):
			rmtree("FileSystem")
		self.serverProcess = Popen(["./udrive", "&"])
		sleep(0.05)


	def tearDown(self):
		self.serverProcess.terminate()


	def test_ObtenerLaVersionAnteriorDeUnArchivo(self):
		logTest("ObtenerLaVersionAnteriorDeUnArchivo")
		token = iniciar()

		r = requests.get(FILE + PATH + RESERVED_FIRST, data={"token": token, "user": USER["user"]})
		contenido = open(FILENAME, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(r.content, contenido)

		s = requests.get(FILE + PATH + RESERVED_SECOND, data={"token": token, "user": USER["user"]})
		contenido2 = open(FILENAME2, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(s.content, contenido2)


	def test_borrarYRestaurarVersiones(self):
		logTest("test_borrarYRestaurarVersiones")
		token = iniciar()

		r = requests.delete(FILE + PATH, data={"token" : token, "user" : USER["user"]})
		self.assertEquals(r.status_code, SUCCESS)

		s = requests.delete(FILE + USER["user"] + "/" + TRASH_TYPE + FILENAME + RESERVED_STR + "0", \
			data={"restore" : "true", "token" : token, "user" : USER["user"]})
		self.assertEquals(s.status_code, SUCCESS)

		t = requests.get(FILE + PATH + RESERVED_FIRST, data={"token": token, "user": USER["user"]})
		contenido = open(FILENAME, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(t.content, contenido)

		u = requests.get(FILE + PATH + RESERVED_SECOND, data={"token": token, "user": USER["user"]})
		contenido2 = open(FILENAME2, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(u.content, contenido2)


	def test_versionesRenombradas(self):
		logTest("versionesRenombradas")
		token = iniciar()
		otroToken = registrarYLoguear("PABLO", "password", PERFIL)

		metadatosPermiso = '{"propietario" : "' +  USER["user"] + '" , "extension" : "none", "nombre" : "Makefile", "etiquetas" : [], "usuarios" : ["tobi", "PABLO"], ' \
			+ '"usuario ultima modificacion" : "' + USER["user"] + '" , "fecha ultima modificacion" : "23/10/2015"}'
		r = requests.put(METADATA + PATH, data={"user" : USER["user"], "token" : token, "metadatos" : metadatosPermiso})	# le da permiso a PABLO
		self.assertEquals(r.status_code, SUCCESS)

		requests.put(FILE + PATH + RESERVED_STR + "3", files={'file': open(FILENAME, 'rb'), "token": token, "user": USER["user"]})	# sube Makefile!3

		metadatosRenombre = '{"propietario" : "' +  USER["user"] + '" , "extension" : "NUEVA", "nombre" : "NUEVO", "etiquetas" : [], "usuarios" : ["tobi", "PABLO"], ' \
			+ '"usuario ultima modificacion" : "PABLO" , "fecha ultima modificacion" : "23/10/2015"}'
		s = requests.put(METADATA + PATH, data={"user" : "PABLO", "token" : otroToken, "metadatos" : metadatosRenombre})
		self.assertEquals(s.status_code, SUCCESS)

		requests.put(FILE + USER["user"] + "/NUEVO.NUEVA!4", files={'file': open(FILENAME, 'rb'), "token": token, "user": USER["user"]})	# sube Makefile!3

		t = requests.get(FOLDER + USER["user"], data={"user" : USER["user"], "token" : token})
		estructuraReal = literal_eval(t.content)
		estructuraEsperada = {USER["user"] + "/NUEVO.NUEVA" : "NUEVO.NUEVA" + RESERVED_STR + "4", USER["user"] + "/!trash" : "!trash."+ FOLDER_TYPE}
		expected = {"estructura" :  estructuraEsperada}
		self.assertDictEqual(expected, estructuraReal)





if __name__ == '__main__':
	definirConstantesGlobales()
	unittest.main()
