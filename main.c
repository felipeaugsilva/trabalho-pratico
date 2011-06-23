#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum boolean { false, true } boolean;

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

void leSatelites( int* S, int numSats, FILE* dados )
{
    int i, indice;
    
    for( i=0; i<numSats; i++)
    {
        fscanf( dados, "%d", &indice );
        fscanf( dados, "%d", &S[indice-1] );
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

void leShards( int** ganhos, int** custosH, int** custosV, int numSats, int numShards, FILE* dados )
{
    int i, j, k;
    int ganho, custoH, custoV;
    
    for( k=0; k<numShards; k++)
    {
        fscanf( dados, "%d %d %d %d %d", &i, &j, &ganho, &custoH, &custoV );
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

int calculaObjetivo( int** ganhos, char** shards, int numSats )
{
    int obj = 0;
    int i, j;
    char sat;
    
    for( i=0; i<numSats; i++)
    {
        for( j=0; j<numSats; j++ )
        {
            sat = shards[i][j];
            if( sat != 'n' )
            {
                obj += ganhos[i][j];
                //printf( "%3d %3d %c\n", i+1, j+1, sat );  // imprimir no arquivo de saida
            }
        }
    }
    
    return obj;
}

int calculaOtimo( int** ganhos, int numSats )
{
    int otimo = 0;
    int i, j;
    
    for( i=0; i<numSats; i++)
        for( j=0; j<numSats; j++ )
            if( ganhos[i][j] > 0 )
                otimo += ganhos[i][j];
    
    return otimo;
}

int main( int argc, char *argv[] )
{
    FILE * dados;
    dados = fopen( "dados.in", "r");
    int numSats;
    int numShards;
    int* Sh;
    int* Sv;
    int** ganhos;
    int** custosH;
    int** custosV;
    char** shards;
    int obj;
    
    fscanf( dados, "%d", &numSats );
    
    alocaVetor( &Sh, numSats );
    alocaVetor( &Sv, numSats );
    
    leSatelites( Sh, numSats, dados);
    leSatelites( Sv, numSats, dados);
   
    fscanf( dados, "%d", &numShards);
    
    alocaMatrizInt( &ganhos, numSats );
    alocaMatrizInt( &custosH, numSats );
    alocaMatrizInt( &custosV, numSats );
    alocaMatrizChar( &shards, numSats );
    
    inicializaMatrizes( ganhos, custosH, custosV, shards, numSats );
    
    leShards( ganhos, custosH, custosV, numSats, numShards, dados );
    
    fclose( dados );
    
    constroiSolucao( Sh, Sv, ganhos, custosH, custosV, shards, numSats, numShards );
    
    obj = calculaObjetivo( ganhos, shards, numSats );
    
    printf( "\nFuncao Objetivo: %d\n\n", obj );
    
    /* Usado apenas pra calcular porcentagem em relacao ao valor "otimo" */
    int otimo = calculaOtimo( ganhos, numSats );
    printf( "Valor Otimo: %d\n\n", otimo );
    printf( "Porcentagem: %f\n\n", ((float) obj / otimo) * 100 );
    
    return 0;
}
