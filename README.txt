EXECUÇAO:

Para executar, posicione o terminal no diretório do projeto e digite: "make run".

FUNCIONALIDADE:

O arquivo Makefile gera em ordem os arquivos "monga.c", "monga.o", "main.o" e o executável "tester". Depois de produzi-los, é executada a shell "automatic.sh". A shell utiliza os arquivos.in da pasta "\examples" e os executa em loop individualmente, gerando as saídas de teste arquivos.out que, por sua vez, são comparados com os arquivos.gab. Vale destacar que o conjunto de arquivos para cada teste (files.in, files.gab e files.out) precisam ter exatamente o mesmo nome.

PARA CRIAR TESTES:

Basta criar arquivos.in e arquivos.gab com nomes identicos na pasta "\examples".
