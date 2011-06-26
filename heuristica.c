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
