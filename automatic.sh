#! /bin/bash
#echo "ARVORE SINTATICA:\n"

for file in ast_examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

	#echo "\n++++++PRINTANDO ARVORE DE $filename\n"

    ./tester ast_examples/$filename.in

	#echo "\n*****FIM DA ARVORE SINTATICA*****\n\n\n\n\n"
done

