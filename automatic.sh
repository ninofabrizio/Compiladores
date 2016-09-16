#! /bin/bash
echo "\n\nINICIANDO TESTES - ANALIZADOR LEXICO\n"

for file in examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

    echo  "\n\n++++++TESTANDO $filename\n"

	echo  "GERANDO ARQUIVO DE SAIDA\n"	
    if ( ./tester < examples/$filename.in > examples/$filename.out ) then
    	echo  "ARQUIVO DE SAIDA GERADO\n"
    
    	echo "COMPARANDO ARQUIVO DE SAIDA COM ARQUIVO GABARITO\n"
    	if(diff examples/$filename.out examples/$filename.gab) then
    		echo "\n\t*** TESTE PASSOU ***\n\n" #retorna não nulo se arquivos forem iguais
    	else
    		echo "\n\t*** TESTE NAO PASSOU ***\n\n"
    	fi
    else
    	echo "PROBLEMA NA MAIN - TESTE $filename ABORTADO\n"
	fi

    echo "------TERMINADO $filename\n"
done

echo "\n\nTESTES ACABADOS - ANALIZADOR LEXICO\n"