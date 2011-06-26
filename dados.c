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



/* Funcao para checar se foi possivel alocar memoria */
void checaAlocacaoMemoria( void* ptr )
{
    if( ptr == NULL )
    {
        fprintf( stderr, "Erro: nao foi possivel alocar memoria.\n" );
        exit (1);
    }
    
} /* checaAlocacaoMemoria */


/* Funcao para alocar um vetor de 'numSats' inteiros */
void alocaVetor( int** vetor, int numSats)
{
    *vetor = (int*) malloc( numSats * sizeof(int) );
    checaAlocacaoMemoria( *vetor );
    
} /* alocaVetor */


/* Funcao para alocar uma matriz quadrada de ordem 'numSats' de inteiros */
void alocaMatrizInt( int*** matriz, int numSats )
{
    int i;
    
    *matriz = (int**) malloc( numSats * sizeof(int*) );
    checaAlocacaoMemoria( *matriz );
    
    for( i=0; i<numSats; i++)
    {
        (*matriz)[i] = (int*) malloc( numSats * sizeof(int) );
        checaAlocacaoMemoria( (*matriz)[i] );
    }
    
} /* alocaMatrizInt */


/* Funcao para alocar uma matriz quadrada de ordem 'numSats' de chars */
void alocaMatrizChar( char*** matriz, int numSats )
{
    int i;
    
    *matriz = (char**) malloc( numSats * sizeof(char*) );
    checaAlocacaoMemoria( *matriz );
    
    for( i=0; i<numSats; i++)
    {
        (*matriz)[i] = (char*) malloc( numSats * sizeof(char) );
        checaAlocacaoMemoria( (*matriz)[i] );
    }
    
} /* alocaMatrizChar */


/* Funcao que le informacoes sobre os satelites no arquivo 'entrada' */
void leSatelites( int* S, int numSats, FILE* entrada )
{
    int i, indice;
    
    for( i=0; i<numSats; i++)
    {
        fscanf( entrada, "%d", &indice );
        fscanf( entrada, "%d", &S[indice-1] );
    }
    
} /* leSatelites */


/* Funcao que inicializa as matrizes usadas */
void inicializaMatrizes( int** ganhos, int** custosH, int** custosV, char** shards, int numSats )
{
    int i, j;
    
    for( i=0; i<numSats; i++)
    {
        for( j=0; j<numSats; j++)
        {
            ganhos[i][j] = 0;
            custosH[i][j] = custosV[i][j] = -1;
            shards[i][j] = 'n';
        }
    }
    
} /* inicializaMatrizes */


/* Funcao que le as informacoes sobre os shards no arquivo 'entrada' */
void leShards( int** ganhos, int** custosH, int** custosV, int numSats, int numShards, FILE* entrada )
{
    int i, j, k;
    int ganho, custoH, custoV;
    
    for( k=0; k<numShards; k++)
    {
        fscanf( entrada, "%d %d %d %d %d", &i, &j, &ganho, &custoH, &custoV );
        ganhos[i-1][j-1] = ganho;
        custosH[i-1][j-1] = custoH;
        custosV[i-1][j-1] = custoV;
    }
    
} /* leShards */


/* Funcao que escreve no arquivo de saida */
void escreveSaida( solucao* sol, int numShards, FILE* saida )
{
    int k;
    
    /* Funcao objetivo na primeira linha e numero de shards fotografados na segunda */
    fprintf( saida, "%d\n%d\n", sol->obj, sol->numShardsFot );
    
    for( k=0; k<numShards; k++ )
    {
        if( sol->shards[k]->sat != 'n' )
        {
            /* i j c, onde c eh o satelite que fotografou o shard ij */
            fprintf( saida, "%d %d %c\n", sol->shards[k]->i + 1, sol->shards[k]->j + 1, sol->shards[k]->sat );
        }
    }
    
} /* escreveSaida */

/* Funcao para desalocar memoria das estruturas usadas */
void desaloca( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, solucao* sol, int numSats, int numShards )
{
    int i;
    
    free( Sh );
    free( Sv );
    
    Sh = Sv = NULL;
    
    for( i=0; i<numSats; i++ )
    {
        free( ganhos[i] );
        free( custosH[i] );
        free( custosV[i] );
        free( shards[i] );
    }
    
    free( ganhos );
    free( custosH );
    free( custosV );
    free( shards );
    
    ganhos = custosH = custosV = NULL;
    shards = NULL;
    
    for( i=0; i<numShards; i++ )
        free( sol->shards[i] );
    free( sol->shards );
    free( sol->capacH );
    free( sol->capacV );
    free( sol);
    
    sol = NULL;
    
} /* desaloca */
