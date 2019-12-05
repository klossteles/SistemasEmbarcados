/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Kloss Teles
**/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct queue_t;

// Insere um elemento no final da fila.
void queue_append (queue_t **queue, queue_t *elem) {
    
    queue_t *aux;

    if (elem == NULL) {
        #ifdef DEBUG
            printf("ERROR: O elemento não existe.\n");
        #endif
        return;
    } else if (elem->prev != NULL && elem->next != NULL) {
        #ifdef DEBUG
            printf("ERROR: O elemento pertence à outra fila.\n");
        #endif
        return;
    } else if (*queue == NULL) {
        #ifdef DEBUG
            printf("Fila não existe. Estamos criando uma nova...\n");
        #endif
        aux = elem;
        aux->next = elem;
        aux->prev = elem;
        *queue = aux;
    } else {
        #ifdef DEBUG
            printf("Adicionando elemento no fim da fila...\n");
        #endif
        aux = (*queue)->prev; // último elemento
        aux->next = elem; // aponta para o novo elemento inserido no fim da fila
        elem->prev = aux; // aponta para o último elemento da fila
        elem->next = (*queue); // aponta para o primeiro elemento da fila
        (*queue)->prev = elem; // aponta para o 
    }
}

// Remove o elemento indicado da fila, sem o destruir.
// Retorno: Apontador para o elemento removido, ou NULL se erro.
queue_t *queue_remove (queue_t **queue, queue_t *elem) {

    if (elem == NULL) {
        #ifdef DEBUG
            printf("ERROR: O elemento não existe.\n");
        #endif
        return NULL;
    } else if (*queue == NULL) {
        #ifdef DEBUG
            printf("ERROR: A fila não existe.\n");
        #endif
        return NULL;
    } else if (!(queue_size(*queue) > 0)) {
        printf("ERROR: A fila está vazia.\n");
        return NULL;
    } else {
        if (elem == (*queue)) { // Se o elemento for o primeiro da fila.
            if (((*queue)->next == elem) && ((*queue)->prev == elem)) { // Se o elemento é o único da fila.
                *queue = NULL;
            } else {
                // Vincula o elemento posterior com o anterior.
                elem->prev->next = elem->next;
				elem->next->prev = elem->prev;
                // O início da fila é alterado.
                *queue = elem->next;
            }

            // Desvincula o elemento da fila.
			elem->next = NULL;
			elem->prev = NULL;
			return elem;
        } else {
            queue_t *aux = (*queue)->next;

            while (aux != *queue) { // Busca pelo elemento na fila.
                if (aux == elem) {
                    // Vincula o elemento posterior com o anterior.
                    elem->prev->next = elem->next;
                    elem->next->prev = elem->prev;
					// Desvincula o elemento da fila.
					elem->prev = NULL;
					elem->next = NULL;
					return elem;
                }

                aux = aux->next;
            }
            #ifdef DEBUG
                printf("ERROR: O elemento não se encontra na fila.\n");
            #endif
            return NULL;
        }
    }
}

// Conta o número de elementos na fila.
// Retorno: Número de elementos na fila.
int queue_size (queue_t *queue) {

    int count = 0;

    if (queue == NULL) {
        #ifdef DEBUG
            printf("ERROR: A fila não existe.\n");
        #endif
        return count;
    } else {
        count++; // A fila possui pelo menos 1 elemento.
    }

    queue_t *elem = queue;

    while (elem->next != queue) {
        elem = elem->next;
        count++;
    }

    return count;
}

// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função segue o protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir
void queue_print (char *name, queue_t *queue, void print_elem (void*) )
{
	if (queue == NULL)
	{
		printf("%s: []", name); // Fila vazia.
		printf("\n");
		return;
	}

    queue_t *aux = queue;

    printf("%s: [", name);

    print_elem(aux); // Imprimi o primeiro elemento da fila.

    while (aux->next != queue)
    {
        aux = aux->next;
        printf(" ");
        print_elem(aux);
    }
    
    printf("]\n");
}