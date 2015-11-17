============================================================================================================
Known Issues y Troubleshooting
============================================================================================================
Al momento de lanzar el Release Candidate se encontraron los siguientes Known Issues (bugs dentro del servidor que no fueron solucionados) y los siguientes Troubleshooting (bugs que pueden llegar a ocurrir raramente en el servidor con una explicación de como solucionarlo).

Known Issues
<<<<<<<<<<<<<<<<<<<<

* El tamaño del path que abarca desde el root hasta la ubicación física del archivo no puede tener mas de 1024 caracteres.
* --> tambien nombre de archivos, y el value de las variables que se pasan como parametros de la conexion.
* El campo "Nombre" dento del perfil de un usuario no acepta acentos ni caracteres restringidos.
* El campo "mail" dentro del perfil solo puede tener . al final.
* El UDrive se corre desde la carpeta build, sino error, solo se puede correr el build.sh de afuera
* No se pueden subir archivos o carpetas con espacios .


Troubleshooting
<<<<<<<<<<<<<<<<<<

* Si llegan a fallar los test de python por algun error en la conexion es por el sleep en los test, se soluciona agrandando ese sleep dentor del Set Up.


En caso de que surja cualquier otro error no mencionado anteriormente, por favor comunicarse a alguna de las direcciones de correo electrónico de los desarrolladores:
 + `Pablo Ciruzzi <p.ciruzzi@hotmail.com>`_
 + `Santiago Fernández <fernandezsantid@gmail.com>`_
 + `Tobías Lichtig <toblich@gmail.com>`_