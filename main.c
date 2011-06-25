#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct shardAux {
    int i, j;
    float ganhoPorCusto;
    char sat;
} shardAux;

typedef struct shard {
    int i, j;
    char sat;
} shard;

typedef struct solucao {
    shard** shards;
    int obj;
    int numShardsFot;
    int* capacH;
    int* capacV;
} solucao;

void checaAlocacaoMemoria( void* ptr )
{
    if( ptr == NULL )
    {
        fprintf( stderr, "Erro: nao foi possivel alocar memoria.\n" );
        exit (1);
    }
}

void alocaVetor( int** vetor, int numSats)
{
    *vetor = (int*) malloc( numSats * sizeof(int) );
    checaAlocacaoMemoria( *vetor );
}

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
}

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
    shardAux** shardAptr = (shardAux**) a;
    shardAux** shardBptr = (shardAux**) b;
    
    shardAux* shardA = *shardAptr;
    shardAux* shardB = *shardBptr;
    
    if( shardA->ganhoPorCusto > shardB->ganhoPorCusto )
        return -1;
    
    return ( shardA->ganhoPorCusto < shardB->ganhoPorCusto );
}

solucao* constroiSolucao( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, int numSats, int numShards )
{
    solucao* sol;
    shardAux** vetorShards;
    int* capacH;
    int* capacV;
    int i, j, k;
    
    alocaVetor( &capacH, numSats );
    alocaVetor( &capacV, numSats );
    
    memcpy( capacH, Sh, numSats * sizeof(int) );
    memcpy( capacV, Sv, numSats * sizeof(int) );
    
    vetorShards = (shardAux**) malloc( 2 * numShards * sizeof(shardAux*) );
    checaAlocacaoMemoria( vetorShards );
    for( k=0; k<2*numShards; k++ )
    {
        vetorShards[k] = (shardAux*) malloc( sizeof(shardAux) );
        checaAlocacaoMemoria( vetorShards[k] );
    }
    
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
    
    qsort( vetorShards, 2*numShards, sizeof(shardAux*), comparaGanhoPorCusto );
    
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
            else if ( vetorShards[k]->sat == 'v' && custosV[i][j] <= capacV[j] )
            {
                shards[i][j] = 'v';
                capacV[j] -= custosV[i][j];
            }
        }
    }
    
    //free( capacH );
    //free( capacV );
    //capacH = capacV = NULL;
    
    for( k=0; k<2*numShards; k++ )
        free( vetorShards[k] );
    free( vetorShards );
    vetorShards = NULL;
    
    sol = (solucao*) malloc( sizeof(solucao) );
    checaAlocacaoMemoria( sol );
    sol->shards = (shard**) malloc( numShards * sizeof(shard*) );
    checaAlocacaoMemoria( sol->shards );
    for( k=0; k<numShards; k++ )
    {
        sol->shards[k] = (shard*) malloc( sizeof(shard) );
        checaAlocacaoMemoria( sol->shards[k] );
    }
    
    k=0;
    sol->obj = sol->numShardsFot = 0;
    for( i=0; i<numSats; i++ )
    {
        for( j=0; j<numSats; j++ )
        {
            if( ganhos[i][j] != 0 )
            {
                sol->shards[k]->i = i;
                sol->shards[k]->j = j;
                sol->shards[k]->sat = shards[i][j];
                k++;
                
                if( shards[i][j] != 'n' )
                {
                    sol->obj += ganhos[i][j];
                    sol->numShardsFot++;
                }
            }
        }
    }
    sol->capacH = capacH;
    sol->capacV = capacV;
    
    return sol;
}

void escreveSaida( solucao* sol, int numShards, FILE* saida )
{
    int k;
    
    fprintf( saida, "%d\n%d\n", sol->obj, sol->numShardsFot );
    
    for( k=0; k<numShards; k++ )
    {
        if( sol->shards[k]->sat != 'n' )
        {
            fprintf( saida, "%d %d %c\n", sol->shards[k]->i + 1, sol->shards[k]->j + 1, sol->shards[k]->sat );
        }
    }
}

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
    free( sol);
    
    sol = NULL;
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
    solucao* sol;
    
    if( argc != 6 )
    {
        fprintf( stderr, "Usage: heur -t <tempo> -o <saida> <entrada>\n" );
        return 1;
    }
    
    tempo = atoi( argv[2] );
    
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
    
    escreveSaida( sol, numShards, saida );
    
    fclose( saida );
    
    /* Usado apenas pra verificacao ****************************/
    
    int total = calculaTotal( ganhos, numSats );
    printf( "\nFuncao Objetivo: %d\n", sol->obj );
    printf( "Valor Total:     %d\n", total );
    printf( "Porcentagem:     %f\n\n", ((float) sol->obj / total) * 100 );
    
    saida = fopen( argv[4], "r" );
    checaViabilidade( Sh, Sv, numSats, saida, custosH, custosV );
    fclose( saida );
    
    /***********************************************************/
    
    desaloca( Sh, Sv, ganhos, custosH, custosV, shards, sol, numSats, numShards );
    
    return 0;
}
