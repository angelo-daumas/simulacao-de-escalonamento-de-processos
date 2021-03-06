#include <stdio.h>

#include "scheduler.h"
#include "output.h"

// Prioridade atribuída a processos que foram preemptidos por consumirem toda sua fatia de tempo.
#define PREEMPTED_PROCESS_PRIORITY 1

// Quantum máximo para um processo no Round Robin.
#define MAX_TIME_USED 3

static Queue* ready_queues[NUM_PRIORITIES];

/*
 Esta função irá atribuir à global "currentProcess" o ponteiro do próximo processo
a ser executado, de acordo com a ordem das filas de prioridade.
 Se nenhuma das filas tiver um processo a ser executado, o ponteiro currentProcess é null;
*/
static void get_next_process(){
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!queue_isempty(ready_queues[i])) {
            currentProcess = process_table[queue_pop(ready_queues[i])];
            currentProcess->state = PSTATE_RUNNING;
            timeUsed = 0;
            return;
        }
    }
    if (currentProcess) {
        timeUsed = 0;
        currentProcess = NULL;
    }
}

static void terminate_process(Process* p){
    currentProcess->state = PSTATE_TERMINATED;
    process_count--;
    output_event_terminate(currentProcess);
}

// -----

Process* currentProcess = NULL;
uint8_t timeUsed = 0;

extern void schedule_process(Process* p){
    if (p->instructions->length){
        p->state = PSTATE_READY;
        queue_push(ready_queues[p->priority], p->id);
    }
    else{
        // Processos sem instrução não podem chegar na CPU.
        // Necessário para suportar processos que terminam com instrução de E/S.
        terminate_process(p);  
    }
}

extern void scheduler_block(){
    currentProcess->state = PSTATE_BLOCKED;
    get_next_process();
}

extern void scheduler(){
    if (!currentProcess){
        get_next_process();
    }
    else {
        if (!currentProcess->instructions->length){
            terminate_process(currentProcess);
            get_next_process();
        }
        else if (timeUsed >= MAX_TIME_USED){
            currentProcess->priority = PREEMPTED_PROCESS_PRIORITY;
            schedule_process(currentProcess);
            output_event_preempt(currentProcess);
            get_next_process();
        }
    }
}

extern void scheduler_init(){
    for (int i = 0; i < NUM_PRIORITIES; i++)
        ready_queues[i] = queue_create(NUM_PROCESSES);
}

unsigned scheduler_qlength(int priority){
    return ready_queues[priority]->length;
}

void scheduler_qforeach(int priority, void (*fun)(TYPE)){
    queue_foreach(ready_queues[priority], fun);
}
