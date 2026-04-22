# Algoritmo do Banqueiro - Prevenção de Deadlock

Este projeto é uma simulação do **Algoritmo do Banqueiro**, desenvolvida para a disciplina de Sistemas Operacionais. O programa gerencia a alocação de recursos entre múltiplas threads (clientes) de forma a evitar estados de impasse (deadlock).

## 🚀 Tecnologias Utilizadas
* **Linguagem C**
* **Pthreads**: Para simulação de múltiplos processos concorrentes.
* **Mutex**: Para garantir a exclusão mútua e evitar condições de corrida.

## 📂 Estrutura do Projeto
* `src/main.c`: Código-fonte principal contendo a lógica do banqueiro e das threads.
* `Relatorio.pdf`: Documentação detalhada do projeto.

## 🛠️ Como Executar

O programa aceita a quantidade inicial de recursos via linha de comando. Recomenda-se o uso do ambiente **OnlineGDB** para testes rápidos:

1. Acesse o [OnlineGDB](https://www.onlinegdb.com/online_c_compiler).
2. Cole o conteúdo de `src/main.c`.
3. Clique na seta ao lado do botão **Run** e selecione **Command line arguments**.
4. Digite os recursos iniciais (Exemplo: `10 5 7`).
5. Clique em **Run**.

## 📊 Funcionamento
O sistema cria 5 threads de clientes. Cada cliente solicita e libera recursos aleatoriamente. O "Banqueiro" verifica através de um algoritmo de segurança se a solicitação pode ser atendida sem comprometer a estabilidade do sistema.