División de tareas
==================

Dentro de la división de tareas, se plantearon 3 entregas, 2 intermedias y una final, para ir mostrando el avance del trabajo práctico. Éstas son:

 * `Checkpoint 1 - 8/10/2015`_
 * `Checkpoint 2 - 29/10/2015`_
 * `Entrega Final - 19/11/2015`_

Checkpoint 1 - 8/10/2015
^^^^^^^^^^^^^^^^^^^^^^^^
En este checkpoint se buscó comenzar con funcionalidades básicas como tener un servidor con `Mongoose <https://github.com/cesanta/mongoose>`_, un manejo de una Base de Datos con `RocksDB <https://rocksdb.org/>`_, comenzar a definir la API REST, empezar a utilizar Code Coverage (Mediante unitTests, con la herramienta `LCov <http://ltp.sourceforge.net/coverage/lcov.php>`_), tener configurado un servidor de Integración Continua como `Travis-CI <https://travis-ci-org>`_, un parser de archivos Json que se utilizaría por todo el proyecto (Utilizando la biblioteca `jsoncpp <http://jsoncpp.sourceforge.net/old.html>`_), armar un script de compilación que permita fácilmente correr la aplicación con diversas opciones, así como también tener ya configurado un `Docker <https://www.docker.com/>`_ que permita correr la aplicación de manera simple. Además, todo tipo de testing se llevo a cabo mediante la herramienta `GTest <https://code.google.com/p/googletest/>`_ de Google, así como para la compilación se utilizó `CMake <https://cmake.org/>`_. Por último, se tuvo como objetivo empezar con el manejo tanto de Usuarios (Creación y logueo), así como también con el de Archivos y Metadatos.

Repositorio del servidor
~~~~~~~~~~~~~~~~~~~~~~~~
 * `(ABIERTO) Extender tests de integración <https://github.com/toblich/UDrive/issues/31>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Folder: Poder crear una carpeta, obtener la estructura y los archivos de una carpeta y poder eliminarla <https://github.com/toblich/UDrive/issues/30>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `(ABIERTO) Metadata: poder actualizar los metadatos de los archivos <https://github.com/toblich/UDrive/issues/29>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Automatizar tests Servidor <https://github.com/toblich/UDrive/issues/28>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Poder descargar archivos, subir archivos y eliminar archivos del servidor <https://github.com/toblich/UDrive/issues/27>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Crear Dockerfile <https://github.com/toblich/UDrive/issues/26>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Armar script de compilación <https://github.com/toblich/UDrive/issues/25>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Validar perfiles Servidor <https://github.com/toblich/UDrive/issues/24>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Testear parserJson <https://github.com/toblich/UDrive/issues/23>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Discriminar por URL y verbo HTTP <https://github.com/toblich/UDrive/issues/22>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `API REST <https://github.com/toblich/UDrive/issues/21>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Completar lista de caracteres inválidos <https://github.com/toblich/UDrive/issues/20>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Mover base de datos a paquete aparte <https://github.com/toblich/UDrive/issues/19>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Agregar Code Coverage <https://github.com/toblich/UDrive/issues/18>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Parser debe poder (des)serializar metadatos de usuario <https://github.com/toblich/UDrive/issues/17>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Conexión servidor cliente <https://github.com/toblich/UDrive/issues/16>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Testear sin RocksDB <https://github.com/toblich/UDrive/issues/15>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Agregar Batch a BD <https://github.com/toblich/UDrive/issues/14>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Crear alternativa a base de datos <https://github.com/toblich/UDrive/issues/13>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Hacer que el Server se empiece relacionar con la base de datos y el manejador de usuarios <https://github.com/toblich/UDrive/issues/12>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Procesar eventos de mongoose <https://github.com/toblich/UDrive/issues/11>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Crear ManejadorDeArchivosYMetadatos <https://github.com/toblich/UDrive/issues/10>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Pruebas unitarias de parserJson <https://github.com/toblich/UDrive/issues/9>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Activar Travis con UnitTests <https://github.com/toblich/UDrive/issues/8>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `No se pueden crear múltiples Bases de Datos <https://github.com/toblich/UDrive/issues/7>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Implementar Excepciones en BaseDeDatos <https://github.com/toblich/UDrive/issues/6>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Agregar autenticación de usuarios al servidor <https://github.com/toblich/UDrive/issues/5>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Crear cliente Android básico <https://github.com/toblich/UDrive/issues/4>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `JSON: Crear (des)serializador <https://github.com/toblich/UDrive/issues/3>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Crear un manejador básico de RocksDB <https://github.com/toblich/UDrive/issues/2>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Crear server inicial <https://github.com/toblich/UDrive/issues/1>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_

Repositorio del cliente
~~~~~~~~~~~~~~~~~~~~~~~
 * `(ABIERTO) Embellecer la aplicación <https://github.com/plandino/clienteUdrive/issues/2>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `(ABIERTO) Terminar la subida de archivos desde el cliente al servidor <https://github.com/plandino/clienteUdrive/issues/1>`_ Realizado por `Francisco <https://github.com/plandino>`_


Checkpoint 2 - 29/10/2015
^^^^^^^^^^^^^^^^^^^^^^^^^
En este checkpoint se buscó seguir con las funcionalidades empezadas en el primer checkpoint, además de agregar nuevas funcionalidades. Entre ellas se destacan el hecho de poder dar permisos y los distintos tipos de búsquedas, además de poder eliminar, renombrar y restaurar archivos. Por parte del cliente, se implementó la utilización del GPS, además de que se acompañó el desarrollo hecho en el servidor para el compartido, renombrado, restaurado, la eliminación, las búsquedas de archivos. Por último, se comenzó la documentación del código del servidor con `Doxygen <http://www.stack.nl/~dimitri/doxygen/>`_, además de la documentación de la API REST y la compilación e instalación de dependencias.

Repositorio del servidor
~~~~~~~~~~~~~~~~~~~~~~~~
 * `(ABIERTO) Comenzar con la documentación del código en Doxygen <https://github.com/toblich/UDrive/issues/49>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Armar Changelog 2do checkpoint <https://github.com/toblich/UDrive/issues/48>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Buscar usuarios que contengan string en su username <https://github.com/toblich/UDrive/issues/47>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Actualizar la ultima ubicacion del usuario cuando se sube un archivo <https://github.com/toblich/UDrive/issues/46>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Agregar Multithreading en el servidor <https://github.com/toblich/UDrive/issues/45>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Búsquedas en ManejadorArchivosYMetadatos <https://github.com/toblich/UDrive/issues/42>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `TODO Papelera <https://github.com/toblich/UDrive/issues/41>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Hacer que el usuario pueda actualizar su foto de perfil <https://github.com/toblich/UDrive/issues/40>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Continuar funcionalidades de ManjeadorArchivosYMetadatos (ver detalle...) <https://github.com/toblich/UDrive/issues/39>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Hacer el manual de la API REST <https://github.com/toblich/UDrive/issues/38>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `(ABIERTO) Agregar Coverage a Travis <https://github.com/toblich/UDrive/issues/37>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Búsquedas en Metadata <https://github.com/toblich/UDrive/issues/36>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Agregar tests de integración a Travis <https://github.com/toblich/UDrive/issues/35>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Verificar que el perfil actualizado siga siendo valido <https://github.com/toblich/UDrive/issues/34>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Refactor de los distintos realizadores de eventos <https://github.com/toblich/UDrive/issues/33>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Agregar la parte de permisos al ManejadorArchivosYMetadatos <https://github.com/toblich/UDrive/issues/32>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Extender tests de integración <https://github.com/toblich/UDrive/issues/31>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Metadata: poder actualizar los metadatos de los archivos <https://github.com/toblich/UDrive/issues/29>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_

Repositorio del cliente
~~~~~~~~~~~~~~~~~~~~~~~
 * `(ABIERTO) Poder buscar archivos por los distintos metadatos <https://github.com/plandino/clienteUdrive/issues/11>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Hacer el acceso a la carpeta de archivos compartidos <https://github.com/plandino/clienteUdrive/issues/10>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Hacer que se puedan eliminar y restaurar archivos desde la papelera <https://github.com/plandino/clienteUdrive/issues/9>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `(ABIERTO) Agregar usabilidad <https://github.com/plandino/clienteUdrive/issues/8>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Mostrar foto de perfil en la activity de perfil <https://github.com/plandino/clienteUdrive/issues/7>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Usar el GPS del telefono para obtener la ubicacion <https://github.com/plandino/clienteUdrive/issues/6>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Crear un comportamiento correcto al moverse dentro de las carpetas y archivos <https://github.com/plandino/clienteUdrive/issues/5>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Poder crear carpetas en el servidor desde el cliente <https://github.com/plandino/clienteUdrive/issues/4>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Poder visualizar y modificar los metadatos de los archivos <https://github.com/plandino/clienteUdrive/issues/3>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `(ABIERTO) Embellecer la aplicación <https://github.com/plandino/clienteUdrive/issues/2>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Terminar la subida de archivos desde el cliente al servidor <https://github.com/plandino/clienteUdrive/issues/1>`_ Realizado por `Francisco <https://github.com/plandino>`_


Entrega Final - 19/11/2015
^^^^^^^^^^^^^^^^^^^^^^^^^^
Las funcionalidades entregadas en esta entrega ya contemplan todas las pedidas en el enunciado. Propiamente dicho, se hizo el versionado de archivos, además de un gran refactor de clases que habían quedado muy grandes. Por otro lado, se terminó de hacer todo lo requerido respecto de documentación (Tanto del servidor como del cliente), así como también se terminaron algunos detalles de funcionalidad que habían quedado a medio hacer.

Repositorio del servidor
~~~~~~~~~~~~~~~~~~~~~~~~
 * `Tests de stress <https://github.com/toblich/UDrive/issues/59>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Actualización de una versión vieja <https://github.com/toblich/UDrive/issues/58>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Cambiar espacios por ~ en los archivos bug <https://github.com/toblich/UDrive/issues/57>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Hacer Documentación Técnica <https://github.com/toblich/UDrive/issues/56>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Hacer Documentación de Administrador <https://github.com/toblich/UDrive/issues/55>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Hacer Documentación de Proyecto <https://github.com/toblich/UDrive/issues/54>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Hacer Manual de Usuario del Cliente <https://github.com/toblich/UDrive/issues/53>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Revisar TODO's del código enhancement Servidor <https://github.com/toblich/UDrive/issues/52>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_
 * `Refactor: extraer paquetes <https://github.com/toblich/UDrive/issues/51>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Refactor del Server y de las clases de la API REST <https://github.com/toblich/UDrive/issues/50>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Documentar el código con Doxygen <https://github.com/toblich/UDrive/issues/49>`_ Realizado por `Santiago <https://github.com/sfernandez11>`_
 * `Agregar Control de versiones <https://github.com/toblich/UDrive/issues/44>`_ Realizado por `Tobías <https://github.com/toblich>`_
 * `Partir ManejadorArchivosYMetadatos en clases más pequeñas <https://github.com/toblich/UDrive/issues/43>`_ Realizado por `Pablo <https://github.com/pciruzzi>`_

Repositorio del cliente
~~~~~~~~~~~~~~~~~~~~~~~
 * `Generar documentacion <https://github.com/plandino/clienteUdrive/issues/15>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Versionado de archivos <https://github.com/plandino/clienteUdrive/issues/14>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Hacer que la sesion persista si no cierro sesion <https://github.com/plandino/clienteUdrive/issues/13>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Ordenar codigo y repositorio. <https://github.com/plandino/clienteUdrive/issues/12>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Poder buscar archivos por los distintos metadatos <https://github.com/plandino/clienteUdrive/issues/11>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Agregar usabilidad <https://github.com/plandino/clienteUdrive/issues/8>`_ Realizado por `Francisco <https://github.com/plandino>`_
 * `Embellecer la aplicación <https://github.com/plandino/clienteUdrive/issues/2>`_ Realizado por `Francisco <https://github.com/plandino>`_
