#ifdef __cplusplus
extern "C" {
#endif

typedef struct _intmap {
    void *data_structure;
} intmap;

void init_intmap(intmap *m);
void destroy_intmap(intmap *m);

int intmap_remove_upto(intmap* m, int key); // removes all keys less than or equal to n. Returns the number of keys erased

void intmap_insert(intmap* m, int key, int value); // inserts an element with key n and value p

int intmap_get(intmap *m,int key, int *value); // returns 1 when the element is found, otherwise 0. Writes the value of element with key n in result

void intmap_clear(intmap *m); //removes all entries

#ifdef __cplusplus
}
#endif