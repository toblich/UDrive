#include "server.h"

using namespace std;

Server::Server(std::string listeningPort, BD* perfiles, BD* sesiones, BD* passwords) {
	server = mg_create_server((void *) this, Server::mgEventHandler);
	mg_set_option(server, "listening_port", listeningPort.c_str());
	mg_set_option(server, "document_root", ".");

	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;

	manejador = new ManejadorDeUsuarios(perfiles, sesiones, passwords);

}

Server::~Server() {
	mg_destroy_server(&server);
	delete manejador;
	//TODO: Sacar estas instrucciones para que despues persistan los datos.
	perfiles->deleteBD(); //
	sesiones->deleteBD(); //
	passwords->deleteBD(); //
	delete perfiles;
	delete sesiones;
	delete passwords;
}

int Server::mgEventHandler(mg_connection* connection, mg_event event) {
	Server* _server = (Server*) connection->server_param;
	return _server->eventHandler(connection, event);
}

enum mg_result Server::eventHandler(mg_connection* connection, mg_event event) {
	switch (event) {

		case MG_AUTH: return MG_TRUE;

		case MG_REQUEST: return requestHandler(connection);

		case MG_CLOSE: return closeHandler(connection);

		default: return MG_FALSE;
	}
}

enum mg_result Server::closeHandler(mg_connection* connection){
	if(connection){
		free(connection->connection_param);
		connection->connection_param = NULL;
	}
	return MG_TRUE;
}

void Server::pollServer(int milliseconds) {
	mg_poll_server(server, milliseconds);
}

string Server::mensajeSegunURI(string uri) {
	if (uri == "/")
		return "Root";
	if (uri == "/santi")
		return "Roro";
	else return "Te equivocaste MUAJAJAJAJA";
}

enum mg_result Server::requestHandler(mg_connection* connection) {
	string verb = string(connection->request_method);
	if (verb == "GET"){
		return GETHandler(connection);
	}
	else if (verb == "POST"){
		return POSTHandler(connection);
	}
	else if (verb == "PUT"){
		return PUTHandler(connection);
	}
	else if (verb == "DELETE"){
		return DELETEHandler(connection);
	}
	else{
		cout << "Verbo no existente: " << verb << endl;
		return MG_TRUE;
	}
}

enum mg_result Server::GETHandler(mg_connection* connection) {
	string uri(connection->uri);
	cout << "Estoy en get" << endl;
	if (uri == "/archivo"){
		cout << "Estoy en archivo" << endl;
		mg_send_file(connection, "Facebook_icon.jpg", NULL);
		return MG_MORE;
	}else {
		mg_printf_data(connection, "Hola!: [%s]\n", Server::mensajeSegunURI(uri).c_str());
		return MG_TRUE;
	}
}

enum mg_result Server::PUTHandler(mg_connection* connection) {
	//Recibe un archivo de texto
	string uri(connection->uri);
	cout << "Estoy en put" << endl;
	string filename = uri.substr(1, uri.length()-1); // Supone que no hay carpetas
	ofstream outFile(filename.data(), std::ofstream::binary);
	outFile.write(connection->content, connection->content_len);
	outFile.close();
	mg_printf_data(connection, "PUT [%s]\n", Server::mensajeSegunURI(uri).c_str());
	return MG_TRUE;
}

enum mg_result Server::POSTHandler(mg_connection* connection) {
	//Recibe un archivo binario
	string uri(connection->uri);
	cout << "Estoy en post" << endl;

	if (uri == "/reg"){
		//TODO:implementar la parte de registracion.
		string username = "root";
		string password = "administrador";
		string perfil = "{\"pablo\": \"santi\"}";
		manejador->registrarUsuario(username, password, perfil);
		mg_printf_data(connection, "Usuario registrado\n");

	}

	if (uri == "/login"){
		string user = getVar(connection, "user");
		string pass = getVar(connection, "pass");

		if(manejador->validarLogin(user, pass)){
			string token = manejador->iniciarSesion(user);
			mg_printf_data(connection, "Token de sesion: %s\n", token.c_str());
		}else{
			mg_printf_data(connection, "No es posible conectar, usuario o contraseña incorrecto\n");
		}
	}

	if (uri == "/upload"){
		return getMultipartData(connection);
	}

	return MG_TRUE;
}

string Server::getVar(mg_connection* connection, string varName){
	string value;
	int max = 100;
	int n = 0;

	value.resize(max);
	mg_get_var_n(connection, varName.c_str(), (char*) value.data(), max, n);

	value.resize(strlen(value.data()));

	mg_printf_data(connection, "Variable: '%s'\n", value.c_str());

	return value;
}

enum mg_result Server::DELETEHandler(mg_connection* connection) {
	string uri(connection->uri);
	cout << "Estoy en delete" << endl;
	mg_printf_data(connection, "DELETE [%s]\n", Server::mensajeSegunURI(uri).c_str());
	return MG_TRUE;
}

enum mg_result Server::getMultipartData(mg_connection* connection){
	const char *data;
	int data_len, n1, n2;
	char var_name[100], file_name[100];

	n1 = n2 = 0;

	while ((n2 = mg_parse_multipart(connection->content + n1, connection->content_len - n1,
				   var_name, sizeof(var_name), file_name, sizeof(file_name), &data, &data_len)) > 0) {
		mg_printf_data(connection, "var: %s, file_name: %s, size: %d bytes\n", var_name, file_name, data_len);
		n1 += n2;
	}
	ofstream outFile(file_name, std::ofstream::binary);
	outFile.write(data, data_len);
	outFile.close();
	return MG_TRUE;
}
