// DijkstraOpenMP.c : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <omp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAX_NOS 3500
#define NUM_VERTICES 1000
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

double get_current_time() {
    LARGE_INTEGER time, frequency;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&frequency);
    return (double)time.QuadPart / (double)frequency.QuadPart;
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

int main() {
    srand(time(NULL));

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

    double start_time = get_current_time();

    for (int m = 0; m < NUM_VERTICES; m++)
        dijkstra(grafo, m);

    double end_time = get_current_time();
    printf("\nTempo de execucao: %f segundos\n", end_time - start_time);

    return 0;
}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
