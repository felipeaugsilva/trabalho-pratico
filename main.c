#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "heuristica.h"

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



int main( int argc, char *argv[] )
{
    FILE* entrada;      /* Arquivo de entrada */
    FILE* saida;        /* Arquivo de saida */
    int tempoMax;       /* Tempo maximo de execucao */
    int numSats;        /* Numero total de satelites */
    int numShards;      /* Numero total de shards */
    int* Sh;            /* Capacidade dos satelites do conjunto H */
    int* Sv;            /* Capacidade dos satelites do conjunto V */
    int** ganhos;       /* Matriz de ganhos */
    int** custosH;      /* Matriz de custos dos satelites de H */
    int** custosV;      /* Matriz de custos dos satelites de V */
    char** shards;      /* Matriz que indica qual satelite fotografou o shard */
    solucao* sol;       /* Solucao */
    time_t tempoInicio;
    
    time( &tempoInicio );
    
    /* Verifica parametros de entrada */
    if( argc != 6 )
    {
        fprintf( stderr, "Usage: heur -t <tempo> -o <saida> <entrada>\n" );
        return 1;
    }
    
    tempoMax = atoi( argv[2] );
    
    saida = fopen( argv[4], "w" );
    if( saida == NULL )
    {
        fprintf( stderr, "Erro: nao foi posivel criar arquivo %s\n", argv[4] );
        return 1;
    }
    
    entrada = fopen( argv[5], "r" );
    if( entrada == NULL )
    {
        fprintf( stderr, "Erro: arquivo %s nao encontrado\n", argv[5] );
        return 1;
    }
    
    fscanf( entrada, "%d", &numSats );
    
    alocaVetor( &Sh, numSats );
    alocaVetor( &Sv, numSats );
    
    leSatelites( Sh, numSats, entrada );
    leSatelites( Sv, numSats, entrada );
   
    fscanf( entrada, "%d", &numShards );
    
    alocaMatrizInt( &ganhos, numSats );
    alocaMatrizInt( &custosH, numSats );
    alocaMatrizInt( &custosV, numSats );
    alocaMatrizChar( &shards, numSats );
    
    inicializaMatrizes( ganhos, custosH, custosV, shards, numSats );
    
    leShards( ganhos, custosH, custosV, numSats, numShards, entrada );
    
    fclose( entrada );
    
    sol = constroiSolucao( Sh, Sv, ganhos, custosH, custosV, shards, numSats, numShards );
    
    melhoraSolucao( sol, ganhos, custosH, custosV, shards, numShards, tempoInicio, tempoMax );
    
    escreveSaida( sol, numShards, shards, saida );
    
    fclose( saida );
    
    desaloca( Sh, Sv, ganhos, custosH, custosV, shards, sol, numSats, numShards );
    
    return 0;
    
} /* main */
