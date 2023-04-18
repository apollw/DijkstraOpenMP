// DijkstraOpenMP.c : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <omp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOS 3500
#define NUM_VERTICES 1500
#define MIN_PESO 1
#define MAX_PESO 20
#define DIST_MAX INT_MAX

struct No {
    int vertice;
    int peso;
    struct No* prox;
};

struct Grafo {
    struct No* cabeca[NUM_VERTICES];
    int numVertices;
};

struct No* criarNo(int v, int p) {
    struct No* novoNo = (struct No*)malloc(sizeof(struct No));
    novoNo->vertice = v;
    novoNo->peso = p;
    novoNo->prox = NULL;
    return novoNo;
}

struct Grafo* criarGrafo(int vertices) {
    struct Grafo* grafo = (struct Grafo*)malloc(sizeof(struct Grafo));
    grafo->numVertices = vertices;

    for (int i = 0; i < vertices; i++) {
        grafo->cabeca[i] = NULL;
    }

    return grafo;
}

void adicionarAresta(struct Grafo* grafo, int orig, int dest, int peso) {
    struct No* novoNo = criarNo(dest, peso);
    novoNo->prox = grafo->cabeca[orig];
    grafo->cabeca[orig] = novoNo;
}

void imprimirGrafo(struct Grafo* grafo) {
    printf("\nGrafo:\n");
    for (int i = 0; i < grafo->numVertices; i++) {
        struct No* temp = grafo->cabeca[i];
        printf("Vertice %d: ", i);
        while (temp != NULL) {
            printf("(%d,%d) -> ", temp->vertice, temp->peso);
            temp = temp->prox;
        }
        printf("NULL\n");
    }
}

//Abordagem de execução utilizando diretivas OpenMP externas
void dijkstra(struct Grafo* grafo, int inicio) {
    int distancias[NUM_VERTICES];
    bool visitados[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++) {
        distancias[i] = INT_MAX;
        visitados[i] = false;
    }

    distancias[inicio] = 0;

    for (int count = 0; count < NUM_VERTICES - 1; count++) {
        int u = -1;

        for (int i = 0; i < NUM_VERTICES; i++) {
            if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                u = i;
            }
        }

        visitados[u] = true;

        struct No* v = grafo->cabeca[u];

        while (v != NULL) {
            if (!visitados[v->vertice] &&
                distancias[u] + v->peso < distancias[v->vertice]) {
                distancias[v->vertice] = distancias[u] + v->peso;
            }
            v = v->prox;
        }
    }
    /*printf("\nDistancias minimas a partir do vertice %d:\n", inicio);
    for (int i = 0; i < NUM_VERTICES; i++) {
        printf("Vertice %d: %d\n", i, distancias[i]);
    }*/
}

//Abordagem de execução utilizando diretivas OpenMP internas
void dijkstraOpenMP(struct Grafo* grafo, int inicio) {
    int distancias[NUM_VERTICES];
    bool visitados[NUM_VERTICES];

    omp_set_num_threads(8); //Setando internamente à função o número de threads máximo

    //Não há necessidade de paralelizar esse loop
    for (int i = 0; i < NUM_VERTICES; i++) {
        distancias[i] = INT_MAX;
        visitados[i] = false;
    }

    distancias[inicio] = 0;

    int i; //i deve ser inicializado fora do loop, em OpenMP
    for (int count = 0; count < NUM_VERTICES - 1; count++) {
        int u = -1;
        #pragma omp parallel for num_threads(8) //Definindo 8 threads para uso no laço interno
        for (i = 0; i < NUM_VERTICES; i++) {
            /*int id = omp_get_thread_num();
            printf("Thread %d esta executando a iteracao %d do loop\n", id, i);*/
            if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                #pragma omp critical //Aqui uma região crítica
                {
                    if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                        u = i;
                    }
                }
            }
        }

        visitados[u] = true;

        struct No* v = grafo->cabeca[u];

        while (v != NULL) {
            if (!visitados[v->vertice] &&
                distancias[u] + v->peso < distancias[v->vertice]) {
                distancias[v->vertice] = distancias[u] + v->peso;
            }
            v = v->prox;
        }
    }
}

void dijkstraOpenMPGeral(struct Grafo* grafo, int inicio) {
    int distancias[NUM_VERTICES];
    bool visitados[NUM_VERTICES];

    omp_set_num_threads(8); //Setando internamente à função o número de threads máximo

    //Não há necessidade de paralelizar esse loop
    for (int i = 0; i < NUM_VERTICES; i++) {
        distancias[i] = INT_MAX;
        visitados[i] = false;
    }

    distancias[inicio] = 0;

    int count; //count deve ser inicializado fora do loop, em OpenMP
    #pragma omp parallel for num_threads(2) //Definindo 2 threads para uso no laço externo e geral
    for (count = 0; count < NUM_VERTICES - 1; count++) {
        int u = -1;
        for (int i = 0; i < NUM_VERTICES; i++) {
            /*int id = omp_get_thread_num();
            printf("Thread %d esta executando a iteracao %d do loop\n", id, i);*/
            if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
            #pragma omp critical //Aqui uma região crítica
                {
                    if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                        u = i;
                    }
                }
            }
        }

        visitados[u] = true;

        struct No* v = grafo->cabeca[u];

        while (v != NULL) {
            if (!visitados[v->vertice] &&
                distancias[u] + v->peso < distancias[v->vertice]) {
                distancias[v->vertice] = distancias[u] + v->peso;
            }
            v = v->prox;
        }
    }
}

void dijkstraOpenMPCombinado(struct Grafo* grafo, int inicio) {
    int distancias[NUM_VERTICES];
    bool visitados[NUM_VERTICES];

    omp_set_num_threads(8); //Número máximo de threads a serem utilizados

    for (int i = 0; i < NUM_VERTICES; i++) {
        distancias[i] = INT_MAX;
        visitados[i] = false;
    }

    distancias[inicio] = 0;

    int i;

    for (int count = 0; count < NUM_VERTICES - 1; count++) {
        int u = -1;
        #pragma omp parallel for num_threads(4)
        for (i = 0; i < NUM_VERTICES; i++) {
            if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                #pragma omp critical
                {
                    if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                        u = i;
                    }
                }
            }
        }

        visitados[u] = true;

        //Aqui ocorre a tentativa de paralelização combinada, dos dois laços
        struct No* v = grafo->cabeca[u];
        #pragma omp parallel num_threads(4)
        {
            struct No* local_v = v;
            while (local_v != NULL) {
                #pragma omp critical
                {
                    if (!visitados[local_v->vertice] &&
                        distancias[u] + local_v->peso < distancias[local_v->vertice]) {
                        distancias[local_v->vertice] = distancias[u] + local_v->peso;
                    }
                }
                local_v = local_v->prox;
            }
        }
    }
}

void dijkstraOpenMPRestrito(struct Grafo* grafo, int inicio) {
    int distancias[NUM_VERTICES];
    bool visitados[NUM_VERTICES];

    omp_set_num_threads(8); //Número máximo de threads a serem utilizados

    for (int i = 0; i < NUM_VERTICES; i++) {
        distancias[i] = INT_MAX;
        visitados[i] = false;
    }

    distancias[inicio] = 0;

    for (int count = 0; count < NUM_VERTICES - 1; count++) {
        int u = -1;   

        //Não paralelizo esse laço
        for (int i = 0; i < NUM_VERTICES; i++) {
            if (!visitados[i] && (u == -1 || distancias[i] < distancias[u])) {
                u = i;
            }
        }
        visitados[u] = true;

        //Paralelizo apenas o laço while
        struct No* v = grafo->cabeca[u];
        #pragma omp parallel num_threads(2)
        {
            struct No* local_v = v;
            while (local_v != NULL) {
                #pragma omp critical
                {
                    if (!visitados[local_v->vertice] &&
                        distancias[u] + local_v->peso < distancias[local_v->vertice]) {
                        distancias[local_v->vertice] = distancias[u] + local_v->peso;
                    }
                }
                local_v = local_v->prox;
            }
        }
    }
}

//-------------------------------- - Casos de Uso------------------------------
//1 - Cálculo de Dijkstra(todos:todos) com Paralelização Externa
//2 - Cálculo de Dijkstra(todos:todos) com Paralelização Interna no for (dijkstraOpenMP)
//3 - Cálculo de Dijsktra(todos:todos) com Paralelização Interna no for geral (dijkstraOpenMPGeral)
//4 - Cálculo de Dijkstra(todos:todos) com Paralelização Interna no while (dijkstraOpenMPRestrito)
//5 - Cálculo de Dijkstra(todos:todos) com Paralelização Interna em ambos(dijkstraOpenMPCombinado)
//6 - Cálculo de Dijkstra(todos:todos) com Paral.Interna Geral e Externa(dijkstraOpenMPGeral + Par.Externa)


int main(int argc, char* argv[]) { 

    omp_set_num_threads(8); //Setando o número máximo de threads para 8

    struct Grafo* grafo = criarGrafo(NUM_VERTICES);
    int peso = 0;
    int numArestas = 0;
    
    for (int i = 0; i < NUM_VERTICES; i++) {
        for (int j = i + 1; j < NUM_VERTICES; j++) {
            peso++;
            adicionarAresta(grafo, i, j, peso);
            adicionarAresta(grafo, j, i, peso);

            numArestas++;
            if (peso > 20)
                peso = 0;
        }
    }    

    printf("Numero de Vertices = %d\n", NUM_VERTICES);
    printf("Numero de Arestas = %d\n", numArestas);
    //imprimirGrafo(grafo);
    
    /*------------------------------Paralelização Externa----------------------------------------*/

    //int m;
    //printf("\nutilizando openmp - 8 threads\n");
    ////utilizando a diretiva de openmp
    //double start = omp_get_wtime();
    //#pragma omp parallel for num_threads(8)
    //for (m = 0; m < NUM_VERTICES; m++) {
    //    //int id = omp_get_thread_num();
    //    //printf("thread %d esta executando a iteracao %d do loop\n", id, m);
    //    dijkstra(grafo, m);
    //}
    //double end = omp_get_wtime();
    //printf("tempo de execucao = %3.5f seconds\n", end - start);

    /*------------------------------Paralelização dijkstraOpenMP------------------------------------*/

    //int n;
    //printf("\nUtilizando OpenMP - Implementacao Interna - 8 threads\n");
    ////Utilizando a diretiva de OpenMP internamente
    //double start = omp_get_wtime();
    //for (n = 0; n < NUM_VERTICES; n++) {
    //    //int id = omp_get_thread_num();
    //    //printf("Thread %d esta executando a iteracao %d do loop\n", id, n);
    //    dijkstraOpenMP(grafo, n);
    //}
    //double end = omp_get_wtime();
    //printf("Tempo de Execucao = %3.5f seconds\n", end - start);

    /*------------------------------Paralelização dijkstraOpenMPGeral------------------------------------*/

    int n;
    printf("\nUtilizando OpenMP - Implementacao Interna Geral - 2 threads\n");
    //Utilizando a diretiva de OpenMP internamente
    double start = omp_get_wtime();
    for (n = 0; n < NUM_VERTICES; n++) {
        //int id = omp_get_thread_num();
        //printf("Thread %d esta executando a iteracao %d do loop\n", id, n);
        dijkstraOpenMPGeral(grafo, n);
    }
    double end = omp_get_wtime();
    printf("Tempo de Execucao = %3.5f seconds\n", end - start);

    /*------------------------------Paralelização dijkstraOpenMPCombinado------------------------------------*/

    //int n;
    //printf("\nUtilizando OpenMP - Implementacao Interna Combinado - 4 threads\n");
    ////Utilizando a diretiva de OpenMP internamente
    //double start = omp_get_wtime();
    //for (n = 0; n < NUM_VERTICES; n++) {
    //    //int id = omp_get_thread_num();
    //    //printf("Thread %d esta executando a iteracao %d do loop\n", id, n);
    //    dijkstraOpenMPCombinado(grafo, n);
    //}
    //double end = omp_get_wtime();
    //printf("Tempo de Execucao = %3.5f seconds\n", end - start);

    /*------------------------------Paralelização dijkstraOpenMPRestrito------------------------------------*/

    //int n;
    //printf("\nUtilizando OpenMP - Implementacao Interna Restrito - 2 threads\n");
    ////Utilizando a diretiva de OpenMP internamente
    //double start = omp_get_wtime();
    //for (n = 0; n < NUM_VERTICES; n++) {
    //    //int id = omp_get_thread_num();
    //    //printf("Thread %d esta executando a iteracao %d do loop\n", id, n);
    //    dijkstraOpenMPRestrito(grafo, n);
    //}
    //double end = omp_get_wtime();
    //printf("Tempo de Execucao = %3.5f seconds\n", end - start);

    /*-----------------------------Sem Paralelização------------------------------------------*/

    ////Sem utilizar OpenMP
    //printf("\nSem utilizar OpenMP - 1 thread\n");
    //double start = omp_get_wtime();
    //for (int m = 0; m < NUM_VERTICES; m++) {
    //    dijkstra(grafo, m);
    //}
    //double end = omp_get_wtime();
    //printf("Tempo de Execucao = %3.5f seconds\n", end - start);


    return 0;
}