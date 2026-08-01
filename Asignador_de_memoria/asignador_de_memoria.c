#include <stdio.h>
#include <string.h> // Para usar strcpy()
#include "func_asignador_de_memoria.h"

int main() {
    printf("--- INICIANDO PRUEBAS DEL ASIGNADOR DE MEMORIA ---\n\n");

    /* 
     * PASO 1: (Opcional) Inicializar la memoria.
     * Si tu código requiere llamar a una función inicial antes de empezar 
     * (por ejemplo, para configurar el bloque gigante inicial), hazlo aquí.
     * Ej: init_memory(); 
     */

    init();
    
    // --- PRUEBA 1: Reservar espacio para un número ---
    printf("Prueba 1: Reservando memoria para un entero (int)...\n");
    
    int *mi_numero = (int *)func_malloc(sizeof(int));
    
    // Siempre hay que comprobar si la memoria se asignó correctamente
    if (mi_numero == NULL) {
        printf("ERROR: func_malloc devolvio NULL. No hay espacio.\n");
        return 1; // Salimos del programa con error
    }
    
    // Usamos la carga útil
    *mi_numero = 42; 
    printf("EXITO: Se guardo el numero %d en la direccion %p\n\n", *mi_numero, (void*)mi_numero);


    // --- PRUEBA 2: Reservar espacio para un texto (Array de caracteres) ---
    printf("Prueba 2: Reservando 20 bytes para un texto...\n");
    
    char *mi_texto = (char *)func_malloc(20);
    
    if (mi_texto == NULL) {
        printf("ERROR: No se pudo reservar memoria para el texto.\n");
        return 1;
    }
    
    // Escribimos en la carga útil
    strcpy(mi_texto, "Hola Mundo!");
    printf("EXITO: Se guardo el texto '%s' en la direccion %p\n\n", mi_texto, (void*)mi_texto);


    // --- PRUEBA 3: Liberar la memoria ---
    printf("Prueba 3: Liberando la memoria...\n");
    
    func_free(mi_numero);
    printf("Memoria de 'mi_numero' liberada.\n");
    
    func_free(mi_texto);
    printf("Memoria de 'mi_texto' liberada.\n\n");


    // --- PRUEBA 4: Reutilización (La prueba de fuego) ---
    printf("Prueba 4: Pidiendo memoria de nuevo para ver si recicla los bloques...\n");
    
    int *nuevo_numero = (int *)func_malloc(sizeof(int));
    
    if (nuevo_numero != NULL) {
        *nuevo_numero = 99;
        // Si el sistema funciona perfecto, esta dirección debería ser IGUAL 
        // a la dirección que tuvo 'mi_numero' en la Prueba 1.
        printf("EXITO: Nuevo numero %d guardado en la direccion %p\n", *nuevo_numero, (void*)nuevo_numero);
    }
    
    func_free(nuevo_numero);

    printf("\--- TODAS LAS PRUEBAS FINALIZADAS ---\n");
    return 0;
}