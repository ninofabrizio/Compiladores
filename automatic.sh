#! /bin/bash
echo -e "\n\nINICIANDO TESTES - ANALIZADOR LEXICO\n"

for file in examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

    echo -e "\n\n++++++TESTANDO $filename\n"

	echo -e "GERANDO ARQUIVO DE SAIDA\n"	
    if ( ./tester < examples/$filename.in > examples/$filename.out ) then
    	echo -e "ARQUIVO DE SAIDA GERADO\n"
    
    	echo -e "COMPARANDO ARQUIVO DE SAIDA COM ARQUIVO GABARITO\n"
    	if(diff examples/$filename.out examples/$filename.gab &>/dev/null) then
    		echo -e "\n\t*** TESTE PASSOU ***\n\n" #retorna não nulo se arquivos forem iguais
    	else
    		echo -e "\n\t*** TESTE NAO PASSOU ***\n\n"
    	fi
    else
    	echo -e "PROBLEMA NA MAIN - TESTE $filename ABORTADO\n"
	fi

    echo -e "------TERMINADO $filename\n"
done

echo -e "\n\nTESTES ACABADOS - ANALIZADOR LEXICO\n"