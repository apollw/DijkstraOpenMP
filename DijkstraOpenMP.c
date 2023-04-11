// DijkstraOpenMP.c : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <omp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOS 3500
#define NUM_VERTICES 500
#define MIN_PESO 1
#define MAX_PESO 20
#define DIST_MAX 100

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

    for (int i = 0; i < NUM_VERTICES; i++) {
        distancias[i] = INT_MAX;
        visitados[i] = false;
    }

    distancias[inicio] = 0;

    for (int count = 0; count < NUM_VERTICES - 1; count++) {
        int u = -1;

#pragma omp parallel num_threads(4)
        {
            int id = omp_get_thread_num();
            int local_u = -1;
            int start = id * NUM_VERTICES / 4;
            int end = (id + 1) * NUM_VERTICES / 4;

            for (int i = start; i < end; i++) {
                if (!visitados[i] && (local_u == -1 || distancias[i] < distancias[local_u])) {
                    local_u = i;
                }
            }

#pragma omp critical
            {
                if (u == -1 || (local_u != -1 && distancias[local_u] < distancias[u])) {
                    u = local_u;
                }
            }
        }

        visitados[u] = true;

#pragma omp parallel num_threads(4)
        {
            int id = omp_get_thread_num();
            int start = id * grafo->numVertices / 4;
            int end = (id + 1) * grafo->numVertices / 4;

            for (int i = start; i < end; i++) {
                struct No* v = grafo->cabeca[i];
                while (v != NULL) {
                    if (!visitados[v->vertice] &&
                        distancias[u] + v->peso < distancias[v->vertice]) {
#pragma omp critical
                            {
                                if (distancias[u] + v->peso < distancias[v->vertice]) {
                                    distancias[v->vertice] = distancias[u] + v->peso;
                                }
                            }
                    }
                    v = v->prox;
                }
            }
        }
    }
    /*
    printf("\nDistancias minimas a partir do vertice %d:\n", inicio);
    for (int i = 0; i < NUM_VERTICES; i++) {
        printf("Vertice %d: %d\n", i, distancias[i]);
    }
    */
}




int main(int argc, char* argv[]) { 

    omp_set_num_threads(8);

    ////int i;
    //double start = omp_get_wtime();
    //#pragma omp parallel for num_threads(2)
    ///*for (i = 0; i < 10; i++) {
    //    int id = omp_get_thread_num();
    //    printf("Thread %d esta executando a iteracao %d do loop\n", id, i);
    //}*/
    //double end = omp_get_wtime();
    //printf("Tempo de Execucao = %3.5f seconds\n", end - start);

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

   /* printf("Numero de Vertices = %d\n", NUM_VERTICES);
    printf("Numero de Arestas = %d\n", numArestas);*/
    //imprimirGrafo(grafo);
    
    //int m;

    //printf("\nutilizando openmp - 4 threads\n");

    ////utilizando a diretiva de openmp
    //double start = omp_get_wtime();
    //#pragma omp parallel for num_threads(4)
    //for (m = 0; m < NUM_VERTICES; m++) {
    //    int id = omp_get_thread_num();
    //    printf("thread %d esta executando a iteracao %d do loop\n", id, m);
    //    dijkstra(grafo, m);
    //}
    //double end = omp_get_wtime();
    //printf("tempo de execucao = %3.5f seconds\n", end - start);

    int n;

    printf("\nUtilizando OpenMP - Implementacao Interna - 4 threads\n");

    //Utilizando a diretiva de OpenMP
    double start = omp_get_wtime();
    for (n = 0; n < NUM_VERTICES; n++) {
        /*int id = omp_get_thread_num();
        printf("Thread %d esta executando a iteracao %d do loop\n", id, n);*/
        dijkstraOpenMP(grafo, n);
    }
    double end = omp_get_wtime();
    printf("Tempo de Execucao = %3.5f seconds\n", end - start);


    //printf("\nSem utilizar OpenMP - 1 thread\n");

    ////Sem utilizar OpenMP
    //start = omp_get_wtime();
    //for (m = 0; m < NUM_VERTICES; m++) {
    //    int id = omp_get_thread_num();
    //    //printf("Thread %d esta executando a iteracao %d do loop\n", id, m);
    //    dijkstra(grafo, m);
    //}
    //end = omp_get_wtime();
    //printf("Tempo de Execucao = %3.5f seconds\n", end - start);

    return 0;
}