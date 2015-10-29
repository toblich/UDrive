API REST
=============================

Para que los diferentes clientes se puedan conectar y comunicar con el servidor de *UDrive*, éste debe brindar una interfaz para la comunicación. Para el desarrollo de esta interfaz se utilizó un protocolo REST (Restful API), donde se definieron las formas en que el cliente enviaba las solicitudes y el servidor enviaba las respuestas de las mismas. 

.. note:: 
	Exceptuando el registro y el inicio de sesión, todo pedido debe contener los parámetros **username** y **token** con el nombre de usuario y el token de la sesión correspondiente. De faltar estos campos o ser incorrectos, el servidor responderá con el código HTTP 401 (*UNAUTHORIZED*). En algunos casos, como para cerrar sesión, dichos valores forman parte de la URL y por lo tanto no es necesario repetirlos como parámetro.


Usuarios
---------------------------------------------------------------------------------------------------------------

- Registrar:  ``POST`` a ``/profile``
	+ *user* - nombre de usuario
	+ *pass* - contraseña
	+ *profile* - JSON con el perfil
- Actualizar el perfil: ``PUT`` a ``/profile/<username>``
	+ *nombre* - nombre real en el perfil
	+ *email* - dirección de correo electrónico
	+ *picture* - foto de perfil
- Buscar usuarios: ``GET`` a ``/profile``
	+ *busqueda* - parte del nombre a buscar
- Consultar el perfil:  ``GET`` a ``/profile/<username>``

Sesiones
---------------------------------------------------------------------------------------------------------------

- Login:  ``POST`` a ``/session``
	+ *user* - nombre de usuario
	+ *pass* - contraseña
- Logout:  ``DELETE`` a ``/session/<token>``

Archivos
---------------------------------------------------------------------------------------------------------------
- Subir:  ``PUT`` a  ``/file/<username_de_propietario>/<path>``
	+ *file* - archivo
	+ *latitud* - latitud de la ubicación actual
	+ *longitud* - longitud de la ubicación actual
- Restaurar:  ``DELETE`` a ``/file/<username_de_propietario>/<path>``
	+ *restore* - con valor "true"
- Eliminar:  ``DELETE`` a ``/file/<username_de_propietario>/<path>``
- Bajar:  ``GET`` a ``/file/<username_de_propietario>/<path>``

Búsquedas de archivos
---------------------------------------------------------------------------------------------------------------
- Busquedas en general: ``GET`` a ``/metadata/``, con **alguno** de los siguientes parámetros:
	+ *nombre* - parte del nombre del archivo
	+ *propietario* - nombre exacto del propietario del archivo
	+ *etiqueta* - etiqueta que debe tener el archivo
	+ *extensión* - extensión exacta del archivo

Metadatos
---------------------------------------------------------------------------------------------------------------
- Actualizar: ``PUT`` a ``/metadata/<username_de_propietario>/<filename>``
	+ *metadatos* - JSON con los nuevos metadatos completos. Una lista vacía de usuarios habilitados significa mantener la lista actual.
- Consultar: ``GET`` a ``/metadata/<username_de_propietario>/<filename>``


Carpetas
---------------------------------------------------------------------------------------------------------------
- Crear:  ``PUT`` a ``/folder/<username_de_propietario>/<path>``
- Obtener estructura:  ``GET`` a ``/folder/<username_de_propietario>/<path>``
- Eliminar:  ``DELETE`` a ``/folder/<username_de_propietario>/<path>``
