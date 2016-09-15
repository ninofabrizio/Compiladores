#! /bin/bash
echo
echo
echo "INICIANDO TESTES - ANALIZADOR LEXICO"
echo
for file in examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

    echo
    echo "++++++TESTANDO $filename"
    echo

    ./tester < examples/$filename.in > examples/$filename.out
    
    diff -q examples/$filename.out examples/$filename.gab

    echo "------TERMINADO $filename"
    echo

done