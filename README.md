# UDrive
Trabajo Práctico - Taller II

Develop: [![Build Status](https://travis-ci.org/toblich/UDrive.svg?branch=develop)](https://travis-ci.org/toblich/UDrive)

## Instalación, configuración y ejecución
Para ver cómo instalar, configurar y ejecutar el servidor, vaya al Manual del Administrador en la documentación (carpeta `docs/Servidor/Manual_del_Administrador`).

- Para generar los html de toda la documentación, instale [Doxygen](http://www.stack.nl/~dimitri/doxygen/) y [Sphinx](http://sphinx-doc.org/). Con ambas dependencias, ejecute el script `buildDocs.sh` desde la carpeta `docs`. Luego abra `docs/Servidor/Manual_del_Administrador/_build/html/index.html` para verla.

- Para generar la documentación solo del Manual del Administrador, con Sphinx instalado ejecutar `make html` desde `docs/Servidor/Manual_del_Administrador`.

> Para instalar `Sphinx`, con `pip` instalado, ejecutar `pip install sphinx`.
>
> Para instalar `pip` si no lo tuviera, ejecutar `sudo apt-get install python-pip`.
>
> Para instalar `Doxygen` ejecutar `sudo apt-get install doxygen`.

## Repositorio cliente
Por comodidad, el cliente de Android de esta aplicación está en [su propio repositorio](https://github.com/plandino/clienteUdrive).