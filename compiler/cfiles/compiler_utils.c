#include "compiler_utils.h"
#include "../../core/shamon.h"
#include "../../gen/mmlib.h"

void init_buffer_group(buffer_group *bg) {
    //bg = malloc(sizeof(buffer_group));
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

void bg_insert(buffer_group *bg, shm_stream *stream, void* buffer, void *args, bool (*order_exp)(void *args1, void *args2)) {
    dll_node *new_node =  malloc(sizeof(dll_node));
    new_node->stream = stream;
    new_node->buffer = buffer;
    new_node->args = args;
    printf("beginning : %d\n", order_exp(new_node->args, args));
    if(bg->size == 0) {
        bg->head = new_node;
        bg->tail = new_node;
        printf("size bg is 0\n");
        
    } else {
        // check if it goes on the tail
        if (bg->tail->stream == stream) return;
        if (bg->head->stream == stream) return;
        if (order_exp(new_node->args, bg->tail->args)) {
            bg->tail->next = new_node;
            new_node->next = NULL;
            new_node->prev = bg->tail;
            bg->tail = new_node;            
        }
        // check if it goes on head
        
        else if(!order_exp(args, bg->head->args)) {
            new_node->next = bg->head;
            new_node->prev = NULL;
            bg->head->prev = new_node;
            bg->head = new_node;
        }else {
            // new node goes somewhere in the middle
            dll_node *curr = bg->head;
            while (order_exp(new_node->args, curr->args)) {
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

dll_node ** bg_get_first_n(buffer_group *bg, int n) {
    if (bg->size < n) {
        return NULL;
    }
    dll_node **result = calloc(n, sizeof(dll_node*));
    dll_node * curr = bg->head;
    for (int i = 0; i < n; i++){
        result[i] = curr;
        curr = curr->next;
        result++;
    }
    return result;
}

dll_node ** bg_get_last_n(buffer_group *bg, int n) {
    if (bg->size < n) {
        return NULL;
    }
    dll_node **result = calloc(n, sizeof(dll_node*));
    dll_node * curr = bg->tail;
    for (int i = 0; i< n; i++){
        result[i] = curr;
        curr = curr->prev;
    }
    return result;
}

