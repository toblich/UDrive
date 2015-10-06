import unittest
import requests
from subprocess import Popen
from time import sleep
import json


def definirConstantesGlobales():
	global BASE 
	BASE = "http://localhost:8080/"
	
	global PROFILE
	PROFILE = BASE + "profile/"
	
	global SESSION 
	SESSION = BASE + "session/"
	
	global FILE
	FILE = BASE + "file/"

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

	global RESOURCE_CREATED
	RESOURCE_CREATED = 201

	global SUCCESS
	SUCCESS = 200


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
		self.serverProcess = Popen(["./udrive", "&"])


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

		s = requests.put(PROFILE + username, data={"nombre": "otroNombre", "email": 'otro@e.mail',
			"token": token, "latitud" : "0.0", "longitud" : 10.0}) # actualizar perfil
		self.assertEquals(s.status_code, SUCCESS)
		t = requests.get(PROFILE + username, data={"token": token})	# obtener nuevo perfil
		self.assertEquals(t.status_code, SUCCESS)
		nuevoPerfilObtenido = t.json().get("perfil")
		self.assertEquals(nuevoPerfilObtenido.get("nombre"), "otroNombre")
		self.assertEquals(nuevoPerfilObtenido.get("email"), "otro@e.mail")
		self.assertEquals(nuevoPerfilObtenido.get("ultima ubicacion").get("latitud"), 0.0)
		self.assertEquals(nuevoPerfilObtenido.get("ultima ubicacion").get("longitud"), 10.0)


	def test_subirBajarYBorrarArchivoTexto(self):
		# TODO: borrar
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "Makefile"
		# Popen(["rm", "FileSystem/" + USER_SIMPLE["user"] + "/" + FILENAME])	# elimina el archivo, si llega a existir

		uri = FILE + USER_SIMPLE["user"] + "/" + FILENAME
		r = requests.put(uri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# lo sube
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.get(uri, data={"user": USER_SIMPLE["user"], "token": token})	# lo baja
		self.assertEquals(s.status_code, SUCCESS)

		contenido = open(FILENAME, 'rb').read()
		self.assertEquals(s.content, contenido)

		t = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(t.status_code, SUCCESS)

		# u = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # trata de borrar otra vez
		# self.assertEquals(t.status_code, NOT_FOUND)


	def test_obtenerYActualizarMetadatos(self):
		pass



if __name__ == '__main__':
	definirConstantesGlobales()
	unittest.main()