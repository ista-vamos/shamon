#include "compiler_utils.h"

void init_buffer_group(buffer_group *bg) {
    bg = malloc(sizeof(buffer_group));
    bg->size = 0;
    bg->head = NULL;
    bg->tail = NULL;
}

void destroy_buffer_group(buffer_group *bg) {
    dll_node * curr = bg->head;
    dll_node *next;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
    free(bg);
}

void bg_insert(buffer_group *bg, shm_stream *stream, bool (*order_exp)(shm_stream *ev1, shm_stream *ev2)) {
    dll_node *new_node =  malloc(sizeof(dll_node));
    new_node->stream = stream;
    if(bg->size == 0) {
        bg->head = new_node;
        bg->tail = new_node;
        
    } else {
        // check if it goes on the tail
        if (bg->tail->stream == stream) return;

        if (order_exp(stream, bg->tail->stream)) {
            bg->tail->next = new_node;
            new_node->next = NULL;
            new_node->prev = bg->tail;
            bg->tail = new_node;
        }

        if (bg->head->stream == stream) return;
        // check if it goes on head
        if(!order_expr(stream, bg->head->stream)) {
            new_node->next = bg->head;
            new_node->prev = NULL;
            bg->head->prev = new_node;
            bg->head = new_node;
        }

        // new node goes somewhere in the middle
        dll_node *curr = bg->head;
        while (order_exp(stream, curr->stream)) {
            // curr should never be NULL at this point
            curr = curr->next;
        }

        dll_node *prev_node = curr->prev;
        if(curr->stream == stream || prev_node->stream == stream) return;
        new_node->prev = prev_node;
        new_node->next = curr;

        prev_node->next = new_node;
        curr->prev = new_node;
    }

    bg->size +=1;
}


bool bg_remove(buffer_group *bg, shm_stream *stream) {
    // returns true only when the element was found and therefore, removed.
    dll_node * curr = bg->head;

    while(curr != NULL) {
        if (curr->stream == stream) {
            dll_node *after_curr = curr->next;
            dll_node *before_curr = curr->prev;
            before_curr->next = after_curr;
            after_curr->prev = before_curr;
            if(curr == bg->head) {
                bg->head = after_curr;
            }
            if(curr == bg->tail) {
                bg->tail = before_curr;
            }
            free(curr);
            bg->size -=1;
            return true;
        } 
        curr = curr->next;
    }
    return false;

}

void bg_remove_first_n(buffer_group *bg, int n) {
    dll_node * curr = bg->head;
    dll_node * next;
    while(n > 0 && bg->size > 0) {
        next = curr->next;
        bg_remove(bg, curr->stream);
        n--;
        curr = next;
    }
}

void bg_remove_last_n(buffer_group *bg, int n) {
    dll_node * curr = bg->tail;
    dll_node * next;
    while(n > 0 && bg->size > 0) {
        next = curr->prev;
        bg_remove(bg, curr->stream);
        n--;
        curr = next;
    }
}

shm_event *bg_get_first_n(buffer_group *bg, int n) {
    if (bg->size < n) {
        return NULL;
    }
    shm_event * result[n];

    dll_node * curr = bg->head;
    for (int i = 0; i< n; i++){
        result[i] = curr->stream;
        curr = curr->next;
    }

    return result;
}

shm_event *bg_get_last_n(buffer_group *bg, int n) {
    if (bg->size < n) {
        return NULL;
    }
    shm_event * result[n];
    dll_node * curr = bg->tail;
    for (int i = 0; i< n; i++){
        result[i] = curr->stream;
        curr = curr->prev;
    }
    return result;
}

