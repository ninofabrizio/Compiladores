#! /bin/bash
#echo "LLVM:\n"

for file in llvm_examples/*.monga
do
    filename=$(basename "$file")
    filename="${filename%.*}"

	#echo "\n++++++MONTANDO LLVM DE $filename\n"

    ./tester llvm_examples/$filename.monga

	#echo "\n*****$filename.ll GERADO*****\n\n\n\n\n"
done