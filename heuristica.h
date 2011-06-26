#include <string.h>
#include <time.h>
#include "dados.h"

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



/* Funcao de comparacao usada pela funcao 'qsort' */
int comparaGanhoPorCusto( const void* a, const void* b );


/* Funcao que constroi uma solucao usando uma heuristica gulosa */
solucao* constroiSolucao( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, int numSats, int numShards );


/* Funcao para tentar melhorar a solucao */
void melhoraSolucao( solucao* sol, int** ganhos, int** custosH, int** custosV, char** shards, int numShards, time_t tempoInicio, int tempoMax );
