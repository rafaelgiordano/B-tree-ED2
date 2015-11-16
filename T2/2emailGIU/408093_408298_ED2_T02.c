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
#include "408093_408298_ED2_T02.h"

// Variavel global referenciada
extern int ordem;

/* ================================================================== */
/* ========== FUNCOES QUE DETERMINAM AS OPERACOES POSSIVEIS ========= */
/* ================================================================== */


/* Inicializa os contadores de cada indice e a quantidade de registros de cada indice */
void inicializarIndices(int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex, int max_size)
{
	int i;

	// Inicializa com 0 o contador de chaves secundarias repetidas de cada posicao dos indices secundarios
	for (i = 0; i < max_size; i++)
	{
		ititle[i].contRepetido = 0;
		idirector[i].contRepetido = 0;
	}

	// Inicializa a quantidade de cada indice
	*qtdePrimaryIndex = 0;
	*qtdeTitleIndex = 0;
	*qtdeDirectorIndex = 0;
}

/* Remocao fisica de todos os registros marcados como excluidos e atualizacao dos indices */
void liberarEspaco(FILE *f, PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex, int max_size)
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
	inicializarIndices(qtdePrimaryIndex, ititle, qtdeTitleIndex, idirector, qtdeDirectorIndex, max_size);

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
			criarIndiceSecundarioTitulo(ititle, qtdeTitleIndex, serie.tituloPortugues, serie.codigo, primary_index, qtdePrimaryIndex);
			criarIndiceSecundarioDiretor(idirector, qtdeDirectorIndex, serie.diretor, serie.codigo, primary_index, qtdePrimaryIndex);

			ordenarIndicePrimario(primary_index, *qtdePrimaryIndex);
			ordenarIndiceSecundarioTitulo(ititle, *qtdeTitleIndex);
			ordenarIndiceSecundarioDiretor(idirector, *qtdeDirectorIndex);

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
/* ============= Rotinas de manipulacao da arvore-B ============= */
/* ============================================================== */

/* -------------------- Rotinas de Insercao --------------------- */

/* Inicializa a arvore-B */
Btree *inicializaBtree(Btree *t)
{
	printf ("\n--\nFuncao inicializaBtree - Inicio\n");
	t = criaNo();
	t->numKeys = 0;
	t->leaf = TRUE;
	printf ("Funcao inicializaBtree - Termino\n--\n");

	return t;
}

/* Insere uma chave primaria na arvore-B */
Btree *insereChave(Btree *t, char codigo[MAX_COD], int rrn)
{
	Node *r;
	Node *s;

	printf ("\n--\nFuncao insereChave - Inicio\n");

	r = t;
	printf ("r->numKeys: %d\n", r->numKeys);
	if (r->numKeys == ordem - 1)
	{
		// Ocorrencia de overflow.

		printf ("r->numKeys == ordem - 1\n");
		s = criaNo();
		printf ("criou o no s\n");
		t = s;
		s->leaf = FALSE;
		s->numKeys = 0;
		printf ("Numero de chaves da raiz: %d.\n", t->numKeys);
		s->desc[0] = r;
		divideNo(s, 0, r);
		printf ("dividiu o no\n");
		insereNoNaoCheio(s, codigo, rrn);
		printf ("inseriu o codigo.\n");
	}
	else
	{
		// No nao esta cheio, insercao da chave ocorre normalmente.
		printf ("r->numKeys != ordem - 1\n");
		insereNoNaoCheio(r, codigo, rrn);
	}

	printf ("Funcao insereChave - Termino\n--\n");

	return t;
}

/* Insere uma chave primaria em um no nao cheio da arvore-B */
void insereNoNaoCheio(Node *x, char codigo[MAX_COD], int rrn)
{
	int i;

	printf ("\n--\nFuncao insereNoNaoCheio - Inicio\n");

	i = x->numKeys;

	if (x->leaf)
	{
		printf ("Folha!\n");
		printf ("i: %d\n", i);
		while ((i - 1 >= 0) && ((strcmp(codigo, x->codigo[i - 1])) < 0))
		{
			strcpy(x->codigo[i], x->codigo[i - 1]);
			x->RRN[i] = x->RRN[i - 1];
			i--;
			printf ("i: %d\n", i - 1);
		}

		strcpy(x->codigo[i], codigo);
		x->RRN[i] = rrn;
		x->numKeys++;
		printf ("Inseriu chave %s na posicao %d do vetor de chaves.\n", x->codigo[i], i);
		printf("Numero de chaves atualizado: %d.\n", x->numKeys);
	}

	if (!x->leaf)
	{
		printf ("Nao Folha!\n");
		printf("x->numKeys: %d\n", x->numKeys);
		printf ("x->codigo[%d - 1]: %s\n", i, x->codigo[i - 1]);
		while ((i - 1 >= 0) && ((strcmp(codigo, x->codigo[i - 1])) < 0))
		{
			printf ("i: %d\n", i);
			i--;
		}
		i++;

		if (x->desc[i - 1]->numKeys == ordem - 1)
		{
			divideNo(x, i - 1, x->desc[i - 1]);

			if ((strcmp(codigo, x->codigo[i - 1])) > 0)
			{
				i++;
			}
		}

		insereNoNaoCheio(x->desc[i - 1], codigo, rrn);
	}

	printf ("Funcao insereNoNaoCheio - Termino\n--\n");
}

/* Realiza o split de um no da arvore-B */
void divideNo(Node *pai, int i, Node *filho)
{
	int t;
	int j;

	Node *novoNo;

	printf ("\n--\nFuncao divideNo - Inicio\n");

	t = floor((ordem - 1) / 2);

	novoNo = criaNo();
	novoNo->leaf = filho->leaf;
	novoNo->numKeys = t - 1;

	printf ("t: %d.\n", t);
	printf ("No-filho\n");
	printf ("Chave 0: %s.\n", filho->codigo[0]);
	printf ("Chave 1: %s.\n", filho->codigo[0 + t]);

	for (j = 0; j <= t - 1; j++)
	{
		printf("j: %d\n", j);
		printf ("Copiou a chave %s para o novo no criado.\n", filho->codigo[j + t]);
		strcpy(novoNo->codigo[j], filho->codigo[j + t]);
		novoNo->RRN[j] = filho->RRN[j + t];
	}

	printf ("Passou do primeiro laco.\n");

	if (!filho->leaf)
	{
		for (j = 0; j < t; j++)
		{
			novoNo->desc[j] = filho->desc[j + t];
		}
	}
	else
	{
		// else acrescentado somente para teste.
		printf ("No filho eh folha.\n");
	}
	printf ("Numero de chaves do no filho reduziu para %d.\n", t);
	filho->numKeys = t;
	printf ("Numero de chaves do no pai: %d.\n", pai->numKeys);

	for (j = pai->numKeys + 1; j > i + 1; j--)
	{
		printf ("Abrindo espaco para os descendentes do no pai.\n");
		pai->desc[j + 1] = pai->desc[j];
	}
	pai->desc[i + 1] = novoNo;
	printf("Filho 0 do no pai tem chave 0: %s.\n", pai->desc[0]->codigo[0]);
	printf("Filho 1 do no pai tem chave 0: %s.\n", pai->desc[1]->codigo[0]);

	for (j = pai->numKeys; j > i; j--)
	{
		printf ("Copiou a chave %s uma posicao a frente do no pai.\n", pai->codigo[j]);
		strcpy(pai->codigo[j + 1], pai->codigo[j]);
		pai->RRN[j + 1] = pai->RRN[j];
	}
	printf ("Copiou a chave %s para a posicao 0 das chaves do no pai.\n", filho->codigo[t]);
	strcpy(pai->codigo[i], filho->codigo[t]);
	pai->RRN[i] = filho->RRN[t];
	strcpy(filho->codigo[t], ""); // apagando a chave retirada do no
	pai->numKeys++;

	printf ("Funcao divideNo - Termino\n--\n");
}

/* Cria um novo no e retorna a posicao dele */
Btree *criaNo()
{
	int i;

	Btree *t;

	printf ("\n--\nFuncao criaNo - Inicio\n");
	
	t = (Btree *)malloc(sizeof(Btree));

	if (!t)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}

	t->RRN = (int *)malloc((ordem - 1) * sizeof(int));

	if (!t->RRN)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}

	t->codigo = (char **)malloc((ordem - 1) * sizeof(char *));

	if (!t->codigo)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);	
	}

	for (i = 0; i < ordem - 1; i++)
	{
		t->codigo[i] = (char *)malloc(MAX_COD * sizeof(char));

		if (!t->codigo[i])
		{
			printf (FRASE_ERROALOCACAO);
			exit(0);
		}
		strcpy(t->codigo[i], "");
	}

	t->desc = (Node **)malloc(ordem * sizeof(Node *));

	if (!t->desc)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}

	for (i = 0; i < ordem; i++)
	{
		t->desc[i] = NULL;
	}

	printf ("Funcao criaNo - Termino\n--\n");

	return t;
}

/* ---------------------- Rotinas de Busca ---------------------- */

/* Busca por uma chave na arvore-B */
Node *buscaChave(Node *x, char chave[MAX_COD], int choice)
{
	int i;

	i = 1;

	while ((i - 1 <= x->numKeys - 1) && ((strcmp(chave, x->codigo[i - 1])) > 0))
	{
		if (choice != PROCURA_IGUAL)
		{
			if (i != 1)
			{
				printf (", ");
			}
			printf ("%s", x->codigo[i - 1]);
		}
		i++;
	}

	if (choice != PROCURA_IGUAL)
	{
		if ((i != 1) && (strcmp(x->codigo[i - 1], "") != 0))
		{
			printf (", ");
		}
		printf ("%s\n", x->codigo[i - 1]);
	}

	if ((i - 1 <= x->numKeys - 1) && (!strcmp(chave, x->codigo[i - 1])))
	{
		return x;
	}

	if (x->leaf)
	{
		return NULL;
	}
	else
	{
		return buscaChave(x->desc[i - 1], chave, choice);
	}
}

/* Realiza a busca binaria em um no da arvore-B */
int buscaBinariaNo(char **vetCodigo, char chaveBusca[MAX_COD], int RRN[], int esquerda, int direita, int choice)
{
	FILE *f;

	int meio;

	// printf("esquerda: %d - direita: %d.\n", esquerda, direita);
	int nota;

	char recoverData[1024];
	char *ptrString;

	// Busca binaria pelo no onde esta localizada a chave primaria desejada
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

	if (!strcmp(vetCodigo[meio], chaveBusca))
	{
		if (choice == PROCURA_IGUAL)
		{
			return TRUE;
		}

		if (choice == BUSCA_RRN)
		{
			return RRN[meio];
		}

		// printf ("Funcao buscaBinariaNo\nAchou a chave %s na posicao %d da pagina.\n", chaveBusca, meio);
		printf ("\nDados:\n");

		f = fopen("series.dat", "r");

		fseek(f, RRN[meio], SEEK_SET);
		fgets(recoverData, 1025, f);

		if ((recoverData[0] == '*') && (recoverData[1] == '|'))
		{
			fclose(f);

			return FALSE;
		}

		printf ("Codigo: %s\n", strtok(recoverData, "@"));
		printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
		printf ("Titulo original: %s\n", strtok(NULL, "@"));
		printf ("Diretor: %s\n", strtok(NULL, "@"));
		printf ("Genero: %s\n", strtok(NULL, "@"));
		printf ("Duracao: %s\n", strtok(NULL, "@"));
		printf ("Ano estreia: %s\n", strtok(NULL, "@"));
		printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
		printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
		printf ("Numero episodios: %s\n", strtok(NULL, "@"));
		printf ("Pais: %s\n", strtok(NULL, "@"));
		ptrString = strtok(NULL, "@");
		nota = *ptrString - '0';
		printf ("Nota: %d\n", nota);
		printf ("Resumo: %s\n", strtok(NULL, "@"));

		fclose(f);

		return TRUE;
	}
	else
	{
		if (strcmp(vetCodigo[meio], chaveBusca) > 0)
		{
			return buscaBinariaNo(vetCodigo, chaveBusca, RRN, esquerda, meio - 1, choice);
		}
		else
		{
			return buscaBinariaNo(vetCodigo, chaveBusca, RRN, meio + 1, direita, choice);
		}
	}
}

/* --------------------- Rotinas de Remocao --------------------- */

/* Remove uma chave primaria da arvore-B */
int removeChave(Btree *t, char codigo[MAX_COD])
{
	return FALSE;
}

/* Realiza o balanceamento de um no-folha */
void balanceiaFolha(Btree *t, Node *f)
{

}

/* Realiza a redistribuicao de chaves do no a esquerda do no */
void balanceiaEsqDir(Node *p, int e, int esq, int dir)
{

}

/* Realiza a concatenacao de chaves, resultando em juncao de nos e diminuicao da altura da arvore */
void diminuiAltura(Node *x, Btree *t)
{

}

/* Realiza a juncao de dois nos */
void juncaoNo(Node *x, int i)
{

}


/* ============================================================== */
/* ===== Rotinas que implementam operacoes com os registros ===== */
/* ============================================================== */

/* Insere uma nova serie no arquivo de dados. Retorna (1) se foi inserido com sucesso ou (0) caso
   a chave primaria gerada ja exista.																*/
Btree *inserirSerie(FILE *f, PRIMARY_INDEX *primary_index, Btree *t, int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex)
{
	int i;
	int rrn;
	int tamanho;

	Serie serie;

	// Leitura dos dados das series
	// Foi utilizado expressao regular para tratar o enter do usuario

	printf ("\n\nCadastro de nova serie inicializado\n");
	printf ("Titulo em portugues: ");
	scanf ("\n%[^\n]", serie.tituloPortugues);
	printf ("Titulo original: ");
	getchar();
	if (scanf ("%[^\n]s", serie.tituloOriginal) < 1)
	{
		// Titulo original nao foi informado

		strcpy(serie.tituloOriginal, "Idem");
	}
	if (!strcmp(serie.tituloPortugues, serie.tituloOriginal))
	{
		// Titulo original eh igual ao titulo em portugues

		strcpy(serie.tituloOriginal, "Idem");
	}
	printf ("Diretor: ");
	scanf ("\n%[^\n]", serie.diretor);
	printf ("Genero: ");
	scanf ("\n%[^\n]", serie.genero);
	printf ("Duracao em minutos: ");
	scanf ("\n%[^\n]", serie.duracao);
	printf ("Ano de estreia: ");
	scanf ("\n%[^\n]", serie.anoEstreia);
	printf ("Ano de encerramento: ");
	scanf ("\n%[^\n]", serie.anoEncerramento);
	printf ("Numero de temporadas: ");
	scanf ("\n%[^\n]", serie.numeroTemporadas);
	printf ("Numero de episodios: ");
	scanf ("\n%[^\n]", serie.numeroEpisodios);
	printf ("Pais: ");
	scanf ("\n%[^\n]", serie.pais);
	printf ("Nota: ");
	scanf ("%d", &serie.nota);
	printf ("Resumo: ");
	scanf ("\n%[^\n]", serie.resumo);

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

		return NULL;
	}

	if (buscaChave(t, serie.codigo, PROCURA_IGUAL))
	{
		// Achou uma serie que possui a mesma chave primaria, o que nao eh permitido

		return NULL;
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

	t = insereChave(t, serie.codigo, rrn);

	criarIndicePrimario(primary_index, qtdePrimaryIndex, serie.codigo, rrn);
	criarIndiceSecundarioTitulo(ititle, qtdeTitleIndex, serie.tituloPortugues, serie.codigo, primary_index, qtdePrimaryIndex);
	criarIndiceSecundarioDiretor(idirector, qtdeDirectorIndex, serie.diretor, serie.codigo, primary_index, qtdePrimaryIndex);

	ordenarIndicePrimario(primary_index, *qtdePrimaryIndex);
	ordenarIndiceSecundarioTitulo(ititle, *qtdeTitleIndex);
	ordenarIndiceSecundarioDiretor(idirector, *qtdeDirectorIndex);

	return t;
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
	scanf ("\n%[^\n]", chaveBusca);

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
	scanf ("\n%[^\n]", chaveBusca);

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


/* ============================================================== */
/* ============= Rotinas que criam os indices em RAM ============ */
/* ============================================================== */

/* Recupera todos os dados do arquivo de dados */
void recuperarDados(PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex, ITITLE *ititle, int *qtdeTitleIndex, IDIRECTOR *idirector, int *qtdeDirectorIndex)
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
		criarIndiceSecundarioTitulo(ititle, qtdeTitleIndex, serie.tituloPortugues, serie.codigo, primary_index, qtdePrimaryIndex);
		criarIndiceSecundarioDiretor(idirector, qtdeDirectorIndex, serie.diretor, serie.codigo, primary_index, qtdePrimaryIndex);

		rrn = rrn + 1024;
	}

	// Ordenacao e gravacao dos indices em disco

	ordenarIndicePrimario(primary_index, *qtdePrimaryIndex);
	ordenarIndiceSecundarioTitulo(ititle, *qtdeTitleIndex);
	ordenarIndiceSecundarioDiretor(idirector, *qtdeDirectorIndex);

	fclose(f);
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
void criarIndiceSecundarioTitulo(ITITLE *ititle, int *qtdeTitleIndex, char titulo[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	int retorno;

	retorno = buscarTituloPortugues(ititle, titulo, 0, *qtdeTitleIndex, primary_index, *qtdePrimaryIndex, PROCURA_IGUAL);

	// Guarda informacoes
	if (retorno != -1)
	{
		// Titulo em portugues nao existente, ou seja, chave secundaria nova. A chave primaria sera guardada em sua propria posicao 
		ititle[retorno].contRepetido = ititle[retorno].contRepetido + 1;
		strcpy(ititle[retorno].primaryCod[ititle[retorno].contRepetido], codigo);		
	}
	else
	{
		// Titulo em portugues ja existente. A chave primaria sera guardada no vetor de chaves primarias correspondente
		// aquele titulo em portugues em questao
		strcpy(ititle[*qtdeTitleIndex].primaryCod[ititle[*qtdeTitleIndex].contRepetido], codigo);
		strcpy(ititle[*qtdeTitleIndex].tituloPortugues, titulo);
		*qtdeTitleIndex = *qtdeTitleIndex + 1;
	}

}

/* Cria o indice secundario por diretor em RAM a partir do arquivo de dados */
void criarIndiceSecundarioDiretor(IDIRECTOR *idirector, int *qtdeDirectorIndex, char diretor[MAX_STRING], char codigo[MAX_COD], PRIMARY_INDEX *primary_index, int *qtdePrimaryIndex)
{
	int retorno;

	retorno = buscarDiretor(idirector, diretor, 0, *qtdeDirectorIndex, primary_index, *qtdePrimaryIndex, PROCURA_IGUAL); 

	// Guarda informacoes
	if (retorno != -1)
	{
		// Diretor nao existente, ou seja, chave secundaria nova. A chave primaria sera guardada em sua propria posicao 
		idirector[retorno].contRepetido = idirector[retorno].contRepetido + 1;
		strcpy(idirector[retorno].primaryCod[idirector[retorno].contRepetido], codigo);
	}
	else
	{
		// Diretor ja existente. A chave primaria sera guardada no vetor de chaves primarias correspondente
		// aquele diretor em questao
		strcpy(idirector[*qtdeDirectorIndex].primaryCod[idirector[*qtdeDirectorIndex].contRepetido], codigo);
		strcpy(idirector[*qtdeDirectorIndex].diretor, diretor);
		*qtdeDirectorIndex = *qtdeDirectorIndex + 1;
	}
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
		printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
		printf ("Titulo original: %s\n", strtok(NULL, "@"));
		printf ("Diretor: %s\n", strtok(NULL, "@"));
		printf ("Genero: %s\n", strtok(NULL, "@"));
		printf ("Duracao: %s\n", strtok(NULL, "@"));
		printf ("Ano estreia: %s\n", strtok(NULL, "@"));
		printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
		printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
		printf ("Numero episodios: %s\n", strtok(NULL, "@"));
		printf ("Pais: %s\n", strtok(NULL, "@"));
		ptrString = strtok(NULL, "@");
		nota = *ptrString - '0';
		printf ("Nota: %d\n", nota);
		printf ("Resumo: %s\n", strtok(NULL, "@"));

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
int buscarTituloPortugues(ITITLE *ititle, char tituloBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice)
{
	FILE *f;

	int i;
	int rrn;
	int meio;
	int nota;
	int imprimiuAlgum;

	char recoverData[1024];
	char *ptrString;

	// Busca binaria pelo vetor de indice de chave secundaria
	// choice indica qual sera o tipo de retorno dessa funcao
	// Em PROCURA_IGUAL retorna TRUE se achar uma serie de mesma chave secundaria
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

	if (!strcmp(ititle[meio].tituloPortugues, tituloBusca))
	{
		if (choice == PROCURA_IGUAL)
		{
			return meio;
		}

		f = fopen("series.dat", "r");

		imprimiuAlgum = FALSE;

		for (i = 0; i <= ititle[meio].contRepetido; i++)
		{
			rrn = buscarChavePrimaria(primary_index, ititle[meio].primaryCod[i], 0, qtdePrimaryIndex, BUSCA_RRN);
			
			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);
			
			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf ("Titulo original: %s\n", strtok(NULL, "@"));
				printf ("Diretor: %s\n", strtok(NULL, "@"));
				printf ("Genero: %s\n", strtok(NULL, "@"));
				printf ("Duracao: %s\n", strtok(NULL, "@"));
				printf ("Ano estreia: %s\n", strtok(NULL, "@"));
				printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf ("Numero episodios: %s\n", strtok(NULL, "@"));
				printf ("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf ("Nota: %d\n", nota);
				printf ("Resumo: %s\n", strtok(NULL, "@"));
			
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
		if (strcmp(ititle[meio].tituloPortugues, tituloBusca) > 0)
		{
			return buscarTituloPortugues(ititle, tituloBusca, esquerda, meio - 1, primary_index, qtdePrimaryIndex, choice);
		}
		else
		{
			return buscarTituloPortugues(ititle, tituloBusca, meio + 1, direita, primary_index, qtdePrimaryIndex, choice);
		}
	}
}

/* Realiza busca binaria com base no titulo em portugues */
int buscarDiretor(IDIRECTOR *idirector, char diretorBusca[MAX_STRING], int esquerda, int direita, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex, int choice)
{
	FILE *f;

	int i;
	int rrn;
	int meio;
	int nota;
	int imprimiuAlgum;

	char recoverData[1024];
	char *ptrString;

	// Busca binaria pelo vetor de indice de chave secundaria
	// choice indica qual sera o tipo de retorno dessa funcao
	// Em PROCURA_IGUAL retorna TRUE se achar uma serie de mesma chave secundaria
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

	if (!strcmp(idirector[meio].diretor, diretorBusca))
	{
		if (choice == PROCURA_IGUAL)
		{
			return meio;
		}

		f = fopen("series.dat", "r");

		imprimiuAlgum = FALSE;

		for (i = 0; i <= idirector[meio].contRepetido; i++)
		{
			rrn = buscarChavePrimaria(primary_index, idirector[meio].primaryCod[i], 0, qtdePrimaryIndex, BUSCA_RRN);
			
			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);

			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{				
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf ("Titulo original: %s\n", strtok(NULL, "@"));
				printf ("Diretor: %s\n", strtok(NULL, "@"));
				printf ("Genero: %s\n", strtok(NULL, "@"));
				printf ("Duracao: %s\n", strtok(NULL, "@"));
				printf ("Ano estreia: %s\n", strtok(NULL, "@"));
				printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf ("Numero episodios: %s\n", strtok(NULL, "@"));
				printf ("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf ("Nota: %d\n", nota);
				printf ("Resumo: %s\n", strtok(NULL, "@"));
			
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
		if (strcmp(idirector[meio].diretor, diretorBusca) > 0)
		{
			return buscarDiretor(idirector, diretorBusca, esquerda, meio - 1, primary_index, qtdePrimaryIndex, choice);
		}
		else
		{
			return buscarDiretor(idirector, diretorBusca, meio + 1, direita, primary_index, qtdePrimaryIndex, choice);
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
void ordenarIndiceSecundarioTitulo(ITITLE *ititle, int qtdeTitleIndex)
{
	int i;
	int j;

	ITITLE aux;

	char aux_chavePrimaria[MAX_COD];

	for (i = 0; i < qtdeTitleIndex; i++)
	{
		for (j = 0; j < qtdeTitleIndex - 1; j++)
		{
			if (strcmp(ititle[j].tituloPortugues, ititle[j + 1].tituloPortugues) > 0)
			{
				aux = ititle[j];
				ititle[j] = ititle[j + 1];
				ititle[j + 1] = aux;
			}
		}
	}

	for (i = 0; i < qtdeTitleIndex; i++)
	{
		for (j = 0; j < ititle[i].contRepetido; j++)
		{
			if (strcmp(ititle[i].primaryCod[j], ititle[i].primaryCod[j + 1]) > 0)
			{
				strcpy(aux_chavePrimaria, ititle[i].primaryCod[j]);
				strcpy(ititle[i].primaryCod[j], ititle[i].primaryCod[j + 1]);
				strcpy(ititle[i].primaryCod[j + 1], aux_chavePrimaria);
			}
		}
	}	
}

/* Ordena lexograficamente em ordem crescente o indice de chaves secundarias por diretor */
void ordenarIndiceSecundarioDiretor(IDIRECTOR *idirector, int qtdeDirectorIndex)
{
	int i;
	int j;

	IDIRECTOR aux;

	char aux_chavePrimaria[MAX_COD];

	for (i = 0; i < qtdeDirectorIndex; i++)
	{
		for (j = 0; j < qtdeDirectorIndex - 1; j++)
		{
			if (strcmp(idirector[j].diretor, idirector[j + 1].diretor) > 0)
			{
				aux = idirector[j];
				idirector[j] = idirector[j + 1];
				idirector[j + 1] = aux;
			}
		}
	}

	for (i = 0; i < qtdeDirectorIndex; i++)
	{
		for (j = 0; j < idirector[i].contRepetido; j++)
		{
			if (strcmp(idirector[i].primaryCod[j], idirector[i].primaryCod[j + 1]) > 0)
			{
				strcpy(aux_chavePrimaria, idirector[i].primaryCod[j]);
				strcpy(idirector[i].primaryCod[j], idirector[i].primaryCod[j + 1]);
				strcpy(idirector[i].primaryCod[j + 1], aux_chavePrimaria);
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
			printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
			printf ("Titulo original: %s\n", strtok(NULL, "@"));
			printf ("Diretor: %s\n", strtok(NULL, "@"));
			printf ("Genero: %s\n", strtok(NULL, "@"));
			printf ("Duracao: %s\n", strtok(NULL, "@"));
			printf ("Ano estreia: %s\n", strtok(NULL, "@"));
			printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
			printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
			printf ("Numero episodios: %s\n", strtok(NULL, "@"));
			printf ("Pais: %s\n", strtok(NULL, "@"));
			ptrString = strtok(NULL, "@");
			nota = *ptrString - '0';
			printf ("Nota: %d\n", nota);
			printf ("Resumo: %s\n\n", strtok(NULL, "@"));		

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
int listarSeriesTitulo(FILE *f, ITITLE *ititle, int qtdeTitleIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
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
		for (j = 0; j <= ititle[i].contRepetido; j++)
		{
			rrn = buscarChavePrimaria(primary_index, ititle[i].primaryCod[j], 0, qtdePrimaryIndex, BUSCA_RRN);

			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);

			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf ("Titulo original: %s\n", strtok(NULL, "@"));
				printf ("Diretor: %s\n", strtok(NULL, "@"));
				printf ("Genero: %s\n", strtok(NULL, "@"));
				printf ("Duracao: %s\n", strtok(NULL, "@"));
				printf ("Ano estreia: %s\n", strtok(NULL, "@"));
				printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf ("Numero episodios: %s\n", strtok(NULL, "@"));
				printf ("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf ("Nota: %d\n", nota);
				printf ("Resumo: %s\n\n", strtok(NULL, "@"));		

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
int listarSeriesDiretor(FILE *f, IDIRECTOR *idirector, int qtdeDirectorIndex, PRIMARY_INDEX *primary_index, int qtdePrimaryIndex)
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
		for (j = 0; j <= idirector[i].contRepetido; j++)
		{
			rrn = buscarChavePrimaria(primary_index, idirector[i].primaryCod[j], 0, qtdePrimaryIndex, BUSCA_RRN);

			fseek(f, rrn, SEEK_SET);
			fgets(recoverData, 1025, f);

			if ((recoverData[0] != '*') && (recoverData[1] != '|'))
			{
				printf ("\nCodigo: %s\n", strtok(recoverData, "@"));
				printf ("Titulo portugues: %s\n", strtok(NULL, "@"));
				printf ("Titulo original: %s\n", strtok(NULL, "@"));
				printf ("Diretor: %s\n", strtok(NULL, "@"));
				printf ("Genero: %s\n", strtok(NULL, "@"));
				printf ("Duracao: %s\n", strtok(NULL, "@"));
				printf ("Ano estreia: %s\n", strtok(NULL, "@"));
				printf ("Ano encerramento: %s\n", strtok(NULL, "@"));
				printf ("Numero temporadas: %s\n", strtok(NULL, "@"));
				printf ("Numero episodios: %s\n", strtok(NULL, "@"));
				printf ("Pais: %s\n", strtok(NULL, "@"));
				ptrString = strtok(NULL, "@");
				nota = *ptrString - '0';
				printf ("Nota: %d\n", nota);
				printf ("Resumo: %s\n\n", strtok(NULL, "@"));		
			
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
