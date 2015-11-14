Introducción
============

El proyecto **UDrive** consistía en hacer una aplicación de almacenamiento de archivos remota, al estilo `Dropbox <https://dropbox.com>`_ o `Google Drive <http://drive.google.com>`_. Para ello se deberían llevar a cabo dos componentes:

* **Servidor:** hecho en C++. Éste debía ser el responsable del procesamiento y almacenamiento de los archivos, así como también de diversos metadatos que hacen al mismo (Como última fecha de modificación o etiquetas) mediante una Base de Datos. Dentro del mismo debía llevarse a cabo la lógica del compartido de archivos y el manejo de versiones de los mismos, así como también permitir enviar a la papelera, restaurar, renombrar y eliminar definitivamente. En su gran mayoría esta parte del trabajo práctico fue llevada a cabo por Pablo Ciruzzi, Santiago Fernández y Tobías Lichtig.
* **Cliente:** es el responsable de la visualización de los archivos y carpetas del usuario, y todo tipo de interacción con el mismo, para que haga fácil el uso de las funcionalidades implementadas en la aplicación Servidor. Esta aplicación debía ser una aplicación móvil y por lo tanto fue realizada en Java/Android. A pesar de haber colaborado con el Servidor en un comienzo, ésta fue desarrollada íntegramente por Francisco Landino.
