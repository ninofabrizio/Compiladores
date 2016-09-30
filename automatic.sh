#! /bin/bash
echo "\n\nINICIANDO TESTES - ANALIZADOR LEXICO\n"

for file in syntax_examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

    echo "\n\n++++++TESTANDO $filename\n"
	
    if ( ./tester < syntax_examples/$filename.in ) then
    	echo "ARQUIVO PASSOU\n"	
	else
		echo "ARQUIVO NAO PASSOU\n"
	fi

    echo "------TERMINADO $filename\n"
done

echo  "\n\nTESTES ACABADOS - ANALIZADOR LEXICO\n"