#!/bin/bash

directories=("Manual_de_Proyecto/" "Servidor/*/" "Cliente/*/")

for dir in ${directories[@]}
do
    make -C $dir html
done


doxygen Doxyfile
