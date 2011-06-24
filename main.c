#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct shard {
    int i, j;
    float ganhoPorCusto;
    char sat;
} shard;


void alocaVetor( int** vetor, int numSats)
{
    *vetor = (int*) malloc( numSats * sizeof(int) );
}

void alocaMatrizInt( int*** matriz, int numSats )
{
    int i;
    
    *matriz = (int**) malloc( numSats * sizeof(int*) );
    for( i=0; i<numSats; i++)
        (*matriz)[i] = (int*) malloc( numSats * sizeof(int) );
}

void alocaMatrizChar( char*** matriz, int numSats )
{
    int i;
    
    *matriz = (char**) malloc( numSats * sizeof(char*) );
    for( i=0; i<numSats; i++)
        (*matriz)[i] = (char*) malloc( numSats * sizeof(char) );
}

void leSatelites( int* S, int numSats, FILE* entrada )
{
    int i, indice;
    
    for( i=0; i<numSats; i++)
    {
        fscanf( entrada, "%d", &indice );
        fscanf( entrada, "%d", &S[indice-1] );
    }
}

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
}

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
}

int comparaGanhoPorCusto( const void* a, const void* b )
{
    shard** shardAptr = (shard**) a;
    shard** shardBptr = (shard**) b;
    
    shard* shardA = *shardAptr;
    shard* shardB = *shardBptr;
    
    if( shardA->ganhoPorCusto > shardB->ganhoPorCusto )
        return -1;
    
    return ( shardA->ganhoPorCusto < shardB->ganhoPorCusto );
}

void constroiSolucao( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, int numSats, int numShards )
{
    shard** vetorShards;
    int* capacH;
    int* capacV;
    int i, j, k;
    
    alocaVetor( &capacH, numSats );
    alocaVetor( &capacV, numSats );
    
    memcpy( capacH, Sh, numSats * sizeof(int) );
    memcpy( capacV, Sv, numSats * sizeof(int) );
    
    vetorShards = (shard**) malloc( 2 * numShards * sizeof(shard*) );
    for( k=0; k<2*numShards; k++ )
        vetorShards[k] = (shard*) malloc( sizeof(shard) );
    
    k = 0;
    for( i=0; i<numSats; i++ )
    {
        for( j=0; j<numSats; j++ )
        {
            if( ganhos[i][j] != 0 )
            {
                vetorShards[k]->i = i;
                vetorShards[k]->j = j;
                vetorShards[k]->sat = 'h';
                vetorShards[k]->ganhoPorCusto = (float) ganhos[i][j] / custosH [i][j];
                k++;
                
                vetorShards[k]->i = i;
                vetorShards[k]->j = j;
                vetorShards[k]->sat = 'v';
                vetorShards[k]->ganhoPorCusto = (float) ganhos[i][j] / custosV [i][j];
                k++;
            }
        }
    }
    
    qsort( vetorShards, 2*numShards, sizeof(shard*), comparaGanhoPorCusto );
    
    for( k=0; k<2*numShards; k++ )
    {
        i = vetorShards[k]->i;
        j = vetorShards[k]->j;
        
        if( shards[i][j] == 'n' )
        {
            if( vetorShards[k]->sat == 'h' && custosH[i][j] <= capacH[i] )
            {
                shards[i][j] = 'h';
                capacH[i] -= custosH[i][j];
            }
            else if ( custosV[i][j] <= capacV[i] )
            {
                shards[i][j] = 'v';
                capacV[i] -= custosV[i][j];
            }
        }
    }
    
    free( capacH );
    free( capacV );
}

void calculaObjetivo( int* obj, int* numShardsFot, int** ganhos, char** shards, int numSats )
{
    int i, j;
    
    *obj = *numShardsFot = 0;
    
    for( i=0; i<numSats; i++)
    {
        for( j=0; j<numSats; j++ )
        {
            if( shards[i][j] != 'n' )
            {
                *obj += ganhos[i][j];
                (*numShardsFot)++;
            }
        }
    }
}

void escreveSaida( int obj, int numShardsFot, char** shards, int numSats, FILE* saida )
{
    int i, j;
    
    fprintf( saida, "%d\n%d\n", obj, numShardsFot );
    
    for( i=0; i<numSats; i++)
    {
        for( j=0; j<numSats; j++ )
        {
            if( shards[i][j] != 'n' )
            {
                fprintf( saida, "%d %d %c\n", i+1, j+1, shards[i][j] );
            }
        }
    }
}

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

void desaloca( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, int numSats )
{
    int i;
    
    free( Sh );
    free( Sv );
    
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
    
    Sh = Sv = NULL;
    ganhos = custosH = custosV = NULL;
    shards = NULL;
}

int main( int argc, char *argv[] )
{
    FILE* entrada;
    FILE* saida;
    int tempo;
    int numSats;
    int numShards;
    int* Sh;
    int* Sv;
    int** ganhos;
    int** custosH;
    int** custosV;
    char** shards;
    int obj;
    int numShardsFot;
    
    if( argc != 6 )
    {
        fprintf( stderr, "Usage: heur -t <tempo> -o <saida> <entrada>\n" );
        return 1;
    }
    
    tempo = atoi( argv[2] );
    
    saida = fopen( argv[4], "w+" );
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
    
    constroiSolucao( Sh, Sv, ganhos, custosH, custosV, shards, numSats, numShards );
    
    calculaObjetivo( &obj, &numShardsFot, ganhos, shards, numSats );
    
    escreveSaida( obj, numShardsFot, shards, numSats, saida );
    
    fclose( saida );
    
    /* Usado apenas pra verificacao ****************************/
    int total = calculaTotal( ganhos, numSats );
    printf( "\nFuncao Objetivo: %d\n", obj );
    printf( "Valor Total:     %d\n", total );
    printf( "Porcentagem:     %f\n\n", ((float) obj / total) * 100 );
    /***********************************************************/
    
    desaloca( Sh, Sv, ganhos, custosH, custosV, shards, numSats );
    
    return 0;
}
