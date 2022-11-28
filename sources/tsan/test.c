#include <stdio.h>
#include <threads.h>

mtx_t m;
int print_num = 0;

int thread(void *data) {
    thrd_t tid1, tid2;

    for (int i = 0; i < 10; ++i) {
	mtx_lock(&m);
	++print_num;
        printf("thread %lu: %d\n", (size_t)data, i);
	mtx_unlock(&m);
    }

    thrd_exit(0);
}

int main(void) {
    mtx_init(&m, mtx_plain);
    thrd_t tid1, tid2, tid3, tid4;
    thrd_create(&tid1, thread, (void *)1);
    thrd_create(&tid2, thread, (void *)2);
    thrd_create(&tid3, thread, (void *)3);
    thrd_create(&tid4, thread, (void *)4);

    for (int i = 0; i < 10; ++i) {
	mtx_lock(&m);
	++print_num;
        printf("thread %d: %d\n", 0, i);
	mtx_unlock(&m);
    }

    thrd_join(tid1, NULL);
    thrd_join(tid2, NULL);
    thrd_join(tid3, NULL);
    thrd_join(tid4, NULL);
    printf("Printed %d messages\n", print_num);
}
