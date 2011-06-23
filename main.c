#include <stdio.h>
#include <stdlib.h>


typedef enum boolean { false, true } boolean;

void alocaVetor( int** vetor, int tamanho)
{
    *vetor = (int*) malloc( tamanho * sizeof(int) );
}

void alocaMatrizInt( int*** matriz, int tamanho )
{
    int i;
    
    *matriz = (int**) malloc( tamanho * sizeof(int*) );
    for( i=0; i<tamanho; i++)
        (*matriz)[i] = (int*) malloc( tamanho * sizeof(int) );
}

void alocaMatrizChar( char*** matriz, int tamanho )
{
    int i;
    
    *matriz = (char**) malloc( tamanho * sizeof(char*) );
    for( i=0; i<tamanho; i++)
        (*matriz)[i] = (char*) malloc( tamanho * sizeof(char) );
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

void inicializaMatrizes( int** ganhos, int** custosH, int** custosV, char** shards, int tamanho )
{
    int i, j;
    
    for( i=0; i<tamanho; i++)
    {
        for( j=0; j<tamanho; j++)
        {
            ganhos[i][j] = 0;
            custosH[i][j] = custosV[i][j] = -1;
            shards[i][j] = 'n';
        }
    }
}

void leShards( int** ganhos, int** custosH, int** custosV, int tamanho, int numShards, FILE* dados )
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
    
    return 0;
}