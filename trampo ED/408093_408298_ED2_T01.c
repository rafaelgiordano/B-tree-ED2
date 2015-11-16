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

	Contem a biblioteca de manipulacao de arquivos, onde todas as 
	funcionalidades do sistema sao implementadas.

 * ================================================================== */

// Inclusao do header
#include "408093_408298_ED2_T01.h"

/* ================================================================== */
/* ========== FUNCOES QUE DETERMINAM AS OPERACOES POSSIVEIS ========= */
/* ================================================================== */


/* Inicializa os contadores de cada indice e a quantidade de registros de cada indice */
void inicializarIndices(int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, int max_size)
{
	int i;

	// Inicializa com 0 o contador de chaves secundarias repetidas de cada posicao dos indices secundarios
	for (i = 0; i < max_size; i++)
	{
		title_index[i].contRepetido = 0;
		director_index[i].contRepetido = 0;
	}

	// Inicializa a quantidade de cada indice
	*qtdePrimaryIndex = 0;
	*qtdeTitleIndex = 0;
	*qtdeDirectorIndex = 0;
}

/* Retorna 0 se o indice estiver inconsistente. Retorna 1 caso contrario */
int verificaConsistenciaIndice(FILE *f)
{
	return (fgetc(f) == '1');	
}

/* Remocao fisica de todos os registros marcados como excluidos e atualizacao dos indices */
void liberarEspaco(FILE *f, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, int max_size)
{
	FILE *temp;

	Serie serie;

	int rrn;

	char *ptrString;
	char recoverData[1024];

	fclose(f);
	f = fopen("series.dat", "r");

	temp = fopen("temp.dat", "w");

	// Indices sao inicializados, pois serao construidos novamente
	inicializarIndices(qtdePrimaryIndex, title_index, qtdeTitleIndex, director_index, qtdeDirectorIndex, max_size);

	rrn = 0;

	// Condicao de parada eh chegar ao final do arquivo de dados
	while (fgets(recoverData, 1025, f))
	{
		if ((recoverData[0] != '*') && (recoverData[1] != '|'))
		{
			// Guarda o registro em um arquivo temporario. Somente aqueles que nao foram marcados para remocao
			fprintf(temp, "%s", recoverData);

			// Desmembramento do registro em seus campos, ignorando os delimitadores

			ptrString = strtok(recoverData, "@");
			strcpy(serie.codigo, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.tituloPortugues, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.tituloOriginal, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.diretor, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.genero, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.duracao, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.anoEstreia, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.anoEncerramento, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.numeroTemporadas, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.numeroEpisodios, ptrString);

			ptrString = strtok(NULL, "@");
			strcpy(serie.pais, ptrString);

			ptrString = strtok(NULL, "@"); 
			serie.nota = *ptrString - '0';
			
			ptrString = strtok(NULL, "@");
			strcpy(serie.resumo, ptrString);


			// Recriando os indices. Necessario ordena-los a todo instante

			criarIndicePrimario(primary_index, qtdePrimaryIndex, serie.codigo, rrn);
			criarIndiceSecundarioTitulo(title_index, qtdeTitleIndex, serie.tituloPortugues, serie.codigo, primary_index, qtdePrimaryIndex);
			criarIndiceSecundarioDiretor(director_index, qtdeDirectorIndex, serie.diretor, serie.codigo, primary_index, qtdePrimaryIndex);

			ordenarIndicePrimario(primary_index, *qtdePrimaryIndex);
			ordenarIndiceSecundarioTitulo(title_index, *qtdeTitleIndex);
			ordenarIndiceSecundarioDiretor(director_index, *qtdeDirectorIndex);

			rrn = rrn + 1024;
		}
	}

	fclose(f);
	f = fopen("series.dat", "w");

	fclose(temp);
	temp = fopen("temp.dat", "r");

	// Condicao de parada eh chegar ao final do arquivo temporario
	while (fgets(recoverData, 1025, temp))
	{
		// Copia os registros do temporario para o arquivo de dados
		fprintf(f, "%s", recoverData);
	}

	fclose(temp);
	remove("temp.dat");

	fclose(f);
	f = fopen("series.dat", "a+");
}

/* Realiza o truncamento da serie para que o registro todo, mais os delimitadores, ocupe 1024 bytes no arquivo de dados */
Serie truncateSerie(Serie serie, int *tamanho)
{
	int i;
	int vetTam[6];

	// Corta o que possa existir de excedente nos campos
	serie.resumo[MAX_STRING - 1] = '\0';
	serie.tituloPortugues[MAX_STRING - 1] = '\0';
	serie.tituloOriginal[MAX_STRING - 1] = '\0';
	serie.genero[MAX_STRING - 1] = '\0';
	serie.pais[MAX_STRING - 1] = '\0';
	serie.diretor[MAX_STRING - 1] = '\0';

	// Definicao de um contador que comeca no final
	i = MAX_STRING - 1;

	// printf ("antes do while\n\n");

	// Condicao de parada eh a soma do tamanho dos campos de tamanho fixo estar menor ou igual que o permitido
	do
	{
		// Cada posicao do vetor guarda o tamanho de um campo
		vetTam[0] = strlen(serie.resumo);
		vetTam[1] = strlen(serie.tituloPortugues);
		vetTam[2] = strlen(serie.tituloOriginal);
		vetTam[3] = strlen(serie.genero);
		vetTam[4] = strlen(serie.pais);
		vetTam[5] = strlen(serie.diretor);
		
		// Na posicao i os campos sao diminuidos. Nao interfere se o campo nao tiver nada na posicao
		serie.resumo[i] = '\0';
		serie.tituloPortugues[i] = '\0';
		serie.tituloOriginal[i] = '\0';
		serie.genero[i] = '\0';
		serie.pais[i] = '\0';
		serie.diretor[i] = '\0';
	
		i--;

	}while ((vetTam[0] + vetTam[1] + vetTam[2] + vetTam[3] + vetTam[4] + vetTam[5]) > MAX_STRING + 4);

	// Dessa maneira, somente os campos que realmente excedem o limite estipulado sao truncados

	*tamanho = 1024;

	return serie;
}

/* ============================================================== */
/* ===== Rotinas que implementam operacoes com os registros ===== */
/* ============================================================== */

/* Insere uma nova serie no arquivo de dados. Retorna (1) se foi inserido com sucesso ou (0) caso
   a chave primaria gerada ja exista.																*/
int inserirSerie(FILE *f, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex)
{
	int i;
	int rrn;
	int tamanho;

	Serie serie;

	// Leitura dos dados das series
	// Foi utilizado expressao regular para tratar o enter do usuario

	printf ("\n\nCadastro de nova serie inicializado\n");
	printf ("Titulo em portugues: ");
	scanf("\n%[^\n]", serie.tituloPortugues);
	printf ("Titulo original: ");
	getchar();
	if (scanf("%[^\n]s", serie.tituloOriginal) < 1)
	{
		// Titulo original nao foi informado

		strcpy(serie.tituloOriginal, "Idem");
	}
	printf ("Diretor: ");
	scanf("\n%[^\n]", serie.diretor);
	printf ("Genero: ");
	scanf("\n%[^\n]", serie.genero);
	printf ("Duracao em minutos: ");
	scanf("\n%[^\n]", serie.duracao);
	printf ("Ano de estreia: ");
	scanf("\n%[^\n]", serie.anoEstreia);
	printf ("Ano de encerramento: ");
	scanf("\n%[^\n]", serie.anoEncerramento);
	printf ("Numero de temporadas: ");
	scanf("\n%[^\n]", serie.numeroTemporadas);
	printf ("Numero de episodios: ");
	scanf("\n%[^\n]", serie.numeroEpisodios);
	printf ("Pais: ");
	scanf("\n%[^\n]", serie.pais);
	printf ("Nota: ");
	scanf ("%d", &serie.nota);
	printf ("Resumo: ");
	scanf("\n%[^\n]", serie.resumo);

	tamanho = strlen(serie.tituloPortugues);
	tamanho = tamanho + strlen(serie.tituloOriginal);
	tamanho = tamanho + strlen(serie.diretor);
	tamanho = tamanho + strlen(serie.genero);
	tamanho = tamanho + strlen(serie.pais);
	tamanho = tamanho + strlen(serie.resumo);
	tamanho = tamanho + 39; 

   	if (tamanho > 1024)
   	{
   		// Necessario realizar o truncamento das informacoes da serie

   		serie = truncateSerie(serie, &tamanho);
   	}
	
   	// Geracao da chave primaria da serie

	for (i = 0; i < 3; i++)
	{
		serie.codigo[i] = serie.tituloPortugues[i];
		serie.codigo[i] = toupper(serie.codigo[i]);
	}
	
	for (i = 0; i < 3; i++)
	{
		serie.codigo[i + 3] = serie.diretor[i];
		serie.codigo[i + 3] = toupper(serie.codigo[i + 3]);
	}

	serie.codigo[6] = serie.anoEstreia[2];
	serie.codigo[7] = serie.anoEstreia[3];
	serie.codigo[8] = '\0';

	if (buscarChavePrimaria(primary_index, serie.codigo, 0, *qtdePrimaryIndex, PROCURA_IGUAL))
	{
		// Achou uma serie que possui a mesma chave primaria, o que nao eh permitido

		return FALSE;
	}

	fclose(f);
	fopen("series.dat", "r");
	fseek(f, 0, SEEK_END);	// Move o cursor de leitura para o fim do arquivo	
    rrn = ftell(f);  		// ftell retorna a posição, em bytes, corrente de leitura, neste caso
	fclose(f);

	// Escreve no final do arquivo de dados os campos da serie 

    f = fopen("series.dat", "a+");

	fprintf(f, "%s", serie.codigo);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.tituloPortugues);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.tituloOriginal);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.diretor);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.genero);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.duracao);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.anoEstreia);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.anoEncerramento);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.numeroTemporadas);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.numeroEpisodios);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.pais);
	fprintf(f, "%c", '@');
	fprintf(f, "%d", serie.nota);
	fprintf(f, "%c", '@');
	fprintf(f, "%s", serie.resumo);
	fprintf(f, "%c", '@');

	// Termina de preencher o bloco de 1024 bytes com '#'
	for (i = 0; i < (1024 - tamanho); i++)
	{
		fprintf(f, "%c", '#');
	}

	// Descarrega o buffer
	fflush(f);

	// Incremento dos indices

	criarIndicePrimario(primary_index, qtdePrimaryIndex, serie.codigo, rrn);
	criarIndiceSecundarioTitulo(title_index, qtdeTitleIndex, serie.tituloPortugues, serie.codigo, primary_index, qtdePrimaryIndex);
	criarIndiceSecundarioDiretor(director_index, qtdeDirectorIndex, serie.diretor, serie.codigo, primary_index, qtdePrimaryIndex);

	ordenarIndicePrimario(primary_index, *qtdePrimaryIndex);
	ordenarIndiceSecundarioTitulo(title_index, *qtdeTitleIndex);
	ordenarIndiceSecundarioDiretor(director_index, *qtdeDirectorIndex);

	return TRUE;
}

/* Atualiza a nota de uma determinada serie */
int alterarSerie(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
{
	int novaNota;
	int retorno;
	int contArroba = 0;

	char caractere;
	char chaveBusca[MAX_COD];

	// Leitura da chave primaria da serie a ser alterada

	printf ("\n\nAlteracao da nota de uma serie inicializada\n");
	printf ("Chave primaria da serie: ");
	scanf("\n%[^\n]", chaveBusca);

	retorno = buscarChavePrimaria(primary_index, chaveBusca, 0, qtdePrimaryIndex, BUSCA_RRN);

	if (retorno == -1)
	{
		// Nao achou a serie com a chave primaria informada

		return FALSE;
	}

	// Insercao da nova nota. So eh aceito um valor valido, de 0 a 5

	printf ("Nova nota: ");
	scanf ("%d", &novaNota);

	while ((novaNota < 0) || (novaNota > 5))
	{
		printf ("Nota invalida!\n");
		printf ("Nova nota: ");
		scanf ("%d", &novaNota);
	}

	fclose(f);

	fopen("series.dat", "r+");

	fseek(f, retorno, SEEK_SET);
	
	// Busca da chave retornou o RRN da serie. Cursor do arquivo eh movido para a posicao exata da serie no
	// arquivo de dados. 
	// Em seguida o cursor do arquivo eh direcionado a posicao do campo nota, e a nota eh modificada
	do
	{
		caractere = fgetc(f);

		if (caractere == '@')
		{
			contArroba++;
		}

	}while (contArroba < 11);

	fprintf(f, "%d", novaNota);

	// Descarrega o buffer
	fflush(f);

	return TRUE;
}

/* Remove uma serie, atribuindo |* ao comeco do registro no arquivo de dados */
int removerSerie(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
{
	int retorno;
	char chaveBusca[MAX_COD];


	// Leitura da chave primaria da serie a ser alterada

	printf ("\n\nRemocao de uma serie inicializada\n");
	printf ("Chave primaria da serie: ");
	scanf("\n%[^\n]", chaveBusca);

	retorno = buscarChavePrimaria(primary_index, chaveBusca, 0, qtdePrimaryIndex, BUSCA_RRN);

	if (retorno == -1)
	{
		// Nao achou a serie com a chave primaria informada

		return FALSE;
	}

	fclose(f);

	f = fopen("series.dat", "r+");

	fseek(f, retorno, SEEK_SET);

	// Busca da chave retornou o RRN da serie. Cursor do arquivo eh movido para a posicao exata da serie no
	// arquivo de dados. 
	// Os indicadores de serie removida *| sao inseridos

	
	fprintf(f, "%s", "*|");

	fclose(f);

	f = fopen("series.dat", "a+");

	return TRUE;	
}


/* ============================================================== */
/* == Rotinas que verificam se os arquivos necessarios existem == */
/* ============================================================== */

/* Retorna (1) se o arquivo de dados estiver vazio, e (0) caso contrario */
int arquivoDadosVazio()
{
	return ((fopen("series.dat", "r")) == NULL);
}

/* Retorna (1) se o arquivo do indice primario estiver vazio, e (0) caso contrario */
int indicePrimarioVazio()
{
	return ((fopen("iprimary.idx", "r")) == NULL);
}

/* Retorna (1) se o arquivo do indice secundario por titulo estiver vazio, e (0) caso contrario */
int indiceSecundarioTituloVazio()
{
	return ((fopen("ititle.idx", "r")) == NULL);
}

/* Retorna (1) se o arquivo do indice secundario por diretor estiver vazio, e (0) caso contrario */
int indiceSecundarioDiretorVazio()
{
	return ((fopen("idirector.idx", "r")) == NULL);
}


/* ============================================================== */
/* ============= Rotinas que criam os indices em RAM ============ */
/* ============================================================== */

/* Recupera todos os dados do arquivo de dados */
void recuperarDados(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, TITLE_INDEX *title_index, int *qtdeTitleIndex, DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex)
{
	FILE *f;

	Serie serie;

	int rrn;
	char *ptrString;
	char recoverData[1024];

	f = fopen("series.dat", "r");

	rrn = 0;

	// Condicao de parada eh chegar ao final do arquivo de dados
	while (fgets(recoverData, 1025, f))
	{
		// Desmembramento do registro em seus campos, ignorando os delimitadores

		ptrString = strtok(recoverData, "@");
		strcpy(serie.codigo, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.tituloPortugues, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.tituloOriginal, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.diretor, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.genero, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.duracao, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.anoEstreia, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.anoEncerramento, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.numeroTemporadas, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.numeroEpisodios, ptrString);

		ptrString = strtok(NULL, "@");
		strcpy(serie.pais, ptrString);

		ptrString = strtok(NULL, "@"); 
		serie.nota = *ptrString - '0';
		
		ptrString = strtok(NULL, "@");
		strcpy(serie.resumo, ptrString);


		// Incrementando os indices

		criarIndicePrimario(primary_index, qtdePrimaryIndex, serie.codigo, rrn);
		criarIndiceSecundarioTitulo(title_index, qtdeTitleIndex, serie.tituloPortugues, serie.codigo, primary_index, qtdePrimaryIndex);
		criarIndiceSecundarioDiretor(director_index, qtdeDirectorIndex, serie.diretor, serie.codigo, primary_index, qtdePrimaryIndex);

		rrn = rrn + 1024;
	}

	// Ordenacao e gravacao dos indices em disco

	ordenarIndicePrimario(primary_index, *qtdePrimaryIndex);
	ordenarIndiceSecundarioTitulo(title_index, *qtdeTitleIndex);
	ordenarIndiceSecundarioDiretor(director_index, *qtdeDirectorIndex);

	gravarIndicePrimario(primary_index, qtdePrimaryIndex, INICIANDO);
	gravarIndiceSecundarioTitulo(title_index, qtdeTitleIndex, INICIANDO);
	gravarIndiceSecundarioDiretor(director_index, qtdeDirectorIndex, INICIANDO);

	fclose(f);
}

/* Refaz o indice primario a partir do arquivo 'iprimary.idx' */
void refazerIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	FILE *f;

	int i;
	int rrn;

	char caractere;
	char *ptr_string;
	char RRN[1000000];
	char codigo[MAX_COD];

	f = fopen("iprimary.idx", "r");

	// Faz f apontar para o primeiro caractere do primeiro codigo do arquivo
	fseek(f, 2, SEEK_SET);

	// Condicao de parada eh chegar ao final do arquivo de indice primario
	do
	{
		i = 0;

		// Recuperacao da chave primaria
		do
		{
			caractere = fgetc(f);

			if ((caractere != '@') && (caractere != EOF))
			{
				codigo[i] = caractere;
				i++;
			}

		}while ((caractere != '@') && (caractere != EOF));

		codigo[i] = '\0';
		i = 0;

		if (caractere != EOF)
		{
			// Recuperacao do RRN
			do
			{
				caractere = fgetc(f);
				
				if (caractere != '@')
				{
					RRN[i] = caractere;
					i++;
				}

			}while (caractere != '@');

			RRN[i] = '\0';
			ptr_string = RRN;
			sscanf(ptr_string, "%d", &rrn);

			// Incrementa o indice primario
			criarIndicePrimario(primary_index, qtdePrimaryIndex, codigo, rrn);
		}

	}while (caractere != EOF);

	fclose(f);

	// Grava o indice primario em disco
	gravarIndicePrimario(primary_index, qtdePrimaryIndex, INICIANDO);
}

/* Refaz o indice secundario por titulo a partir do arquivo 'ititle.idx' */
void refazerIndiceSecundarioTitulo(TITLE_INDEX *title_index, int *qtdeTitleIndex, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	FILE *f;

	int i;

	char caractere;
	char codigo[MAX_COD];
	char titulo[MAX_STRING];

	f = fopen("ititle.idx", "r");

	// Faz f apontar para o primeiro caractere do primeiro codigo do arquivo
	fseek(f, 2, SEEK_SET);

	// Condicao de parada eh chegar ao final do arquivo de indice secundario
	do
	{
		i = 0;

		// Recuperacao do titulo em portugues
		do
		{
			caractere = fgetc(f);

			if ((caractere != '@') && (caractere != EOF))
			{
				titulo[i] = caractere;
				i++;
			}

		}while ((caractere != '@') && (caractere != EOF));

		titulo[i] = '\0';
		i = 0;

		if (caractere != EOF)
		{
			// Recuperacao da chave primaria
			do
			{
				caractere = fgetc(f);
				
				if (caractere != '@')
				{
					codigo[i] = caractere;
					i++;
				}

			}while (caractere != '@');

			codigo[i] = '\0';

			// Incrementa o indice secundario
			criarIndiceSecundarioTitulo(title_index, qtdeTitleIndex, titulo, codigo, primary_index, qtdePrimaryIndex);
		}

	}while (caractere != EOF);

	fclose(f);

	// Grava o indice secundario em disco
	gravarIndiceSecundarioTitulo(title_index, qtdeTitleIndex, INICIANDO);
}

/* Refaz o indice secundario por diretor a partir do arquivo 'idirector.idx' */
void refazerIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	FILE *f;

	int i;

	char caractere;
	char codigo[MAX_COD];
	char diretor[MAX_STRING];

	f = fopen("idirector.idx", "r");

	// Faz f apontar para o primeiro caractere do primeiro codigo do arquivo
	fseek(f, 2, SEEK_SET);

	// Condicao de parada eh chegar ao final do arquivo de indice primario
	do
	{
		i = 0;

		// Recuperacao do sobrenome-nome do diretor
		do
		{
			caractere = fgetc(f);

			if ((caractere != '@') && (caractere != EOF))
			{
				diretor[i] = caractere;
				i++;
			}

		}while ((caractere != '@') && (caractere != EOF));

		diretor[i] = '\0';
		i = 0;

		if (caractere != EOF)
		{
			// Recuperacao da chave primaria
			do
			{
				caractere = fgetc(f);
				
				if (caractere != '@')
				{
					codigo[i] = caractere;
					i++;
				}

			}while (caractere != '@');

			codigo[i] = '\0';

			// Incrementa o indice secundario
			criarIndiceSecundarioDiretor(director_index, qtdeDirectorIndex, diretor, codigo, primary_index, qtdePrimaryIndex);
		}

	}while (caractere != EOF);

	fclose(f);

	// Grava o indice secundario em disco
	gravarIndiceSecundarioDiretor(director_index, qtdeDirectorIndex, INICIANDO);
}

/* Cria um indice primario em RAM a partir do arquivo de dados */
void criarIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, char codigo[MAX_COD], int rrn)
{
	// Guarda informacoes e incrementa o contador do indice 
	strcpy(primary_index[*qtdePrimaryIndex].primaryCod, codigo);
	primary_index[*qtdePrimaryIndex].RRN = rrn;
	*qtdePrimaryIndex = *qtdePrimaryIndex + 1;
}

/* Cria o indice secundario por titulo em RAM a partir do arquivo de dados */
void criarIndiceSecundarioTitulo(TITLE_INDEX *title_index, int *qtdeTitleIndex, char titulo[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	int retorno;

	retorno = buscarTituloPortugues(title_index, titulo, 0, *qtdeTitleIndex, primary_index, *qtdePrimaryIndex, PROCURA_IGUAL);

	// Guarda informacoes
	if (retorno != -1)
	{
		// Titulo em portugues nao existente, ou seja, chave secundaria nova. A chave primaria sera guardada em sua propria posicao 
		title_index[retorno].contRepetido = title_index[retorno].contRepetido + 1;
		strcpy(title_index[retorno].primaryCod[title_index[retorno].contRepetido], codigo);		
	}
	else
	{
		// Titulo em portugues ja existente. A chave primaria sera guardada no vetor de chaves primarias correspondente
		// aquele titulo em portugues em questao
		strcpy(title_index[*qtdeTitleIndex].primaryCod[title_index[*qtdeTitleIndex].contRepetido], codigo);
		strcpy(title_index[*qtdeTitleIndex].tituloPortugues, titulo);
		*qtdeTitleIndex = *qtdeTitleIndex + 1;
	}

}

/* Cria o indice secundario por diretor em RAM a partir do arquivo de dados */
void criarIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, char diretor[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	int retorno;

	retorno = buscarDiretor(director_index, diretor, 0, *qtdeDirectorIndex, primary_index, *qtdePrimaryIndex, PROCURA_IGUAL); 

	// Guarda informacoes
	if (retorno != -1)
	{
		// Diretor nao existente, ou seja, chave secundaria nova. A chave primaria sera guardada em sua propria posicao 
		director_index[retorno].contRepetido = director_index[retorno].contRepetido + 1;
		strcpy(director_index[retorno].primaryCod[director_index[retorno].contRepetido], codigo);
	}
	else
	{
		// Diretor ja existente. A chave primaria sera guardada no vetor de chaves primarias correspondente
		// aquele diretor em questao
		strcpy(director_index[*qtdeDirectorIndex].primaryCod[director_index[*qtdeDirectorIndex].contRepetido], codigo);
		strcpy(director_index[*qtdeDirectorIndex].diretor, diretor);
		*qtdeDirectorIndex = *qtdeDirectorIndex + 1;
	}
}


/* ============================================================== */
/* ========== Rotinas que gravam os indices em arquivos ========= */
/* ============================================================== */

/* Grava o indice primario no arquivo 'imprimary.idx' */
void gravarIndicePrimario(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, int estadoSistema)
{
	FILE *f;

	int i;

	f = fopen("iprimary.idx", "w");

	// Se for o inicio do sistema, entao deve-se guardar 0 na primeira posicao do arquivo
	// Isso indica que os dados estao inconsistentes
	// Caso contrario, ou seja, eh a finalizacao do sistema, entao deve-se guardar 1 na 
	// primeira posicao do arquivo. Isso indica que os dados estao consistentes
	if (estadoSistema == INICIANDO)
	{
		fprintf(f, "%c", '0');
	}
	else
	{
		fprintf(f, "%c", '1');
	}

	fprintf(f, "%c", '@');

	// Guarda todos os dados no arquivo de indice
	for (i = 0; i < *qtdePrimaryIndex; i++)
	{
		fprintf(f, "%s", primary_index[i].primaryCod);
		fprintf(f, "%c", '@');
		fprintf(f, "%d", primary_index[i].RRN);
		fprintf(f, "%c", '@');
	}

	fclose(f);
}

/* Grava o indice secundario por titulo no arquivo 'ititle.idx' */
void gravarIndiceSecundarioTitulo(TITLE_INDEX *title_index, int *qtdeTitleIndex, int estadoSistema)
{
	FILE *f;

	int i;
	int j;

	f = fopen("ititle.idx", "w");

	// Se for o inicio do sistema, entao deve-se guardar 0 na primeira posicao do arquivo
	// Isso indica que os dados estao inconsistentes
	// Caso contrario, ou seja, eh a finalizacao do sistema, entao deve-se guardar 1 na 
	// primeira posicao do arquivo. Isso indica que os dados estao consistentes
	if (estadoSistema == INICIANDO)
	{
		fprintf(f, "%c", '0');
	}
	else
	{
		fprintf(f, "%c", '1');
	}

	fprintf(f, "%c", '@');

	// Guarda todos os dados no arquivo
	for (i = 0; i < *qtdeTitleIndex; i++)
	{
		for (j = 0; j <= title_index[i].contRepetido; j++)
		{
			fprintf(f, "%s", title_index[i].tituloPortugues);
			fprintf(f, "%c", '@');
			fprintf(f, "%s", title_index[i].primaryCod[j]);
			fprintf(f, "%c", '@');			
		}
	}

	fclose(f);
}

/* Grava o indice secundario por diretor no arquivo 'idirector.idx' */
void gravarIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int *qtdeDirectorIndex, int estadoSistema)
{
	FILE *f;

	int i;
	int j;

	f = fopen("idirector.idx", "w");

	// Se for o inicio do sistema, entao deve-se guardar 0 na primeira posicao do arquivo
	// Isso indica que os dados estao inconsistentes
	// Caso contrario, ou seja, eh a finalizacao do sistema, entao deve-se guardar 1 na 
	// primeira posicao do arquivo. Isso indica que os dados estao consistentes
	if (estadoSistema == INICIANDO)
	{
		fprintf(f, "%c", '0');
	}
	else
	{
		fprintf(f, "%c", '1');
	}

	fprintf(f, "%c", '@');

	// Guarda todos os dados no arquivo
	for (i = 0; i < *qtdeDirectorIndex; i++)
	{
		for (j = 0; j <= director_index[i].contRepetido; j++)
		{
			fprintf(f, "%s", director_index[i].diretor);
			fprintf(f, "%c", '@');
			fprintf(f, "%s", director_index[i].primaryCod[j]);
			fprintf(f, "%c", '@');			
		}
	}

	fclose(f);
}


/* ============================================================== */
/* ================= Rotinas de busca por chave ================= */
/* ============================================================== */

/* Realiza busca binaria com base na chave primaria */
int buscarChavePrimaria(PRIMARY_INDEX *primary_index, char chaveBusca[MAX_COD], int esquerda, int direita, int choice)
{
	FILE *f;

	int meio;
	int nota;

	char recoverData[1024];
	char *ptrString;

	// Busca binaria pelo vetor de indice de chave primaria
	// choice indica qual sera o tipo de retorno dessa funcao
	// Em BUSCA_RRN retorno eh o RRN da serie
	// Em PROCURA_IGUAL retorna TRUE se achar uma serie de mesma chave primaria
	// Do contrario, se a serie for achada e nao estiver marcada para remocao, seus dados sao impressos

	if (esquerda > direita)
	{
		if (choice == BUSCA_RRN)
		{
			return -1;
		}

		return FALSE;
	}

	meio = (esquerda + direita) / 2;

	if (!strcmp(primary_index[meio].primaryCod, chaveBusca))
	{
		if (choice == PROCURA_IGUAL)
		{
			return TRUE;
		}

		if (choice == BUSCA_RRN)
		{
			return primary_index[meio].RRN;
		}

		f = fopen("series.dat", "r");

		fseek(f, primary_index[meio].RRN, SEEK_SET);
		fgets(recoverData, 1025, f);

		if ((recoverData[0] == '*') && (recoverData[1] == '|'))
		{
			fclose(f);

			return FALSE;
		}

		printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
		printf("Titulo portugues: %s\n", strtok(NULL, "@"));
		printf("Titulo original: %s\n", strtok(NULL, "@"));
		printf("Diretor: %s\n", strtok(NULL, "@"));
		printf("Genero: %s\n", strtok(NULL, "@"));
		printf("Duracao: %s\n", strtok(NULL, "@"));
		printf("Ano estreia: %s\n", strtok(NULL, "@"));
		printf("Ano encerramento: %s\n", strtok(NULL, "@"));
		printf("Numero temporadas: %s\n", strtok(NULL, "@"));
		printf("Numero episodios: %s\n", strtok(NULL, "@"));
		printf("Pais: %s\n", strtok(NULL, "@"));
		ptrString = strtok(NULL, "@");
		nota = *ptrString - '0';
		printf("Nota: %d\n", nota);
		printf("Resumo: %s\n", strtok(NULL, "@"));

		fclose(f);

		return TRUE;
	}
	else
	{
		if (strcmp(primary_index[meio].primaryCod, chaveBusca) > 0)
		{
			return buscarChavePrimaria(primary_index, chaveBusca, esquerda, meio - 1, choice);
		}
		else
		{
			return buscarChavePrimaria(primary_index, chaveBusca, meio + 1, direita, choice);
		}
	}
}

/* Realiza busca binaria com base no titulo em portugues */
int buscarTituloPortugues(TITLE_INDEX *title_index, char tituloBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice)
{
	FILE *f;

	int i;
	int rrn;
	int meio;
	int nota;
	int imprimiuAlgum;

	char recoverData[1024];
	char *ptrString;

	// Busca binaria pelo vetor de indice de chave primaria
	// choice indica qual sera o tipo de retorno dessa funcao
	// Em PROCURA_IGUAL retorna TRUE se achar uma serie de mesma chave primaria
	// Do contrario, se a serie for achada e nao estiver marcada para remocao, seus dados sao impressos

	if (esquerda > direita)
	{
		if (choice == PROCURA_IGUAL)
		{
			return -1;
		}

		return FALSE;
	}

	meio = (esquerda + direita) / 2;

	if (!strcmp(title_index[meio].tituloPortugues, tituloBusca))
	{
		if (choice == PROCURA_IGUAL)
		{
			return meio;
		}

		f = fopen("series.dat", "r");

		imprimiuAlgum = FALSE;

		for (i = 0; i <= title_index[meio].contRepetido; i++)
		{
			rrn = buscarChavePrimaria(primary_index, title_index[meio].primaryCod[i], 0, qtdePrimaryIndex, BUSCA_RRN);
			
			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);
			
			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf("Titulo original: %s\n", strtok(NULL, "@"));
				printf("Diretor: %s\n", strtok(NULL, "@"));
				printf("Genero: %s\n", strtok(NULL, "@"));
				printf("Duracao: %s\n", strtok(NULL, "@"));
				printf("Ano estreia: %s\n", strtok(NULL, "@"));
				printf("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf("Numero episodios: %s\n", strtok(NULL, "@"));
				printf("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf("Nota: %d\n", nota);
				printf("Resumo: %s\n", strtok(NULL, "@"));
			
				imprimiuAlgum = TRUE;
			}
		}

		fclose(f);

		if (!imprimiuAlgum)
		{
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		if (strcmp(title_index[meio].tituloPortugues, tituloBusca) > 0)
		{
			return buscarTituloPortugues(title_index, tituloBusca, esquerda, meio - 1, primary_index, qtdePrimaryIndex, choice);
		}
		else
		{
			return buscarTituloPortugues(title_index, tituloBusca, meio + 1, direita, primary_index, qtdePrimaryIndex, choice);
		}
	}
}

/* Realiza busca binaria com base no titulo em portugues */
int buscarDiretor(DIRECTOR_INDEX *director_index, char diretorBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice)
{
	FILE *f;

	int i;
	int rrn;
	int meio;
	int nota;
	int imprimiuAlgum;

	char recoverData[1024];
	char *ptrString;

	// Busca binaria pelo vetor de indice de chave primaria
	// choice indica qual sera o tipo de retorno dessa funcao
	// Em PROCURA_IGUAL retorna TRUE se achar uma serie de mesma chave primaria
	// Do contrario, se a serie for achada e nao estiver marcada para remocao, seus dados sao impressos

	if (esquerda > direita)
	{
		if (choice == PROCURA_IGUAL)
		{
			return -1;
		}

		return FALSE;
	}

	meio = (esquerda + direita) / 2;

	if (!strcmp(director_index[meio].diretor, diretorBusca))
	{
		if (choice == PROCURA_IGUAL)
		{
			return meio;
		}

		f = fopen("series.dat", "r");

		imprimiuAlgum = FALSE;

		for (i = 0; i <= director_index[meio].contRepetido; i++)
		{
			rrn = buscarChavePrimaria(primary_index, director_index[meio].primaryCod[i], 0, qtdePrimaryIndex, BUSCA_RRN);
			
			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);

			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{				
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf("Titulo original: %s\n", strtok(NULL, "@"));
				printf("Diretor: %s\n", strtok(NULL, "@"));
				printf("Genero: %s\n", strtok(NULL, "@"));
				printf("Duracao: %s\n", strtok(NULL, "@"));
				printf("Ano estreia: %s\n", strtok(NULL, "@"));
				printf("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf("Numero episodios: %s\n", strtok(NULL, "@"));
				printf("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf("Nota: %d\n", nota);
				printf("Resumo: %s\n", strtok(NULL, "@"));
			
				imprimiuAlgum = TRUE;
			}
		}

		fclose(f);

		if (!imprimiuAlgum)
		{
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		if (strcmp(director_index[meio].diretor, diretorBusca) > 0)
		{
			return buscarDiretor(director_index, diretorBusca, esquerda, meio - 1, primary_index, qtdePrimaryIndex, choice);
		}
		else
		{
			return buscarDiretor(director_index, diretorBusca, meio + 1, direita, primary_index, qtdePrimaryIndex, choice);
		}
	}
}


/* ============================================================== */
/* =============== Rotinas de ordenacao de indices ============== */
/* ============================================================== */

/* Ordena lexograficamente em ordem crescente o indice de chaves primarias */
void ordenarIndicePrimario(PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
{
	int i;
	int j;

	PRIMARY_INDEX aux;

	for (i = 0; i < qtdePrimaryIndex; i++)
	{
		for (j = 0; j < qtdePrimaryIndex - 1; j++)
		{
			if (strcmp(primary_index[j].primaryCod, primary_index[j + 1].primaryCod) > 0)
			{
				aux = primary_index[j];
				primary_index[j] = primary_index[j + 1];
				primary_index[j + 1] = aux;
			}
		}
	}
}

/* Ordena lexograficamente em ordem crescente o indice de chaves secundarias por titulo */
void ordenarIndiceSecundarioTitulo(TITLE_INDEX *title_index, int qtdeTitleIndex)
{
	int i;
	int j;

	TITLE_INDEX aux;

	char aux_chavePrimaria[MAX_COD];

	for (i = 0; i < qtdeTitleIndex; i++)
	{
		for (j = 0; j < qtdeTitleIndex - 1; j++)
		{
			if (strcmp(title_index[j].tituloPortugues, title_index[j + 1].tituloPortugues) > 0)
			{
				aux = title_index[j];
				title_index[j] = title_index[j + 1];
				title_index[j + 1] = aux;
			}
		}
	}

	for (i = 0; i < qtdeTitleIndex; i++)
	{
		for (j = 0; j < title_index[i].contRepetido; j++)
		{
			if (strcmp(title_index[i].primaryCod[j], title_index[i].primaryCod[j + 1]) > 0)
			{
				strcpy(aux_chavePrimaria, title_index[i].primaryCod[j]);
				strcpy(title_index[i].primaryCod[j], title_index[i].primaryCod[j + 1]);
				strcpy(title_index[i].primaryCod[j + 1], aux_chavePrimaria);
			}
		}
	}	
}

/* Ordena lexograficamente em ordem crescente o indice de chaves secundarias por diretor */
void ordenarIndiceSecundarioDiretor(DIRECTOR_INDEX *director_index, int qtdeDirectorIndex)
{
	int i;
	int j;

	DIRECTOR_INDEX aux;

	char aux_chavePrimaria[MAX_COD];

	for (i = 0; i < qtdeDirectorIndex; i++)
	{
		for (j = 0; j < qtdeDirectorIndex - 1; j++)
		{
			if (strcmp(director_index[j].diretor, director_index[j + 1].diretor) > 0)
			{
				aux = director_index[j];
				director_index[j] = director_index[j + 1];
				director_index[j + 1] = aux;
			}
		}
	}

	for (i = 0; i < qtdeDirectorIndex; i++)
	{
		for (j = 0; j < director_index[i].contRepetido; j++)
		{
			if (strcmp(director_index[i].primaryCod[j], director_index[i].primaryCod[j + 1]) > 0)
			{
				strcpy(aux_chavePrimaria, director_index[i].primaryCod[j]);
				strcpy(director_index[i].primaryCod[j], director_index[i].primaryCod[j + 1]);
				strcpy(director_index[i].primaryCod[j + 1], aux_chavePrimaria);
			}
		}
	}
}


/* ============================================================== */
/* =============== Rotinas de listagem das series =============== */
/* ============================================================== */

/* Lista as series em ordem crescente lexograficamente dos codigos */
int listarSeriesCodigo(FILE *f, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
{
	int i;
	int nota;
	int imprimiuAlgum;

	char *ptrString;
	char recoverData[1024];

	if (arquivoDadosVazio())
	{
		return FALSE;
	}

	imprimiuAlgum = FALSE;

	for (i = 0; i < qtdePrimaryIndex; i++)
	{
		fseek(f, primary_index[i].RRN, SEEK_SET);
		fgets(recoverData, 1025, f);

		if ((recoverData[0] != '*') && (recoverData[1] != '|'))
		{
			printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
			printf("Titulo portugues: %s\n", strtok(NULL, "@"));
			printf("Titulo original: %s\n", strtok(NULL, "@"));
			printf("Diretor: %s\n", strtok(NULL, "@"));
			printf("Genero: %s\n", strtok(NULL, "@"));
			printf("Duracao: %s\n", strtok(NULL, "@"));
			printf("Ano estreia: %s\n", strtok(NULL, "@"));
			printf("Ano encerramento: %s\n", strtok(NULL, "@"));
			printf("Numero temporadas: %s\n", strtok(NULL, "@"));
			printf("Numero episodios: %s\n", strtok(NULL, "@"));
			printf("Pais: %s\n", strtok(NULL, "@"));
			ptrString = strtok(NULL, "@");
			nota = *ptrString - '0';
			printf("Nota: %d\n", nota);
			printf("Resumo: %s\n\n", strtok(NULL, "@"));		

			imprimiuAlgum = TRUE;
		}
	}

	if (!imprimiuAlgum)
	{
		return FALSE;
	}

	return TRUE;
}

/* Lista as series em ordem crescente lexograficamente dos titulos em portugues */
int listarSeriesTitulo(FILE *f, TITLE_INDEX *title_index, int qtdeTitleIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
{
	int i;
	int j;
	int rrn;
	int nota;
	int imprimiuAlgum;

	char *ptrString;
	char recoverData[1024];

	if (arquivoDadosVazio())
	{
		return FALSE;
	}

	imprimiuAlgum = FALSE;

	for (i = 0; i < qtdeTitleIndex; i++)
	{
		for (j = 0; j <= title_index[i].contRepetido; j++)
		{
			rrn = buscarChavePrimaria(primary_index, title_index[i].primaryCod[j], 0, qtdePrimaryIndex, BUSCA_RRN);

			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);

			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf("Titulo original: %s\n", strtok(NULL, "@"));
				printf("Diretor: %s\n", strtok(NULL, "@"));
				printf("Genero: %s\n", strtok(NULL, "@"));
				printf("Duracao: %s\n", strtok(NULL, "@"));
				printf("Ano estreia: %s\n", strtok(NULL, "@"));
				printf("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf("Numero episodios: %s\n", strtok(NULL, "@"));
				printf("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf("Nota: %d\n", nota);
				printf("Resumo: %s\n\n", strtok(NULL, "@"));		

				imprimiuAlgum = TRUE;
			}
		}
	}

	if (!imprimiuAlgum)
	{
		return FALSE;
	}

	return TRUE;
}

/* Lista as series em ordem crescente lexograficamente dos diretores */
int listarSeriesDiretor(FILE *f, DIRECTOR_INDEX *director_index, int qtdeDirectorIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
{
	int i;
	int j;
	int rrn;
	int nota;
	int imprimiuAlgum;

	char *ptrString;
	char recoverData[1024];

	if (arquivoDadosVazio())
	{
		return FALSE;
	}

	imprimiuAlgum = FALSE;

	for (i = 0; i < qtdeDirectorIndex; i++)
	{
		for (j = 0; j <= director_index[i].contRepetido; j++)
		{
			rrn = buscarChavePrimaria(primary_index, director_index[i].primaryCod[j], 0, qtdePrimaryIndex, BUSCA_RRN);

			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);

			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf("Titulo original: %s\n", strtok(NULL, "@"));
				printf("Diretor: %s\n", strtok(NULL, "@"));
				printf("Genero: %s\n", strtok(NULL, "@"));
				printf("Duracao: %s\n", strtok(NULL, "@"));
				printf("Ano estreia: %s\n", strtok(NULL, "@"));
				printf("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf("Numero episodios: %s\n", strtok(NULL, "@"));
				printf("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf("Nota: %d\n", nota);
				printf("Resumo: %s\n\n", strtok(NULL, "@"));		
			
				imprimiuAlgum = TRUE;
			}
		}
	}

	if (!imprimiuAlgum)
	{
		return FALSE;
	}

	return TRUE;
}
