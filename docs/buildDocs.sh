#!/bin/bash

directories=("Manual_de_Proyecto/" "Servidor/*/")

for dir in ${directories[@]}
do
    make -C $dir html
done


doxygen Doxyfile
