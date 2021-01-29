/*
Este arquivo contém uma implementação para a API de creator.h.

Esta implementação gera um número potencialmente infinito de processos aleatórios.

Porém, apenas NUM_PROCESSES (definido em process.h) processes podem existir ao mesmo
tempo. Os PIDs de 1 até NUM_PROCESSES são reciclados.

Esta implementação é interativa.


Padrão de construção dos processos:
    - De 1 até 3 conjuntos de instruções.
        - Cada conjunto é formado inicialmente por 2 até 5 instruções CPU.
        - Depois são acrescentadas as instruções de um dos grupos a seguir, escolhidas aleatóriamente:
            - 1xDISK
            - 2xDISK
            - 1xTAPE
            - 1xPRINTER
            - 2xCPU
            - 3xCPU
    - Um processo pode ter até 20 instruções.
    - Nenhum processo pode terminar com instrução de E/S. Se for o caso, é adicionado uma instrução CPU.
        - Se já houver 20 instruções, a última é substituída por uma instr. CPU.

Exemplos de processos gerados:
    - CC
    - CCDC
    - CCDDC
    - CCTC
    - CCPC
    - CCCC
    - CCCCCTCCCPCCCCCDDC
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "process.h"
#include "scheduler.h"
#include "creator.h"
#include "output.h"

// Controla até que momento novos processos podem ser criados.
// Se for um valor negativo, novos processo serão criados infinitamente.
#define MAX_CPU_TIME -1

// Número máximo de instruções atribuídas a um novo processo.
#define MAX_INSTRUCTIONS 20

// Número de processes que podem ser executados simultaneamente
#define NUM_SIMULTANEOUS NUM_PROCESSES

// Fila que contém todos os PIDs não utilizados. Usada para determinar o PID de um novo processo.
static Queue* unused_ids;

// Array que contém os processos criados e um ponteiro para processes_array[-1].
// O ponteiro permite acesso do índice 1 em frente, já que o PID 0 não é válido.
static Process processes_array[NUM_SIMULTANEOUS];
static Process* processes = processes_array-1;

// Retorna um número aleatório de 0 até n.
static unsigned randint(n){
    return rand()%(n+1);
}

// Usar na expressão de um for para executor o laço um número de vezes aleatório entre [start,end].
#define randrange(start,end) int randrange_i=0,randrange_end=(start)+randint(end-start); randrange_i < randrange_end; randrange_i++

// Adiciona instruções aleatórias a um processo, seguindo as regras definidas no topo deste arquivo.
static void randomize_process(Process* p){
    static const enum Instruction instrs[6] = {DISK, DISK, TAPE, PRINTER, CPU, CPU};
    static const uint8_t          amount[6]  = {1   , 2   , 1   , 1      , 2  , 3  };
    
    for(randrange(1,3)){
        process_add_instructions(p, CPU, 2 + randint(3));
        const int r = randint(5);
        process_add_instructions(p, instrs[r], amount[r]);
    }
    
     // Última instrução deve ser CPU.
    if(p->instructions->array[p->instructions->last] != CPU){
        if (queue_isfull(p->instructions))
            p->instructions->array[p->instructions->last] = CPU; 
        else
            queue_push(p->instructions, CPU);
    }
}

// -----

bool is_interactive = true;

extern int has_incoming_processes(){
    #if (MAX_CPU_TIME >= 0)
        return CPUtime <= MAX_CPU_TIME;
    #else
        return 1;
    #endif
}

extern void create_processes(){
    if (has_incoming_processes()){
        for(randrange(0,2)){
            if (unused_ids->length){
                const int pid = queue_pop(unused_ids);
                processes[pid] = (Process) {.start=CPUtime, .priority=INITIAL_PRIORITY, .id = pid, .state=PSTATE_CREATED, .instructions=queue_create(MAX_INSTRUCTIONS)};
                randomize_process(&processes[pid]);
                process_table[pid] = &processes[pid];
                process_count++;
                
                output_info(&processes[pid]);
                
                schedule_process(&processes[pid]);
            }
            else if (process_count < NUM_PROCESSES){
                for (int i = 0; i < NUM_PROCESSES; i++)
                    if (processes[i].state == PSTATE_TERMINATED)
                        queue_push(unused_ids, i);
            }
        }
    }
}

extern void initialize_processes(){
    srand(time(NULL));
    unused_ids = queue_create(NUM_SIMULTANEOUS);
    for (int i = 1; i <= NUM_SIMULTANEOUS; i++){
        queue_push(unused_ids, i);
    }
}
