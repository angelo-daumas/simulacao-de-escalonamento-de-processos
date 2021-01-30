#include <stdio.h>
#include <stdint.h>

//função sleep
#include <unistd.h>

#include "process.h"
#include "queue.h"
#include "scheduler.h"
#include "devices.h"
#include "creator.h"
#include "output.h"

const char* device_names[3];
static void get_user_input();

// Tempo total de execução do simulador (i.e. uptime da CPU).
unsigned CPUtime = 0;

void simulateCPU(){
    int instruction;
    while(currentProcess && (instruction = queue_pop(currentProcess->instructions)) != CPU){
        printf("[Tick %d]\tCPU: REQUEST %s (pid %d)\n", CPUtime, device_names[instruction], currentProcess->id);
        request_device(instruction);
    }
    
    timeUsed++;
    output_info(currentProcess);
    
    CPUtime++;
    
}

//------------------------------------------------------------------------------
//                         S I M U L A D O R
//------------------------------------------------------------------------------
/*
 Para tornar mais fácil o entendimento do projeto, o simulador foi implementado
de forma modular.

 Cada fase do funcionamento da máquina é implementada por funções distintas. 
Para explorar a implementação dessas funções, basta navegar para o arquivo onde 
elas são definidas.
*/

int main(void){
    
    // Inicializar o sistema.
    devices_init();
    scheduler_init();
    initialize_processes();

    // Simular o loop de funcionamento do sistema:
    while (process_count > 0 || has_incoming_processes()){
        // Passo 1: Simular a chegada de novos processos.
        create_processes(); // (implementação: creator.c)
        
        // Passo 2: Simular a execução do escalonador.
        scheduler();  // (implementação: scheduler.c)
        
        // Passo 3: Simular o processamento na CPU.
        simulateCPU();
        
        if (is_interactive) get_user_input();  // não faz parte do simulador

        // Passo 4: Simular o processamento nos dispositivos de E/S.
        simulateIO(); // (implementação: devices.c)
    }
}

//------------------------------------------------------------------------------
// Código usado para gerar a saída do programa e mostar informações da simulação.

static const char instruction_chars[] = {'D', 'T', 'P', 'C'};
static const char* priority_names[] = {"HIGH", "LOW"};
static const char* pstate_names[] = {"XXXXX", "Creat", "Ready", "Run", "Block", "Termi"};
const char* device_names[3] = {"DISK", "TAPE", "PRINTER"};

#define SHOW_EMPTY_QUEUES 1
#define SHOW_EVENTS 1

static void print_int(TYPE pid){
    printf("%d ", pid);
}

static void print_instruction(TYPE i){
    printf("%c", instruction_chars[i]);
}

static void print_process(Process* p){
    printf("%2d\t%5d\t%5s\t", p->id, p->start, pstate_names[p->state]);
    queue_foreach(p->instructions, &print_instruction);
    printf("\n");
}

static void print_process_table(){
    printf("ID\tSTART\tSTATE\tINSTRUCTIONS\n");
    for (int i=1; i <= NUM_PROCESSES; i++){
        Process* p = process_table[i];
        if (p)
            print_process(p);
    }
    printf("\n");
}

static void print_queues(){
    for (int i =0; i < NUM_PRIORITIES; i++){
        if (SHOW_EMPTY_QUEUES || scheduler_qlength(i) > 0){
           printf("[Tick %d]\tReady Queue (%s): { ", CPUtime, priority_names[i]);
            scheduler_qforeach(i, *print_int);
            printf("}\n"); 
        }
    }
    
    for (int i =0; i < IO_DEVICE_NUMBER; i++){
        if (SHOW_EMPTY_QUEUES || device_isactive(i)){
           printf("[Tick %d]\tI/O Queue (%s): { ", CPUtime, device_names[i]);
            device_qforeach(i, *print_int);
            printf("}\n"); 
        }
    }
    
    
    printf("\n");
}

static void get_user_input(){
    printf("Press enter to continue (type T to view process table or P to stop simulation)...\n");
    char c;
    while ((c = getchar())){
        switch (c){
            case '\n': return;
            case 't': print_process_table(); break;
            case 'T': print_process_table(); break;
            case 'P': exit(0);
            case 'p': exit(0);
        }
    };
}

// -----

extern void output_event_preempt(Process* p){
    if (SHOW_EVENTS)
        printf("[Tick %d]\tSCHEDULER: PREEMPT (pid %d)\n", CPUtime, p->id);
}

extern void output_event_terminate(Process *p){
    if (SHOW_EVENTS)
        printf("[Tick %d]\tSCHEDULER: TERMINATE (pid %d)\n", CPUtime, p->id);
}

extern void output_event_iofinished(int device, Process* p){
    if (SHOW_EVENTS)
        printf("[Tick %d]\t%s: FINISHED (pid %d)\n", CPUtime, device_names[device], p->id);
}

extern void output_info(Process* p){
    printf("[Tick %d]\t", CPUtime);

    if (p){
        if (p->state == PSTATE_CREATED){
            printf("New process: pid %d (", p->id);
            queue_foreach(p->instructions, &print_instruction);
            printf(")\n");
        }
        else{
            printf("CPU: RUNNING (pid %d) - t=%d\n", p->id, timeUsed);
            print_queues();
        }
            
    }
    else{
        printf("CPU: IDLE - t=%d\n", timeUsed);
        print_queues();
    }
        

}
