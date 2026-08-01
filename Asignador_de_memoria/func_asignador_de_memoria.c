#include <stdlib.h>
#include "func_asignador_de_memoria.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct bloque_metadato{
    size_t size; // Tamaño del bloque de memoria en bytes que sobra
    bool is_free; // Indica si el bloque de memoria está libre (true) o ocupado (false)
    struct bloque_metadato* siguiente;
} t_bloque_metadato;

#define TAM_METADATO sizeof(t_bloque_metadato)
#define TAM_HEAP 10000 // Tamaño del heap en bytes definido por el usuario, es el valor máximo de memoria que se puede asignar dinámicamente

static uint8_t memoria[TAM_HEAP]; //Estamos creando un es un terreno completamente vacío (bytes sin formato).
static t_bloque_metadato* free_list = NULL; // Puntero al primer bloque de metadatos en la memoria

void init(void){
    free_list = (t_bloque_metadato*)memoria; // Inicializamos la lista de bloques libres apuntando al inicio de la memoria
    free_list->size = TAM_HEAP - TAM_METADATO; // El tamaño del primer bloque libre es el tamaño total de la memoria menos el tamaño del metadato
    free_list->is_free = true; // Marcamos el primer bloque como libre
    free_list->siguiente = NULL; // No hay más bloques libres al inicio
}

/*Si tu struct ocupa 17 bytes, ¿por qué sizeof(t_bloque_metadato) devuelve 24? 
Porque a los procesadores modernos no les gusta leer la memoria en pedacitos irregulares. 
Les gusta leer la memoria en "bloques" fijos (generalmente de 8 en 8 bytes).

Bytes [0 al 7]: Guardan el size (8 bytes).

Byte [8]: Guarda el is_free (1 byte).

Bytes [9 al 15]: RELLENO INVISIBLE (7 bytes). El compilador inserta 7 bytes de "basura" o ceros que no sirven para nada, solo para hacer espacio.

Bytes [16 al 23]: Guarda el puntero siguiente (8 bytes). Ahora el puntero empieza en el índice 16, que es un múltiplo perfecto de 8.

¿Qué pasa físicamente al hacer my_malloc(100)?
El usuario de tu librería pide 100 bytes. 
Tu código de my_malloc entra en acción, ve que el bloque gigante sirve, y decide cortarlo (hacer el split).
La matemática que hicimos en el código fue:nuevo_bloque = actual + TAM_METADATO + size;Es decir: 0 + 24 + 100 = 124.
Esto significa que vas a agarrar el byte número 124 del array y le vas a incrustar a la fuerza una nueva cabecera.
Así queda físicamente el memory_pool ahora:
Bytes [0 al 23] Cabecera del Bloque 1: size=100, is_free=false.
Bytes [24 al 123]Carga Útil del Bloque 1: (Exactamente 100 bytes).
Bytes [124 al 147]Cabecera del Bloque 2 (El nuevo): size=9852, is_free=true.
Bytes [148 al 9999]Carga Útil del Bloque 2: (9852 bytes libres).
*/

void *func_malloc(size_t size) {
    // 1. Validación básica
    if (size == 0) {
        return NULL; 
    }

    // 2. Empezamos a buscar desde el inicio de nuestra memoria
    t_bloque_metadato *actual = free_list;

    while (actual != NULL) {
        // ¿El bloque está libre y tiene tamaño suficiente?
        if (actual->is_free && actual->size >= size) {
            
            // 3. ¿El bloque es lo suficientemente grande como para dividirlo?
            // Necesitamos espacio para lo que pide el usuario + una nueva cabecera + al menos 1 byte extra
            if (actual->size >= size + TAM_METADATO + 1) {
                
                // ¡Aritmética de punteros al rescate!
                // Calculamos dónde irá la nueva cabecera del bloque sobrante.
                // Convertimos a (uint8_t *) para avanzar byte por byte, y luego lo moldeamos al struct.
                t_bloque_metadato *nuevo_bloque = (t_bloque_metadato *)((uint8_t *)actual + TAM_METADATO + size);
                
                // Configuramos el nuevo bloque libre con lo que sobra
                nuevo_bloque->size = actual->size - size - TAM_METADATO;
                nuevo_bloque->is_free = true;
                nuevo_bloque->siguiente = actual->siguiente;
                
                // Ajustamos el tamaño de nuestro bloque actual y lo enlazamos al nuevo
                actual->size = size;
                actual->siguiente = nuevo_bloque;
            }
            
            // 4. Marcamos el bloque como ocupado
            actual->is_free = false;
            
            // Le devolvemos al usuario el puntero a su espacio útil (saltando nuestra cabecera)
            return (void *)((uint8_t *)actual + TAM_METADATO);
        }
        
        // Si no nos sirvió este bloque, saltamos al siguiente
        actual = actual->siguiente;
    }

    // Si el bucle termina y llegamos aquí, significa que recorrimos toda la memoria
    // y no encontramos ningún bloque adecuado. Nos quedamos sin espacio.
    return NULL;
}

void func_free(void *ptr) {
    if (ptr == NULL) {
        return; // No hacemos nada si el puntero es NULL
    }
    
    // Obtenemos el puntero al metadato del bloque a liberar
    t_bloque_metadato *bloque = (t_bloque_metadato *)((uint8_t *)ptr - TAM_METADATO);
    
    // Marcamos el bloque como libre
    bloque->is_free = true;
}