/*
    Arquivo contendo a implementação de uma fila utilizando um array acessado de forma circular.
*/
#include "queue.h"

// Cria uma fila com uma certa capacidade.
Queue* queue_create(unsigned maxlength) { 
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    
    queue->first = queue->length = 0;
    queue->last = maxlength - 1;  // inicializando assim, se tornará 0 na primeira chamada de "push".
    queue->maxlength = maxlength;
    
    queue->array = (TYPE*) malloc(maxlength * sizeof(TYPE));
    return queue;
} 

// Fila está cheia?
int queue_isfull(Queue* queue) {
    return (queue->length == queue->maxlength);
}

// Fila está vazia?
int queue_isempty(Queue* queue) {
    return !(queue->length);
}

// Retorna o primeiro elemento da fila.
TYPE queue_peek(Queue* queue) {
    return queue->array[queue->first];
}

// Coloca um elemento no final da fila.
void queue_push(Queue* queue, TYPE item) {
    if (!queue_isfull(queue)) {
        queue->last = (queue->last + 1) % queue->maxlength;
        queue->array[queue->last] = item;
        queue->length++;
    }
}

// Tira o elemento do início da fila e o retorna.
TYPE queue_pop(Queue* queue) {
    TYPE item = queue_peek(queue);
    queue->first = (queue->first + 1) % queue->maxlength;
    queue->length--; 
    return item;
}
