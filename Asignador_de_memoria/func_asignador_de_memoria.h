#ifndef FUNC_ASIGNADOR_DE_MEMORIA_H
#define FUNC_ASIGNADOR_DE_MEMORIA_H

#include <stdlib.h> // Necesario para size_t

// Inicializa el asignador de memoria
void init(void);

// Reserva 'size' bytes y devuelve un puntero a esa memoria
void *func_malloc(size_t size);

// Libera la memoria previamente reservada
void func_free(void *ptr);

#endif // FUNC_ASIGNADOR_DE_MEMORIA_H