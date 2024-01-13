#include "list.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *new_node(char *data) {
    Node *node = malloc(sizeof(Node));
    node->file_name = data;
    node->next = NULL;
    return node;
}

void add_node(List *list, char *data) {
    Node *node = new_node(data);
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

void free_node(Node *node) {
    if (node != NULL) {
        if (node->file_name != NULL) {
            SDL_free(node->file_name);
            node->file_name = NULL;
        }
        node->next = NULL;
        free(node);
    }
}


void clear_list(Node *head){
    Node *tmp;
    while(head!=NULL){
        tmp = head->next;
        free_node(head); 
        head = tmp;
    }
}


void remove_node(Node *current, Node *previous, List *list) {
    if (previous != NULL) {
        previous->next = current->next;
    } else {
        list->head = current->next;
    }

    if (current->next == NULL) {
        list->tail = previous;
    }
   free_node(current);
}

void print_list(List *list) {
    Node *tmp = list->head;
    while (tmp) {
        puts(tmp->file_name);
        tmp = tmp->next;
    }
}
