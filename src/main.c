#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* Estruturas exigidas pelo documento  */
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* Mutex para evitar condições de corrida  */
pthread_mutex_t lock;

int main(int argc, char *argv[]) {
    // 1. Verificar se os recursos foram passados na linha de comando [cite: 38-39]
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Erro: Use ./a.out <res1> <res2> <res3>\n");
        return -1;
    }

    // 2. Inicializar o array 'available' [cite: 41]
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    // 3. Inicializar matrizes com zero e o Mutex
    pthread_mutex_init(&lock, NULL);
    
    printf("Banqueiro pronto. Recursos disponíveis: ");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) printf("%d ", available[i]);
    printf("\n");

    // Próximo passo será criar os clientes e a lógica de segurança
    pthread_mutex_destroy(&lock);
    return 0;
}