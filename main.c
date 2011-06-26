#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/******    TODO    *******/
// (2) remover funcoes de verificacao
// (3) separar em varios arquivos
// (4) funcao escreve saida (script verificacao)


/* Estrutura para armazenar a relacao ganho/custo do shard i j quando
   ele eh fotografado pelo satelite 'sat' */
typedef struct shardAux {
    int i, j;
    float ganhoPorCusto;
    char sat;
} shardAux;


/* Estrutura que armzena qual satelite fotografou shard i j */
typedef struct shard {
    int i, j;
    char sat;
} shard;


/* Estrutura de uma solucao para o problema */
typedef struct solucao {
    shard** shards;
    int obj;
    int numShardsFot;
    int* capacH;
    int* capacV;
} solucao;


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


/* Funcao de comparacao usada pela funcao 'qsort' */
int comparaGanhoPorCusto( const void* a, const void* b )
{
    shardAux** shardAptr = (shardAux**) a;
    shardAux** shardBptr = (shardAux**) b;
    
    shardAux* shardA = *shardAptr;
    shardAux* shardB = *shardBptr;
    
    /* se o ganho/custo de A for maior que o de B, retorna -1 para ordenar em ordem decrescente */
    if( shardA->ganhoPorCusto > shardB->ganhoPorCusto )
        return -1;
    
    return ( shardA->ganhoPorCusto < shardB->ganhoPorCusto );
    
} /* comparaGanhoPorCusto */


/* Funcao que constroi uma solucao usando uma heuristica gulosa */
solucao* constroiSolucao( int* Sh, int* Sv, int** ganhos, int** custosH, int** custosV, char** shards, int numSats, int numShards )
{
    solucao* sol;                    /* solucao que sera gerada */
    shardAux** vetorGanhoPorCusto;   /* vetor com 2 elementos para cada shard: um com o ganho/custo se for fotografado por 'h' e outro se for fotografado por 'v' */
    int* capacH;                     /* capacidade restantes nos satelites do cjt H */
    int* capacV;                     /* capacidade restantes nos satelites do cjt V */
    int i, j, k;
    
    alocaVetor( &capacH, numSats );
    alocaVetor( &capacV, numSats );
    
    // capacidade original eh igual a capacidade total
    memcpy( capacH, Sh, numSats * sizeof(int) );
    memcpy( capacV, Sv, numSats * sizeof(int) );
    
    vetorGanhoPorCusto = (shardAux**) malloc( 2 * numShards * sizeof(shardAux*) );
    checaAlocacaoMemoria( vetorGanhoPorCusto );
    for( k=0; k<2*numShards; k++ )
    {
        vetorGanhoPorCusto[k] = (shardAux*) malloc( sizeof(shardAux) );
        checaAlocacaoMemoria( vetorGanhoPorCusto[k] );
    }
    
    k = 0;
    for( i=0; i<numSats; i++ )
    {
        for( j=0; j<numSats; j++ )
        {
            /* Se existe um shard no segmento i j */
            if( ganhos[i][j] != 0 )
            {
                /* ganho/custo de fotografa-lo com 'h' */
                vetorGanhoPorCusto[k]->i = i;
                vetorGanhoPorCusto[k]->j = j;
                vetorGanhoPorCusto[k]->sat = 'h';
                vetorGanhoPorCusto[k]->ganhoPorCusto = (float) ganhos[i][j] / custosH [i][j];
                k++;
                
                /* ganho/custo de fotografa-lo com 'v' */
                vetorGanhoPorCusto[k]->i = i;
                vetorGanhoPorCusto[k]->j = j;
                vetorGanhoPorCusto[k]->sat = 'v';
                vetorGanhoPorCusto[k]->ganhoPorCusto = (float) ganhos[i][j] / custosV [i][j];
                k++;
            }
        }
    }
    
    /* Ordena decrescentemente baseado no ganho/custo */
    qsort( vetorGanhoPorCusto, 2*numShards, sizeof(shardAux*), comparaGanhoPorCusto );
    
    /* Escolha gulosa: pega os shards com maiores ganho/custo */
    for( k=0; k<2*numShards; k++ )
    {
        i = vetorGanhoPorCusto[k]->i;
        j = vetorGanhoPorCusto[k]->j;
        
        /* Se shard i j nao foi fotografado */
        if( shards[i][j] == 'n' )
        {
            /* ganho/custo sendo analisado eh de 'h' e nao estoura capacidade */
            if( vetorGanhoPorCusto[k]->sat == 'h' && custosH[i][j] <= capacH[i] )
            {
                shards[i][j] = 'h';
                capacH[i] -= custosH[i][j];
            }
            /* ganho/custo sendo analisado eh de 'v' e nao estoura capacidade */
            else if ( vetorGanhoPorCusto[k]->sat == 'v' && custosV[i][j] <= capacV[j] )
            {
                shards[i][j] = 'v';
                capacV[j] -= custosV[i][j];
            }
        }
    }
    
    /* Desaloca vetorGanhoPorCusto */
    for( k=0; k<2*numShards; k++ )
        free( vetorGanhoPorCusto[k] );
    free( vetorGanhoPorCusto );
    vetorGanhoPorCusto = NULL;
    
    /* Aloca solucao */
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
            /* O vetor 'shards' de solucao contem todos os shards, mesmo os nao fotografados */
            if( ganhos[i][j] != 0 )
            {
                sol->shards[k]->i = i;
                sol->shards[k]->j = j;
                sol->shards[k]->sat = shards[i][j];
                k++;
                
                /* Se shard ij foi fotografado, soma seu ganho e incrementa o numero de shards fotografados */
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
    
} /* constroiSolucao */


/* Funcao que escreve no arquivo de saida */
void escreveSaida( solucao* sol, int numShards, FILE* saida )
//void escreveSaida( solucao* sol, int numShards, FILE* saida, int** custosH, int** custosV )
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
            //fprintf( saida, "%d %d %d %d %c\n", sol->shards[k]->i + 1, sol->shards[k]->j + 1, custosH[sol->shards[k]->i][sol->shards[k]->j], custosV[sol->shards[k]->i][sol->shards[k]->j], sol->shards[k]->sat );
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


/* Funcao para tentar melhorar a solucao */
void melhoraSolucao( solucao* sol, int** ganhos, int** custosH, int** custosV, char** shards, int numShards, time_t tempoInicio, int tempoMax )
{
    int i, j;
    shard* s;                   // shard candidato a entrar na solucao
    shard* c;                   // shard candidato a sair da solucao
    int* capacH = sol->capacH;  // capacidade restante nos satelites do conjunto H
    int* capacV = sol->capacV;  // capacidade restante nos satelites do conjunto V
    int objAnterior = sol->obj; // variavel usada para verificar se valor da funcao objetivo mudou
    int cont = 0;               // contador do numero de iteracoes sem mudanca na funcao objetivo
    time_t tempoAtual;
    
    i = 0;
    
    /* Fica em loop enquanto nao estourar o tempo e enquanto a funcao objetivo estiver sendo melhorada */
    /* Se a funcao objetivo nao for alterada por 'numShards' iteracoes, termina o loop */
    while( difftime( time( &tempoAtual ), tempoInicio ) < (double) tempoMax && cont < numShards )
    {
        s = sol->shards[i];
        
        if( s->sat == 'n' )
        {
            /* Eh possivel fotografar com um satelite de H sem estourar a capacidade */
            if( custosH[s->i][s->j] <= capacH[s->i] )
            {
                s->sat = shards[s->i][s->j] = 'h';
                sol->obj += ganhos[s->i][s->j];
                sol->numShardsFot++;
                capacH[s->i] -= custosH[s->i][s->j];
            }
            /* Eh possivel fotografar com um satelite de V sem estourar a capacidade */
            else if( custosV[s->i][s->j] <= capacV[s->j] )
            {
                s->sat = shards[s->i][s->j] = 'v';
                sol->obj += ganhos[s->i][s->j];
                sol->numShardsFot++;
                capacV[s->j] -= custosV[s->i][s->j];
            }
            /* Tenta trocar por outro shard ja na solucao */
            else
            {
                for( j=0; j<numShards; j++ )
                {
                    c = sol->shards[j];
                    
                    if( c->sat != 'n' )
                    {
                        /* Troca se for do mesmo satelite do conjunto H, se melhorar a solucao e se nao estourar a capacidade */
                        if( s->i == c->i &&
                            c->sat == 'h' &&
                            capacH[s->i] + custosH[c->i][c->j] - custosH[s->i][s->j] >= 0 &&
                            sol->obj - ganhos[c->i][c->j] + ganhos[s->i][s->j] > sol->obj )
                        {
                            c->sat = shards[c->i][c->j] = 'n';
                            s->sat = shards[s->i][s->j] = 'h';
                            sol->obj = sol->obj - ganhos[c->i][c->j] + ganhos[s->i][s->j];
                            capacH[s->i] = capacH[s->i] + custosH[c->i][c->j] - custosH[s->i][s->j];
                            
                            break;
                        }
                        /* Troca se for do mesmo satelite do conjunto V, se melhorar a solucao e se nao estourar a capacidade */
                        if( s->j == c->j &&
                            c->sat == 'v' &&
                            capacV[s->j] + custosV[c->i][c->j] - custosV[s->i][s->j] >= 0 &&
                            sol->obj - ganhos[c->i][c->j] + ganhos[s->i][s->j] > sol->obj )
                        {
                            c->sat = shards[c->i][c->j] = 'n';
                            s->sat = shards[s->i][s->j] = 'v';
                            sol->obj = sol->obj - ganhos[c->i][c->j] + ganhos[s->i][s->j];
                            capacV[s->j] = capacV[s->j] + custosV[c->i][c->j] - custosV[s->i][s->j];
                            
                            break;
                        }
                    }
                }
            }
        }
        i = (i+1) % numShards;
        if( sol-> obj == objAnterior )
            cont++;
        else
        {
            objAnterior = sol->obj;
            cont = 0;
        }
    }
    
} /* melhoraSolucao */

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
    //escreveSaida( sol, numShards, saida, custosH, custosV );
    
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
