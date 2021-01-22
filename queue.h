#pragma once

#include <stdlib.h>

 // Defina aqui o tipo dos elementos guardados na fila.
 typedef short TYPE;

typedef struct Queue { 
    TYPE* array;
    int first, last;  // apontam posições no array da fila
    unsigned length, maxlength; 
} Queue;

// Cria uma fila com uma certa capacidade.
Queue* queue_create(unsigned maxlength);

// Fila está cheia?
int queue_isfull(Queue* queue);

// Fila está vazia?
int queue_isempty(Queue* queue);

// Retorna o primeiro elemento da fila.
TYPE queue_peek(Queue* queue);

// Coloca um elemento no final da fila.
void queue_push(Queue* queue, TYPE item);

// Tira o elemento do início da fila e o retorna.
TYPE queue_pop(Queue* queue);
