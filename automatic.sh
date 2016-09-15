#! /bin/bash
#make
echo "INICIANDO TESTES - ANALIZADOR LEXICO\n\n"
for file in examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

    echo "++++++TESTANDO $filename"

    ./tester < examples/$filename.in > examples/$filename.out
    
    echo "------TERMINADO $filename"

done
#make clean