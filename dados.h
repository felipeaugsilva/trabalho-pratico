#include <stdio.h>
#include <stdlib.h>

/**********************************************************************
 *                                                                    *
 * Heuristica para o Problema de Selecao de Segmentos de Faixa (SSSP) *
 *                                                                    *
 * MC548 - Professor Cid C. de Souza                                  *
 *                                                                    *
 * Grupo 09:                                                          *
 * 070234 - Aruana Kaique Ferreira                                    *
 * 096993 - Felipe Augusto da Silva                                   *
 *                                                                    *
 **********************************************************************/



/* Estrutura para armazenar a relacao ganho/custo do shard i j quando
   ele eh fotografado pelo satelite 'sat' */
typedef struct {
    int i, j;
    float ganhoPorCusto;
    char sat;
} shardAux;


/* Estrutura que armzena qual satelite fotografou shard i j */
typedef struct {
    int i, j;
} shard;


/* Estrutura de uma solucao para o problema */
typedef struct {
    shard** shards;
    int obj;
    int numShardsFot;
    int* capacH;
    int* capacV;
} solucao;


/* Funcao para checar se foi possivel alocar memoria */
void checaAlocacaoMemoria( void* ptr );


/* Funcao para alocar um vetor de 'numSats' inteiros */
void alocaVetor( int** vetor, int numSats);


/* Funcao para alocar uma matriz quadrada de ordem 'numSats' de inteiros */
void alocaMatrizInt( int*** matriz, int numSats );


/* Funcao para alocar uma matriz quadrada de ordem 'numSats' de chars */
void alocaMatrizChar( char*** matriz, int numSats );


/* Funcao que le informacoes sobre os satelites no arquivo 'entrada' */
void leSatelites( int* S, int numSats, FILE* entrada );


/* Funcao que inicializa as matrizes usadas */
void inicializaMatrizes( int** ganhos, int** custosH, int** custosV, char** shards, int numSats );


/* Funcao que le as informacoes sobre os shards no arquivo 'entrada' */
void leShards( int** ganhos, int** custosH, int** custosV, int numSats, int numShards, FILE* entrada );


/* Funcao que escreve no arquivo de saida */
void escreveSaida( solucao* sol, int numShards, char** shards, FILE* saida );


/* Funcao para desalocar memoria das estruturas usadas */
void desaloca( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, solucao* sol, int numSats, int numShards );
