/* ================================================================== *
	
	Arvore-B - Status
	- insercao em qualquer ordem (testado com ordem 10) funciona bem, a busca de todas as chaves eh feita corretamente.
	- split, ordem 3 e cada chave crescente, inseriu corretamente. Porem, na hora da busca, o ponteiro 'iprimary' ainda aponta para a antiga raiz da arvore.

	Foi mudado
	- nome das variaveis dos indices (iprimary, ititle e idirector)
	- insercao (sem splitting) esta funcionando
	- busca esta funcionando (em nos folhas, sendo a altura da arvore maior que 1, ainda nao testado por split nao funcionar)
	- impressao durante o caminho da busca padronizado de acordo com o pdf

	Falta mudar
	- nos pseudocodigos disponibilizados no Moodle, os i-esimos codigos, filhos sao sempre verificados com i inicando em 1. Dessa forma,
	por exemplo, na primeira insercao, o primeiro codigo eh inserido no	no raiz na posicao 1, sendo que a primeira posicao eh zero.

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

	Rotina principal do sistema. Compreende a interacao com o usuario
	por meio de menus.
 
 * ================================================================== */

// Inclusao do header
#include "408093_408298_ED2_T02.h"

// Variavel global que representa a ordem da arvore-B desejada pelo usuario
int ordem;

int main ()
{
	FILE *f_data;

	Btree *iprimary;
	Btree *aux;

	Node *noBusca;

	PRIMARY_INDEX *primary_index;
	ITITLE *ititle;
	IDIRECTOR *idirector;

	int opcao;
	int qtdePrimaryIndex;
	int qtdeTitleIndex;
	int qtdeDirectorIndex;
	int max_size;

	char chaveBusca[MAX_COD];
	char tituloBusca[MAX_STRING];
	char diretorBusca[MAX_STRING];

	// Definicao do tamanho maximo inicial dos vetores de indices.
	max_size = 10000;

	// Alocacao de memoria necessaria para os vetores de indices. 
	// Caso haja algum erro, o programa eh finalizado.

	primary_index = (PRIMARY_INDEX *)malloc(max_size * sizeof(PRIMARY_INDEX));

	if (!primary_index)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}

	ititle = (ITITLE *)malloc(max_size * sizeof(ITITLE));

	if (!ititle)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}

	idirector = (IDIRECTOR *)malloc(max_size * sizeof(IDIRECTOR));

	if (!idirector)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}	

	inicializarIndices(&qtdePrimaryIndex, ititle, &qtdeTitleIndex, idirector, &qtdeDirectorIndex, max_size);

	// Recebe a ordem que a arvore-B deve ter, somente aceitando valores maiores ou iguais a 1. 
	do
	{
		printf ("Ordem da arvore-B: ");
		scanf ("%d", &ordem);
	}while (ordem < 3);

	// Inicializa a arvore-B
	iprimary = inicializaBtree(iprimary);	

	if (arquivoDadosVazio())
	{
		// Arquivo de dados nao existe. Deve ser criado.

		f_data = fopen("series.dat", "a+");
	}
	else
	{
		// Arquivo de dados existe. Deve ser aberto para leitura e escrita no final do arquivo.

		f_data = fopen("series.dat", "a+");

		// Necessario refazer os indices em RAM a partir do arquivo de dados.
	}

	printf ("\n\n\n");
	printf ("Sistema de cadastro de series de TV inicializado.\n");

	// Laco que mantem o programa em execucao enquanto a opcao Finalizar nao for escolhida.
	do
	{
		printf ("\n\nMENU DE OPCOES\n");
		printf ("1. Cadastro\n");
		printf ("2. Alteracao\n");
		printf ("3. Remocao\n");
		printf ("4. Busca\n");
		printf ("5. Listagem\n");
		printf ("6. Liberar espaco\n");
		printf ("7. Finalizar\n");
		scanf ("%d", &opcao);

		if ((qtdeTitleIndex >= ((3 * max_size) / 4)) && (opcao != 7))
		{
			// Se a quantidade de indices mantidos em RAM chegar a 3/4 do limite estipulado
			// Eh realizada uma realocacao, aumentando o limite em 5000
			// Isso so eh feito se a opcao for diferente da opcao Finalizar

			max_size = max_size + 5000;
			
			primary_index = (PRIMARY_INDEX *)realloc(primary_index, max_size * sizeof(PRIMARY_INDEX));
			
			if (!primary_index)
			{
				printf (FRASE_ERROREALOCACAO);
				exit(0);
			}

			ititle = (ITITLE *)realloc(ititle, max_size * sizeof(ITITLE));
			
			if (!ititle)
			{
				printf (FRASE_ERROREALOCACAO);
				exit(0);
			}

			idirector = (IDIRECTOR *)realloc(idirector, max_size * sizeof(IDIRECTOR));

			if (!idirector)
			{
				printf (FRASE_ERROREALOCACAO);
				exit(0);
			}
		}

		switch (opcao)
		{
			case 1:
			{
				// Inserir nova serie

				aux = inserirSerie(f_data, primary_index, iprimary, &qtdePrimaryIndex, ititle, &qtdeTitleIndex, idirector, &qtdeDirectorIndex);

				if (!aux)
				{
					printf (FRASE_REGISTRO_JACADASTRADO);
				}
				else
				{
					// Faz com que a raiz da arvore aponte para a nova raiz, caso ela tenha mudado por conta de overflow.
					iprimary = aux;
				}
			}
			break;

			case 2:
			{
				// Modificar o campo nota de uma serie a partir de sua chave primaria

				if (!alterarSerie(f_data, primary_index, qtdePrimaryIndex))
				{
					printf (FRASE_REGISTRO_NAOENCONTRADO);
				}		
				else
				{
					fopen("series.dat", "a+");
				}
			}
			break;

			case 3:
			{
				// Remover uma serie a partir de sua chave primaria

				if (!removerSerie(f_data, primary_index, qtdePrimaryIndex))
				{
					printf (FRASE_REGISTRO_NAOENCONTRADO);
				}
			}
			break;

			case 4:
			{
				// Buscar series

				printf ("\n\nMENU DE OPCOES DE BUSCA\n");
				printf ("1. Por codigo\n");
				printf ("2. Por titulo\n");
				printf ("3. Por diretor\n");
				scanf ("%d", &opcao);

				switch (opcao)
				{
					case 1:
					{
						// Pela chave primaria

						printf ("\n\nBusca pelo codigo de uma serie inicializada\n");
						printf ("Chave primaria da serie: ");
						scanf ("\n%[^\n]", chaveBusca);

						printf ("\nNos percorridos:\n");

						noBusca = buscaChave(iprimary, chaveBusca, BUSCA);

						if (!noBusca)
						{
							printf (FRASE_REGISTRO_NAOENCONTRADO);
						}
						else
						{
							buscaBinariaNo(noBusca->codigo, chaveBusca, noBusca->RRN, 0, noBusca->numKeys - 1, BUSCA);
						}
					}	
					break;

					case 2:
					{
						// Pelo titulo em portugues

						printf ("\n\nBusca pelo titulo em portugues de uma serie inicializada\n");
						printf ("Titulo em portugues da serie: ");
						scanf ("\n%[^\n]", tituloBusca);

						if (!buscarTituloPortugues(ititle, tituloBusca, 0, qtdeTitleIndex, primary_index, qtdePrimaryIndex, BUSCA))
						{
							printf (FRASE_REGISTRO_NAOENCONTRADO);
						}
					}	
					break;
					
					case 3:
					{
						// Pelo sobrenome-nome do diretor
					
						printf ("\n\nBusca pelo sobrenome-nome do diretor de uma serie inicializada\n");
						printf ("Sobrenome-nome do diretor da serie: ");
						scanf ("\n%[^\n]", diretorBusca);

						if (!buscarDiretor(idirector, diretorBusca, 0, qtdeDirectorIndex, primary_index, qtdePrimaryIndex, BUSCA))
						{
							printf (FRASE_REGISTRO_NAOENCONTRADO);
						}
					}	
					break;
					
					default:
					{
						printf (FRASE_OPCAO_INEXISTENTE);
					}	
				}
			}
			break;

			case 5:
			{
				// Listar todas as series da base de dados 

				printf ("\n\nMENU DE OPCOES DE LISTAGEM\n");
				printf ("1. Por codigo\n");
				printf ("2. Por titulo\n");
				printf ("3. Por diretor\n");
				scanf ("%d", &opcao);

				if (arquivoDadosVazio())
				{
					printf (FRASE_ARQUIVO_VAZIO);
				}
				else
				{
					switch (opcao)
					{
						case 1:
						{
							// Pelo codigo (ordem lexografica)

							if (!listarSeriesCodigo(f_data, primary_index, qtdePrimaryIndex))
							{
								printf (FRASE_ARQUIVO_VAZIO);
							}
						}	
						break;

						case 2:
						{
							// Titulo em portugues (ordem lexografica)

							if (!listarSeriesTitulo(f_data, ititle, qtdeTitleIndex, primary_index, qtdePrimaryIndex))
							{
								printf (FRASE_ARQUIVO_VAZIO);
							}
						}	
						break;
						
						case 3:
						{
							// Pelo sobrenome-nome do diretor (ordem lexografica)

							if (!listarSeriesDiretor(f_data, idirector, qtdeDirectorIndex, primary_index, qtdePrimaryIndex))
							{
								printf (FRASE_ARQUIVO_VAZIO);
							}
						}	
						break;
						
						default:
						{
							printf (FRASE_OPCAO_INEXISTENTE);
						}
					}
				}

			}
			break;

			case 6:
			{
				// Liberar espaco
				// Arquivo de dados devera ser reorganizado com a remocao fisica de todos 
				// os registros marcados como excluidos e os indices deverao ser atualizados
				// em RAM e gravados em disco.

				if (!arquivoDadosVazio())
				{
					liberarEspaco(f_data, primary_index, &qtdePrimaryIndex, ititle, &qtdeTitleIndex, idirector, &qtdeDirectorIndex, max_size);
				}
			}
			break;		

			case 7:
			{
				// Finalizar
				// Fechar todos os arquivos.
				// Liberar toda a memoria alocada.
				
				fclose(f_data);

				free(primary_index);
				free(ititle);
				free(idirector);
				
				printf ("\n\n\n");
				printf ("Sistema de cadastro de series de TV finalizado.\n");
			}
			break;

			default:
			{
				// Opcao informada nao existe.

				printf (FRASE_OPCAO_INEXISTENTE);
			}	
		}
	
	}while (opcao != 7);

	return 0;
}