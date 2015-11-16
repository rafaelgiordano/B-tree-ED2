/* ================================================================== *

	Universidade Federal de Sao Carlos - UFSCar, Sorocaba

	Disciplina: Estruturas de Dados 2
	Prof. Tiago A. Almeida

	Trabalho 1 - Indexacao

    Autor....: Giulianno Raphael Sbrugnera RA: 408093
    Autor....: Rafael Paschoal Giordano    RA: 408298

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

// Definicao da estrutura do indice primario
typedef struct primary_index
{
	int RRN;	// posicao em bytes da serie que possui o codigo primario primaryCod
	char primaryCod[MAX_COD];
}PRIMARY_INDEX;

// Definicao da estrutura do indice secundario por titulo em portugues
typedef struct title_index
{
	int contRepetido;
	char tituloPortugues[MAX_STRING];
	char primaryCod[MAX_REPET][MAX_COD];
}TITLE_INDEX;

// Definicao da estrutura do indice secundario por nome do diretor
typedef struct director_index
{
	int contRepetido;
	char diretor[MAX_STRING];
	char primaryCod[MAX_REPET][MAX_COD];
}DIRECTOR_INDEX;


/* ================================================================== */
/* ===================== PREAMBULO DAS FUNCOES ====================== */
/* ================================================================== */


/* Inicializa os contadores de cada indice */
void inicializarIndices(int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, int max_size);

/* Retorna 0 se o indice estiver inconsistente. Retorna 1 caso contrario */
int verificaConsistenciaIndice(FILE *f);

/* Remocao fisica de todos os registros marcados como excluidos e atualizacao dos indices */
void liberarEspaco(FILE *f, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, int max_size);

/* Realiza o truncamento da serie para que o registro todo, mais os delimitadores, ocupe 1024 bytes no arquivo de dados */
Serie truncateSerie(Serie serie, int *tamanho);

/* ============================================================== */
/* ===== Rotinas que implementam operacoes com os registros ===== */
/* ============================================================== */

/* Insere uma nova serie no arquivo de dados. Retorna (1) se foi inserido com sucesso ou (0) caso
   a chave primaria gerada ja exista.																*/
int inserirSerie(FILE *f, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex);

/* Atualiza a nota de uma determinada serie */
int alterarSerie(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Remove uma serie, atribuindo |* ao comeco do registro no arquivo de dados */
int removerSerie(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);


/* ============================================================== */
/* == Rotinas que verificam se os arquivos necessarios existem == */
/* ============================================================== */

/* Retorna (1) se o arquivo de dados estiver vazio, e (0) caso contrario */
int arquivoDadosVazio();

/* Retorna (1) se o arquivo do indice primario estiver vazio, e (0) caso contrario */
int indicePrimarioVazio();

/* Retorna (1) se o arquivo do indice secundario por titulo estiver vazio, e (0) caso contrario */
int indiceSecundarioTituloVazio();

/* Retorna (1) se o arquivo do indice secundario por diretor estiver vazio, e (0) caso contrario */
int indiceSecundarioDiretorVazio();


/* ============================================================== */
/* ============= Rotinas que criam os indices em RAM ============ */
/* ============================================================== */

/* Recupera todos os dados do arquivo de dados */
void recuperarDados(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex);

/* Refaz o indice primario a partir do arquivo 'iprimary.idx' */
void refazerIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);

/* Refaz o indice secundario por titulo a partir do arquivo 'ititle.idx' */
void refazerIndiceSecundarioTitulo(TITLE_INDEX *title_index, int *qtdeTitleIndex, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);

/* Refaz o indice secundario por diretor a partir do arquivo 'idirector.idx' */
void refazerIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);

/* Cria um indice primario em RAM a partir do arquivo de dados */
void criarIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, char codigo[MAX_COD], int rrn);

/* Cria o indice secundario por titulo em RAM a partir do arquivo de dados */
void criarIndiceSecundarioTitulo(TITLE_INDEX *title_index, int *qtdeTitleIndex, char titulo[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);

/* Cria o indice secundario por diretor em RAM a partir do arquivo de dados */
void criarIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, char diretor[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex);	


/* ============================================================== */
/* ========== Rotinas que gravam os indices em arquivos ========= */
/* ============================================================== */

/* Grava o indice primario no arquivo 'imprimary.idx' */
void gravarIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, int estadoSistema);

/* Grava o indice secundario por titulo no arquivo 'ititle.idx' */
void gravarIndiceSecundarioTitulo(TITLE_INDEX *title_index, int *qtdeTitleIndex, int estadoSistema);

/* Grava o indice secundario por diretor no arquivo 'idirector.idx' */
void gravarIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, int estadoSistema);


/* ============================================================== */
/* ================= Rotinas de busca por chave ================= */
/* ============================================================== */

/* Realiza busca binaria com base na chave primaria */
int buscarChavePrimaria(PRIMARY_INDEX *primary_index, char chaveBusca[MAX_COD], int esquerda, int direita, int choice);

/* Realiza busca binaria com base no titulo em portugues */
int buscarTituloPortugues(TITLE_INDEX *title_index, char tituloBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice);

/* Realiza busca binaria com base no titulo em portugues */
int buscarDiretor(DIRECTOR_INDEX *director_index, char diretorBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice);


/* ============================================================== */
/* =============== Rotinas de ordenacao de indices ============== */
/* ============================================================== */

/* Ordena em ordem crescente lexograficamente o indice de chaves primarias */
void ordenarIndicePrimario(PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Ordena em ordem crescente lexograficamente o indice de chaves secundarias por titulo */
void ordenarIndiceSecundarioTitulo(TITLE_INDEX *title_index, int qtdeTitleIndex);

/* Ordena em ordem crescente lexograficamente o indice de chaves secundarias por diretor */
void ordenarIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int qtdeDirectorIndex);


/* ============================================================== */
/* =============== Rotinas de listagem das series =============== */
/* ============================================================== */

/* Lista as series em ordem crescente lexograficamente dos codigos */
int listarSeriesCodigo(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Lista as series em ordem crescente lexograficamente dos titulos em portugues */
int listarSeriesTitulo(FILE *f, TITLE_INDEX *title_index, int qtdeTitleIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

/* Lista as series em ordem crescente lexograficamente dos diretores */
int listarSeriesDiretor(FILE *f, DIRECTOR_INDEX *director_index, int qtdeDirectorIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex);

#endif /* __408093_408298_ED2_T01_h__ */