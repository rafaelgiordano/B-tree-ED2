/* ================================================================== *

	Universidade Federal de Sao Carlos - UFSCar, Sorocaba

	Disciplina: Estruturas de Dados 2
	Prof. Tiago A. Almeida

	Trabalho 2 - Indexacao usando arvore-B

    Autor: Giulianno Raphael Sbrugnera RA: 408093
    Autor: Rafael Paschoal Giordano    RA: 408298

	Especificacao
	-------------

	Trabalho que consiste em criar e manipular indices em RAM, 
	criados a partir de um arquivo de dados guardado em disco.
	Utilizacao de conceitos como indexacao e procura de registros
	pelo RRN, assim como TADs definidas por nos.
	Indices tambem sao guardados em disco nos momentos oportunos.
	
	Definicao
	----------

	Esse sistema compreende o cadastro de series de TV. Abaixo segue
	as funcionalidades implementadas.
	- insercao, remocao e alteracao de series, onde
	  - remocao nao remove imediatamente os registros do arquivo
	  de dados.
	  - alteracao diz respeito a alteracao da nota de uma serie.
	- buscas de series, onde ha 3 tipos de buscas
	  - pela chave primaria.
	  - pelo titulo em portugues das series.
	  - pelo sobrenome-nome do diretor.
	- listagem ordenada das series, onde ha 3 tipos de listagens
	  - pela chave primaria.
	  - pelo titulo em portugues das series.
	  - pelo sobrenome-nome do diretor.
	- liberacao de espaco em disco, removendo os registros marcados
	como removidos atraves da remocao de series.

	Arquivo
	-------

	Contem as definicoes de todas as funcoes, bem como as definicoes
	dos tipos abstratos de dados utilizadas para os indices	e a 
	serie.

 * ================================================================== */

#ifndef __408093_408298_ED2_T01_h__
#define __408093_408298_ED2_T01_h__

// Inclusao das bibliotecas do sistema
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Definicao das constantes utilizadas ao longo do programa
#define TRUE  			1 		// utilizado em retorno de funcoes
#define FALSE 			0 		// utilizado em retorno de funcoes
#define BUSCA 			2 		// opcao de busca por uma chave
#define PROCURA_IGUAL   3 		// opcao de insercao de serie, busca por uma chave primaria ja existente
#define BUSCA_RRN 		4 		// faz com que a funcao buscaChavePrimaria retorne o RRN
#define INICIANDO 		10 		// indica que o sistema esta inicializando, devendo colocar flag 0 nos arquivos de indices
#define FINALIZANDO 	11 		// indica que o sistema esta finalizando, devendo colocar flag 1 nos arquivos de indices
#define MAX_COD 		9 		// tamanho maximo da string que armazena a chave primaria
#define MAX_STRING	 	981 	// tamanho maximo dos campos de tamanho variavel
#define MAX_REPET 		1000	// tamanho maximo associado a uma chave secundaria repetida

// Definicao das frases de erro que podem ocorrer ao longo da execucao do programa
#define FRASE_ERROALOCACAO	    		"Erro de alocacao!\n"
#define FRASE_ERROREALOCACAO			"Erro de realocacao!\n"
#define FRASE_ARQUIVO_VAZIO				"Arquivo vazio!\n"
#define FRASE_OPCAO_INEXISTENTE			"Opcao inexistente!\n"
#define FRASE_REGISTRO_NAOENCONTRADO	"Registro nao encontrado!\n"
#define FRASE_REGISTRO_JACADASTRADO		"\nRegistro ja cadastrado!\n"

// Definicao da estrutura da serie
typedef struct serie
{
	char codigo[MAX_COD];
	char tituloPortugues[MAX_STRING]; 
	char tituloOriginal[MAX_STRING];  
	char diretor[MAX_STRING];		 
	char genero[MAX_STRING];		  	 
	char duracao[4];
	char anoEstreia[5];
	char anoEncerramento[5];
	char numeroTemporadas[3];
	char numeroEpisodios[5];
	char pais[MAX_STRING];			 
	int nota;
	char resumo[MAX_STRING];		 
}Serie;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Definicao da estrutura do indice primario - PRECISA SER RETIRADO
typedef struct primary_index
{
	int RRN;	// posicao em bytes da serie que possui o codigo primario primaryCod
	char primaryCod[MAX_COD];
}PRIMARY_INDEX;
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Definicao do no
typedef struct node
{
	int numKeys;			// numero de chaves armazenadas
	int *RRN;				// RRN de cada chave
	char **codigo;			// vetor de chaves
	struct node **desc;		// ponteiros para os descendentes
	int leaf;				// flag folha da arvore
}Node;

// Definicao da arvore
typedef Node Btree;

// Definicao da estrutura do indice secundario por titulo em portugues
typedef struct ititle
{
	int contRepetido;
	char tituloPortugues[MAX_STRING];
	char primaryCod[MAX_REPET][MAX_COD];
}ITITLE;

// Definicao da estrutura do indice secundario por nome do diretor
typedef struct idirector
{
	int contRepetido;
	char diretor[MAX_STRING];
	char primaryCod[MAX_REPET][MAX_COD];
}IDIRECTOR;


/* ================================================================== */
/* ===================== PREAMBULO DAS FUNCOES ====================== */
/* ================================================================== */


/* Inicializa os contadores de cada indice */
void inicializarIndices(int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex, int max_size);

/* Remocao fisica de todos os registros marcados como excluidos e atualizacao dos indices */
void liberarEspaco(FILE *f, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex, int max_size);

/* Realiza o truncamento da serie para que o registro todo, mais os delimitadores, ocupe 1024 bytes no arquivo de dados */
Serie truncateSerie(Serie serie, int *tamanho);


/* ============================================================== */
/* ============= Rotinas de manipulacao da arvore-B ============= */
/* ============================================================== */

/* -------------------- Rotinas de Insercao --------------------- */

/* Inicializa a arvore-B */
Btree *inicializaBtree(Btree *t);

/* Insere uma chave primaria na arvore-B */
Btree *insereChave(Btree *t, char codigo[MAX_COD], int rrn);

/* Insere uma chave primaria em um no nao cheio da arvore-B */
void insereNoNaoCheio(Node *x, char codigo[MAX_COD], int rrn);

/* Realiza o split de um no da arvore-B */
void divideNo(Node *pai, int i, Node *filho);

/* Cria um novo no e retorna a posicao dele */
Btree *criaNo();

/* ---------------------- Rotinas de Busca ---------------------- */

/* Busca por uma chave na arvore-B */
Node *buscaChave(Node *x, char chave[MAX_COD], int choice);

/* Realiza a busca binaria em um no da arvore-B */
int buscaBinariaNo(char **vetCodigo, char chaveBusca[MAX_COD], int RRN[], int esquerda, int direita, int choice);

/* --------------------- Rotinas de Remocao --------------------- */

/* Remove uma chave primaria da arvore-B */
int removeChave(Btree *t, char codigo[MAX_COD]);

/* Realiza o balanceamento de um no-folha */
void balanceiaFolha(Btree *t, Node *f);

/* Realiza a redistribuicao de chaves do no a esquerda do no */
void balanceiaEsqDir(Node *p, int e, int esq, int dir);

/* Realiza a concatenacao de chaves, resultando em juncao de nos e diminuicao da altura da arvore */
void diminuiAltura(Node *x, Btree *t);

/* Realiza a juncao de dois nos */
void juncaoNo(Node *x, int i);

/* ============================================================== */
/* ===== Rotinas que implementam operacoes com os registros ===== */
/* ============================================================== */

/* Insere uma nova serie no arquivo de dados. Retorna (1) se foi inserido com sucesso ou (0) caso
   a chave primaria gerada ja exista.																*/
Btree *inserirSerie(FILE *f, PRIMARY_INDEX *primary_index, Btree *t, int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex);

/* Atualiza a nota de uma determinada serie */
int alterarSerie(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Remove uma serie, atribuindo |* ao comeco do registro no arquivo de dados */
int removerSerie(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);


/* ============================================================== */
/* == Rotinas que verificam se os arquivos necessarios existem == */
/* ============================================================== */

/* Retorna (1) se o arquivo de dados estiver vazio, e (0) caso contrario */
int arquivoDadosVazio();


/* ============================================================== */
/* ============= Rotinas que criam os indices em RAM ============ */
/* ============================================================== */

/* Recupera todos os dados do arquivo de dados */
void recuperarDados(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex);

/* Cria um indice primario em RAM a partir do arquivo de dados */
void criarIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, char codigo[MAX_COD], int rrn);

/* Cria o indice secundario por titulo em RAM a partir do arquivo de dados */
void criarIndiceSecundarioTitulo(ITITLE *ititle, int *qtdeTitleIndex, char titulo[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);

/* Cria o indice secundario por diretor em RAM a partir do arquivo de dados */
void criarIndiceSecundarioDiretor(IDIRECTOR *idirector, int *qtdeDirectorIndex, char diretor[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);	


/* ============================================================== */
/* ================= Rotinas de busca por chave ================= */
/* ============================================================== */

/* Realiza busca binaria com base na chave primaria */
int buscarChavePrimaria(PRIMARY_INDEX *primary_index, char chaveBusca[MAX_COD], int esquerda, int direita, int choice);

/* Realiza busca binaria com base no titulo em portugues */
int buscarTituloPortugues(ITITLE *ititle, char tituloBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice);

/* Realiza busca binaria com base no titulo em portugues */
int buscarDiretor(IDIRECTOR *idirector, char diretorBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice);


/* ============================================================== */
/* =============== Rotinas de ordenacao de indices ============== */
/* ============================================================== */

/* Ordena em ordem crescente lexograficamente o indice de chaves primarias */
void ordenarIndicePrimario(PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Ordena em ordem crescente lexograficamente o indice de chaves secundarias por titulo */
void ordenarIndiceSecundarioTitulo(ITITLE *ititle, int qtdeTitleIndex);

/* Ordena em ordem crescente lexograficamente o indice de chaves secundarias por diretor */
void ordenarIndiceSecundarioDiretor(IDIRECTOR *idirector, int qtdeDirectorIndex);


/* ============================================================== */
/* =============== Rotinas de listagem das series =============== */
/* ============================================================== */

/* Lista as series em ordem crescente lexograficamente dos codigos */
int listarSeriesCodigo(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Lista as series em ordem crescente lexograficamente dos titulos em portugues */
int listarSeriesTitulo(FILE *f, ITITLE *ititle, int qtdeTitleIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Lista as series em ordem crescente lexograficamente dos diretores */
int listarSeriesDiretor(FILE *f, IDIRECTOR *idirector, int qtdeDirectorIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

#endif /* __408093_408298_ED2_T01_h__ */