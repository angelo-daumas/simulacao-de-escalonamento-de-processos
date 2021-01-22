 #pragma once

 // Defina aqui o tipo dos elementos guardados na fila.
 typedef short TYPE;

typedef struct Queue { 
    TYPE* array;
    int first, last;  // apontam posições no array da fila
    unsigned length, maxlength; 
} Queue; 