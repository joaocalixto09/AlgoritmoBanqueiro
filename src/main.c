#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* Estruturas exigidas pelo documento  */
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* Mutex para evitar condições de corrida  */
pthread_mutex_t lock;

void inicializar_dados() {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            // Define uma demanda máxima aleatória (entre 0 e o disponível)
            maximum[i][j] = (available[j] > 0) ? rand() % (available[j] + 1) : 0;
            allocation[i][j] = 0;                  // Começa com nada alocado
            need[i][j] = maximum[i][j];            // No início, need = maximum [cite: 21]
        }
    }
}

// Função que verifica se o estado atual é seguro 
bool is_safe_state() {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS];

    // Inicializa Work = Available e Finish = false
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) work[i] = available[i];
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) finish[i] = false;

    int count = 0;
    while (count < NUMBER_OF_CUSTOMERS) {
        bool found = false;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                int j;
                // Verifica se Need <= Work
                for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) break;
                }

                // Se todos os recursos necessários podem ser atendidos
                if (j == NUMBER_OF_RESOURCES) {
                    for (int k = 0; k < NUMBER_OF_RESOURCES; k++)
                        work[k] += allocation[i][k];
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        // Se nenhum cliente pôde ser atendido nesta rodada, o estado é inseguro
        if (!found) break;
    }

    return (count == NUMBER_OF_CUSTOMERS);
}

int request_resources(int customer_num, int request[]) {
    pthread_mutex_lock(&lock); // Protege contra condição de corrida [cite: 35]

    // 1. Verifica se o pedido é maior que a necessidade ou que o disponível
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i] || request[i] > available[i]) {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }

    // 2. Tenta alocar os recursos temporariamente
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // 3. Verifica se o novo estado é seguro [cite: 26-27]
    if (is_safe_state()) {
        pthread_mutex_unlock(&lock);
        return 0; // Sucesso
    } else {
        // 4. Se for inseguro, desfaz a alocação (Rollback) [cite: 5]
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        pthread_mutex_unlock(&lock);
        return -1; // Negado
    }
}

int release_resources(int customer_num, int release[]) {
    pthread_mutex_lock(&lock); // Protege o acesso aos dados globais [cite: 35]

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }

    printf("Cliente %d liberou recursos.\n", customer_num);
    
    pthread_mutex_unlock(&lock);
    return 0; // Sucesso [cite: 33]
}

void* customer_thread(void* arg) {
    int customer_num = *(int*)arg;
    int request[NUMBER_OF_RESOURCES];
    int release[NUMBER_OF_RESOURCES];

    while (1) {
        // Gera uma solicitação aleatória baseada no que ele ainda precisa (need) [cite: 25]
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            request[i] = (need[customer_num][i] > 0) ? rand() % (need[customer_num][i] + 1) : 0;
        }

        printf("Cliente %d solicitando recursos...\n", customer_num);
        if (request_resources(customer_num, request) == 0) {
            printf("--- Pedido do Cliente %d FOI ACEITO.\n", customer_num);
            
            // Simula o uso dos recursos
            sleep(1); 

            // Libera o que foi solicitado (ou uma parte aleatória)
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                release[i] = rand() % (allocation[customer_num][i] + 1);
            }
            release_resources(customer_num, release);
        } else {
            printf("--- Pedido do Cliente %d FOI NEGADO (Estado Inseguro).\n", customer_num);
        }

        sleep(rand() % 3); // Espera um pouco antes da próxima tentativa
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Erro: Use ./a.out <res1> <res2> <res3>\n");
        return -1;
    }

    // 1. Inicializa o array 'available' com os argumentos da linha de comando [cite: 38-41]
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    // 2. Inicializa o mutex para prevenir condições de corrida [cite: 35]
    pthread_mutex_init(&lock, NULL);
    
    // 3. Inicializa as matrizes maximum, allocation e need
    inicializar_dados();

    // 4. Criação das threads dos clientes 
    pthread_t customers[NUMBER_OF_CUSTOMERS];
    int customer_ids[NUMBER_OF_CUSTOMERS];

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        customer_ids[i] = i;
        if (pthread_create(&customers[i], NULL, customer_thread, &customer_ids[i]) != 0) {
            perror("Falha ao criar thread");
            return -1;
        }
    }

    // 5. Aguarda as threads (como elas estão em loop infinito, o programa rodará até ser parado)
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}