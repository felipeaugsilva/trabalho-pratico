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



/******    TODO    *******/
// (2) remover funcoes de verificacao


// TODO (2)
// Apenas verificacao ********************************************************************
int calculaTotal( int** ganhos, int numSats )
{
    int total = 0;
    int i, j;
    
    for( i=0; i<numSats; i++)
        for( j=0; j<numSats; j++ )
            if( ganhos[i][j] > 0 )
                total += ganhos[i][j];
    
    return total;
}

void checaViabilidade( int* Sh, int* Sv, int numSats, FILE* saida, int** custosH, int** custosV )
{
    int i, j, k, obj, numShards;
    int* ShUsado;
    int* SvUsado;
    char sat;
    
    ShUsado = (int*) malloc( numSats * sizeof(int) );
    SvUsado = (int*) malloc( numSats * sizeof(int) );
    for( k=0; k<numSats; k++ )
    {
        ShUsado[k] = SvUsado[k] = 0;
    }
    
    fscanf( saida, "%d", &obj );
    fscanf( saida, "%d", &numShards );
    
    for( k=0; k<numShards; k++ )
    {
        fscanf( saida, "%d %d %c", &i, &j, &sat );
        i--;
        j--;
        if( sat == 'h' )
            ShUsado[i] += custosH[i][j];
        else
            SvUsado[j] += custosV[i][j];
    }
    
    for( k=0; k<numSats; k++ ) {
        if( ShUsado[k] > Sh[k] )
            printf("Erro: Sh[%d] = %d ShUsado[%d] = %d\n", k+1, Sh[k], k+1, ShUsado[k]);
        if( SvUsado[k] > Sv[k] )
            printf("Erro: Sv[%d] = %d SvUsado[%d] = %d\n", k+1, Sv[k], k+1, SvUsado[k]);
    }
}
//*******************************************************************************************

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
    
    escreveSaida( sol, numShards, saida );
    
    fclose( saida );
    
    /* Usado apenas pra verificacao ****************************/
    // TODO (2)
    int total = calculaTotal( ganhos, numSats );
    time_t tempoAtual;
    printf( "\nFuncao Objetivo: %d\n", sol->obj );
    printf( "Valor Total:     %d\n", total );
    printf( "Porcentagem:     %f\n", ((float) sol->obj / total) * 100 );
    printf( "Tempo:           %.2f\n\n", difftime( time( &tempoAtual ), tempoInicio ) );
    
    saida = fopen( argv[4], "r" );
    checaViabilidade( Sh, Sv, numSats, saida, custosH, custosV );
    fclose( saida );
    
    /***********************************************************/
    
    desaloca( Sh, Sv, ganhos, custosH, custosV, shards, sol, numSats, numShards );
    
    return 0;
    
} /* main */
