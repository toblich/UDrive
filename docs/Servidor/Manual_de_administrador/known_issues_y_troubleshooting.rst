============================================================================================================
Known Issues y Troubleshooting
============================================================================================================
Al momento de lanzar el Release Candidate se encontraron los siguientes Known Issues (bugs dentro del servidor que no fueron solucionados) y los siguientes Troubleshooting (bugs que pueden llegar a ocurrir raramente en el servidor con una explicación de como solucionarlo).

Known Issues
<<<<<<<<<<<<<<<<<<<<
* El tamaño del path que abarca desde el root hasta la ubicación física del archivo no puede tener mas de 1024 caracteres.
* El tamaño del nombre de archivos no puede tener mas de 1024 caracteres.
* El valor(string) que tienen los parámetros de la conexión que maneja la API REST no puede tener mas de 1024 caracteres.
* El campo "Nombre" dento del perfil de un usuario no acepta acentos ni caracteres restringidos.
* El campo "mail" dentro del perfil solo puede tener 1 o mas puntos al final, no en la parte que está a la izquierda del @.
* No se pueden subir archivos o carpetas con espacios.


Troubleshooting
<<<<<<<<<<<<<<<<<<
* Dependiendo de donde se corra *UDrive* puede no funcionar bien. Esto se debe a que si no se ejecuta dentro de la carpeta *build* del *Servidor* hay problemas con los distintos path a los archivos y se soluciona ejecutando *UDrive* tal como se indica en la sección `Compilación y ejecución <forma_de_uso.html>`_.
* A veces alguno de los test de python puede fallar por supuestos problemas en la conexión. Esto se debe al parámetro que se le pasa al *sleep* dentro del *Set Up* y se soluciona agrandando el valor que se le pasa al mismo.


En caso de que surja cualquier otro error no mencionado anteriormente, por favor comunicarse a alguna de las direcciones de correo electrónico de los desarrolladores:
 + `Pablo Ciruzzi <p.ciruzzi@hotmail.com>`_
 + `Santiago Fernández <fernandezsantid@gmail.com>`_
 + `Tobías Lichtig <toblich@gmail.com>`_