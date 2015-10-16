import unittest
import requests
import json
from subprocess import Popen
from time import sleep
from shutil import rmtree
from os.path import exists
from ast import literal_eval


def definirConstantesGlobales():
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
	
	global USER_SIMPLE 
	USER_SIMPLE = {"user":"tobi", "pass" : "masdeocholetras", "profile": PERFIL}
	
	global LOG_USER_SIMPLE 
	LOG_USER_SIMPLE = USER_SIMPLE.copy()
	del LOG_USER_SIMPLE["profile"]

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



class ServerTest(unittest.TestCase):
	def setUp(self):
		print
		if exists("FileSystem"):
			rmtree("FileSystem")
		self.serverProcess = Popen(["./udrive", "&"])
		sleep(0.1)


	def tearDown(self):
		requests.get(BASE + "close")
		self.serverProcess.wait()


	def test_registroYLogueoValido(self):
		r = requests.post(PROFILE, data=USER_SIMPLE)
		self.assertEquals(r.status_code, RESOURCE_CREATED)
		s = requests.post(SESSION, LOG_USER_SIMPLE)
		self.assertEquals(s.status_code, RESOURCE_CREATED)


	def test_registrarUsuarioConPasswordInvalida(self):
		userCroto = USER_SIMPLE.copy()
		userCroto['pass'] = "corta"
		r = requests.post(PROFILE, data=userCroto)
		self.assertEquals(r.status_code, BAD_REQUEST)


	def test_logueoInvalido(self):
		requests.post(PROFILE, data=USER_SIMPLE) #registro
		log_erroneo = LOG_USER_SIMPLE.copy()
		log_erroneo["pass"] = "equivocada"
		r = requests.post(SESSION, log_erroneo)
		self.assertEquals(r.status_code, BAD_REQUEST)


	def test_loguearYDesloguearMultiplesSesiones(self):
		r = requests.post(PROFILE, data=USER_SIMPLE)	# registro
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.post(SESSION, LOG_USER_SIMPLE)		# 1er log
		self.assertEquals(s.status_code, RESOURCE_CREATED)
		s.token = str(s.json().get('token'))

		t = requests.post(SESSION, LOG_USER_SIMPLE)		# 2do log
		self.assertEquals(t.status_code, RESOURCE_CREATED)
		t.token = str(t.json().get('token'))

		self.assertNotEquals(s.token, t.token)	# tokens distintos por ser sesiones distintas
		
		u = requests.delete(SESSION + s.token, data={"user" : USER_SIMPLE["user"]})	# 1er logout
		self.assertEquals(u.status_code, SUCCESS)
		
		v = requests.delete(SESSION + s.token, data={"user" : USER_SIMPLE["user"]})	# 1er logout repetido
		self.assertEquals(v.status_code, NOT_FOUND)
		
		w = requests.delete(SESSION + t.token, data={"user" : USER_SIMPLE["user"]})	# 2do logout
		self.assertEquals(w.status_code, SUCCESS)


	def test_perfilSubidoYActualizado(self):
		username = "hola"
		password = "masdeocholetras"
		token = registrarYLoguear(username, password, PERFIL)
		perfilOriginal = json.loads(PERFIL)

		r = requests.get(PROFILE + username, data={"token": token})	# obtener perfil
		self.assertEquals(r.status_code, SUCCESS)
		perfilObtenido = r.json().get("perfil")
		self.assertEquals(perfilObtenido.get("nombre"), perfilOriginal.get("nombre"))
		self.assertEquals(perfilObtenido.get("email"), perfilOriginal.get("email"))

		s = requests.put(PROFILE + username, data={"profile": '{"nombre": "otroNombre", "email": "otro@e.mail"}',
			"token": token}) # actualizar perfil
		self.assertEquals(s.status_code, SUCCESS)
		
		t = requests.get(PROFILE + username, data={"token": token})	# obtener nuevo perfil
		self.assertEquals(t.status_code, SUCCESS)
		nuevoPerfilObtenido = t.json().get("perfil")
		self.assertEquals(nuevoPerfilObtenido.get("nombre"), "otroNombre")
		self.assertEquals(nuevoPerfilObtenido.get("email"), "otro@e.mail")


	def test_subirBajarYBorrarArchivoTexto(self):
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "Makefile"

		uri = FILE + USER_SIMPLE["user"] + "/" + FILENAME
		r = requests.put(uri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# lo sube
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.get(uri, data={"user": USER_SIMPLE["user"], "token": token})	# lo baja
		self.assertEquals(s.status_code, SUCCESS)

		contenido = open(FILENAME, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(s.content, contenido)

		t = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(t.status_code, SUCCESS)

		u = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # trata de borrar otra vez
		self.assertEquals(u.status_code, NOT_FOUND)

		v = requests.get(uri, data={"user": USER_SIMPLE["user"], "token": token})	# trata de bajarlo
		self.assertEquals(v.status_code, NOT_FOUND)


	def test_obtenerYActualizarMetadatos(self):
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "../src/db/batch.cpp"

		internalUri = USER_SIMPLE["user"] + FILENAME[2:] # saca los '..'
		requests.put(FILE + internalUri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# sube archivo

		r = requests.get(METADATA + internalUri, data={"user": USER_SIMPLE["user"], "token": token})	# consulta metadatos
		self.assertEquals(r.status_code, SUCCESS)

		metadata = r.json().get("metadatos")
		self.assertEquals(metadata.get("propietario"), USER_SIMPLE["user"])
		self.assertEquals(metadata.get("nombre"), "batch")
		self.assertEquals(metadata.get("usuario ultima modificacion"), USER_SIMPLE["user"])
		self.assertEquals(metadata.get("extension"), "cpp")
		self.assertEquals(metadata.get("etiquetas"), [])
		self.assertEquals(metadata.get("usuarios"), [USER_SIMPLE["user"]])

		t = requests.delete(FILE + internalUri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(t.status_code, SUCCESS)


	def test_obtenerEstructuraDeCarpetaYBorrarCarperta(self):
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "../src/db/batch.cpp"
		BASE_FOLDER = FOLDER + USER_SIMPLE["user"] + "/src/"

		internalUri = USER_SIMPLE["user"] + FILENAME[2:] # saca los '..'
		requests.put(FILE + internalUri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# sube archivo

		r = requests.put(BASE_FOLDER + "subcarpeta", data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.get(BASE_FOLDER, data={"token": token, "user": USER_SIMPLE["user"]})	#obtiene la estructura de /CMakeFiles
		self.assertEquals(s.status_code, SUCCESS)

		estructura = literal_eval(s.content)
		expected = {"estructura" : {"db" : "#folder", "subcarpeta": "#folder"} }
		self.assertDictEqual(expected, estructura) 

		t = requests.delete(BASE_FOLDER, data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(t.status_code, SUCCESS)

		u = requests.delete(BASE_FOLDER, data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(u.status_code, NOT_FOUND)

		v = requests.delete(BASE_FOLDER, data={"token": 1234567890, "user": USER_SIMPLE["user"]})
		self.assertEquals(v.status_code, UNAUTHORIZED)


	def test_DeberiaDarErrorAlQuererIngresarAUnRecursoInexistente(self):
		r = requests.get(BASE)
		self.assertEquals(r.status_code, NOT_FOUND)

		INEXISTENTE = BASE + "/sarasa"
		s = requests.get(INEXISTENTE)
		self.assertEquals(s.status_code, NOT_FOUND)


	def test_DeberiaDarErrorAlQuererUsarUnMetodoNoSoportado(self):
		r = requests.get(SESSION)
		self.assertEquals(r.status_code, UNSUPPORTED_METHOD)

		s = requests.put(SESSION)
		self.assertEquals(s.status_code, UNSUPPORTED_METHOD)

		t = requests.post(FOLDER)
		self.assertEquals(t.status_code, UNSUPPORTED_METHOD)

		u = requests.delete(METADATA)
		self.assertEquals(u.status_code, UNSUPPORTED_METHOD)


	def test_DeberiaDarErrorAlAccederConTokenErroneo(self):
		token = registrarYLoguearUser(USER_SIMPLE)
		r = requests.get(PROFILE + USER_SIMPLE["user"], data={"user": USER_SIMPLE["user"], "token": "123456789"})
		self.assertEquals(r.status_code, UNAUTHORIZED)



if __name__ == '__main__':
	definirConstantesGlobales()
	unittest.main()
