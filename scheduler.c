#include "scheduler.h"
#include "devices.h"

Process* currentProcess = NULL;
Queue* ready_queues[NUM_PRIORITIES];
uint8_t timeUsed = 0;

/*
 Esta função irá atribuir à global "currentProcess" o ponteiro do próximo processo
a ser executado, de acordo com a ordem das filas de prioridade.
*/
static void get_next_process(){
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!queue_isempty(ready_queues[i])) {
            currentProcess = process_table[queue_pop(ready_queues[i])];
            timeUsed = 0;
            currentProcess->state = PSTATE_RUNNING;
            return;
        }
    }
    currentProcess = NULL;
}

void scheduler_block(){
    currentProcess->state = PSTATE_BLOCKED;
    get_next_process();
}

void scheduler(){
    if (!currentProcess){
        get_next_process();
    }
    else {
        if (queue_isempty(currentProcess->instructions)){
            currentProcess->state = PSTATE_TERMINATED;
            get_next_process();
        }
        else if (timeUsed >= MAX_TIME_USED){
            queue_push(ready_queues[1], currentProcess->id);
            currentProcess->state = PSTATE_READY;
            get_next_process();
        }
    }
}
