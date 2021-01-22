#include "process.h"
#include "queue.h"

Process processList[2] = {
    {.start=0},
    {.start=5}
};

Queue ready_queues[2];

unsigned CPUtime;
Process currentProcess;
unsigned timeUsed;

Queue IOqueues[IO_DEVICE_NUMBER];

int main(void){
    
    
}
