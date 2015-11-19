============================================================================================================
Mantenimiento
============================================================================================================

Para el correcto mantenimiento del *Servidor* de *UDrive* existen tres aspectos a tener en cuenta:
					+ El FileSystem 
					+ Las Bases de Datos
					+ El Log

Las *Bases de Datos* y el *FileSystem* son las partes más importantes del *Servidor* ya que es en donde se guarda toda la información
de los usuarios. Por lo tanto, ambos dos mantienen una consistencia entre si y por ningun motivo se debe modificar manualmente, o sea sin comunicarse a través del servidor, alguno de los archivos que se encuentras en esas carpetas ya que ésto provocaría una inconsistencia en el *Servidor* y desencadenaría en un mal funcionamiento del mismo.

Si en algun momento se quiere volver a cero el *Servidor* se puede recompilar tal como se indica en la sección `Compilación y ejecución <forma_de_uso.html>`_ o se pueden borrar tanto la carpeta del *FileSystem* como la carpeta *db* dentro de la carpeta *build* del *Servidor*.

FileSystem
<<<<<<<<<<<<
La carpeta *FileSystem* es la encargada de almacenar tanto los archivos y carpetas de los usuarios de *UDrive* como la foto de perfil de los mismos. Para realizar estas tareas existe una única carpeta dentro del *FileSystem* llamada *fotos* donde se guardarán las fotos de perfil de los usuarios de la siguiente forma ``usuario.extension`` y, para el almacenamiento de los archivos y carpetas, se cuenta con una carpeta en el *FileSystem* para cada uno de los usuarios de la aplicación.

Bases de Datos
<<<<<<<<<<<<<<<<
Para el correcto manejo tanto de usuarios como de archivos y metadatos el *Servidor* de *UDrive* cuenta con 4 bases de datos:

 + Metadatos: Esta BD es la mas compleja ya que no solo almacena los metadatos asociados a cada uno de los archivos que se encuentren alojados en el *Servidor* sino que también maneja todo el control de permisos y versiones sobre los mismos.
 + Sesiones: Esta BD almacena los tokens relacionados a las distintas sesiones que pueden tener los usuarios de *UDrive*.
 + Passwords: Esta BD almecena las contraseñas que utilizan cada uno de los usuarios de *UDrive*.
 + Perfiles: Esta BD almacena los perfiles(nombre, email, cuota, ultima ubicación , etc.) que tiene cada uno de los usuarios de *UDrive*.

Log
<<<<<<<<
Dentro de la carpeta *build* existe una carpeta llamada *files* que es en donde se guarda el *log* del *Servidor* de *UDrive*, dentro de éste se registran todas las activades que realiza el mismo. Estas actividades pueden ser desde requests que le llegan al *Servidor*, como las tareas que realizan los manejadores tanto de usuarios como de archivos y metadatos y las que realizan las diferentes bases de datos.
Si en algún momento de la ejecución del *Servidor* ocurre algún error o falla, se puede mirar el *log* para saber en donde se encuentra el problema.
