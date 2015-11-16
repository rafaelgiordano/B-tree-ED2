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

	Rotina principal do sistema. Compreende a interacao com o usuario
	por meio de menus.
 
 * ================================================================== */

// Inclusao do header
#include "408093_408298_ED2_T01.h"

int main ()
{
	FILE *f_data;
	FILE *f_primary_index;
	FILE *f_title_index;
	FILE *f_director_index;

	PRIMARY_INDEX *primary_index;
	TITLE_INDEX *title_index;
	DIRECTOR_INDEX *director_index;

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

	title_index = (TITLE_INDEX *)malloc(max_size * sizeof(TITLE_INDEX));

	if (!title_index)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}

	director_index = (DIRECTOR_INDEX *)malloc(max_size * sizeof(DIRECTOR_INDEX));

	if (!director_index)
	{
		printf (FRASE_ERROALOCACAO);
		exit(0);
	}	


	inicializarIndices(&qtdePrimaryIndex, title_index, &qtdeTitleIndex, director_index, &qtdeDirectorIndex, max_size);

	if (arquivoDadosVazio())
	{
		// Arquivo de dados nao existe. Deve ser criado.

		f_data = fopen("series.dat", "a+");
	}
	else
	{
		// Arquivo de dados existe. Deve ser aberto para leitura e escrita no final do arquivo.

		f_data = fopen("series.dat", "a+");

		if ((indicePrimarioVazio()) || (indiceSecundarioTituloVazio()) || (indiceSecundarioDiretorVazio()))
		{
			// Algum indice nao existe. Refazer todos.

			recuperarDados(primary_index, &qtdePrimaryIndex, title_index, &qtdeTitleIndex, director_index, &qtdeDirectorIndex);
		}
		else
		{
			// Todos os indices existem.
			// Verificar se todos estao consistentes.

			f_primary_index = fopen("iprimary.idx", "r");
			f_title_index = fopen("ititle.idx", "r");
			f_director_index = fopen("idirector.idx", "r");

			if ((verificaConsistenciaIndice(f_primary_index)) && (verificaConsistenciaIndice(f_title_index)) && (verificaConsistenciaIndice(f_director_index)))
			{
				// Todos os indices estao consistentes. Carrega-los para a RAM.

				fclose(f_primary_index);
				fclose(f_title_index);
				fclose(f_director_index);

				refazerIndicePrimario(primary_index, &qtdePrimaryIndex);
				refazerIndiceSecundarioTitulo(title_index, &qtdeTitleIndex, primary_index, &qtdePrimaryIndex);
				refazerIndiceSecundarioDiretor(director_index, &qtdeDirectorIndex, primary_index, &qtdePrimaryIndex);
			}
			else
			{
				// Algum dos indices (ou todos) esta inconsistente. Refazer todos.

				fclose(f_primary_index);
				fclose(f_title_index);
				fclose(f_director_index);

				f_primary_index = fopen("iprimary.idx", "w");
				f_title_index = fopen("ititle.idx", "w");
				f_director_index = fopen("idirector.idx", "w");				

				fclose(f_primary_index);
				fclose(f_title_index);
				fclose(f_director_index);

				recuperarDados(primary_index, &qtdePrimaryIndex, title_index, &qtdeTitleIndex, director_index, &qtdeDirectorIndex);
			}
		}
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

			title_index = (TITLE_INDEX *)realloc(title_index, max_size * sizeof(TITLE_INDEX));
			
			if (!title_index)
			{
				printf (FRASE_ERROREALOCACAO);
				exit(0);
			}

			director_index = (DIRECTOR_INDEX *)realloc(director_index, max_size * sizeof(DIRECTOR_INDEX));

			if (!director_index)
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

				if (!inserirSerie(f_data, primary_index, &qtdePrimaryIndex, title_index, &qtdeTitleIndex, director_index, &qtdeDirectorIndex))
				{
					printf ("\nChave primaria ja existente!\n");
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
						scanf("\n%[^\n]", chaveBusca);

						if (!buscarChavePrimaria(primary_index, chaveBusca, 0, qtdePrimaryIndex, BUSCA))
						{
							printf (FRASE_REGISTRO_NAOENCONTRADO);
						}
					}	
					break;

					case 2:
					{
						// Pelo titulo em portugues

						printf ("\n\nBusca pelo titulo em portugues de uma serie inicializada\n");
						printf ("Titulo em portugues da serie: ");
						scanf("\n%[^\n]", tituloBusca);

						if (!buscarTituloPortugues(title_index, tituloBusca, 0, qtdeTitleIndex, primary_index, qtdePrimaryIndex, BUSCA))
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
						scanf("\n%[^\n]", diretorBusca);

						if (!buscarDiretor(director_index, diretorBusca, 0, qtdeDirectorIndex, primary_index, qtdePrimaryIndex, BUSCA))
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

							if (!listarSeriesTitulo(f_data, title_index, qtdeTitleIndex, primary_index, qtdePrimaryIndex))
							{
								printf (FRASE_ARQUIVO_VAZIO);
							}
						}	
						break;
						
						case 3:
						{
							// Pelo sobrenome-nome do diretor (ordem lexografica)

							if (!listarSeriesDiretor(f_data, director_index, qtdeDirectorIndex, primary_index, qtdePrimaryIndex))
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
					liberarEspaco(f_data, primary_index, &qtdePrimaryIndex, title_index, &qtdeTitleIndex, director_index, &qtdeDirectorIndex, max_size);
				}
			}
			break;		

			case 7:
			{
				// Finalizar
				// Atualizar todos os indices no disco.
				// Fechar todos os arquivos.
				// Liberar toda a memoria alocada.
				
				gravarIndicePrimario(primary_index, &qtdePrimaryIndex, FINALIZANDO);
				gravarIndiceSecundarioTitulo(title_index, &qtdeTitleIndex, FINALIZANDO);
				gravarIndiceSecundarioDiretor(director_index, &qtdeDirectorIndex, FINALIZANDO);
				
				fclose(f_data);

				free(primary_index);
				free(title_index);
				free(director_index);
				
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