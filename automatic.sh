#! /bin/bash
echo "Arvore.."

for file in syntax_examples/*.in
do
    filename=$(basename "$file")
    filename="${filename%.*}"

    ./tester syntax_examples/$filename.in

done

