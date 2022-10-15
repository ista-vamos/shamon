#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
    (void)argc;
    int N = atoi(argv[1]);
    for (int i = 0; i < N; ++i) {
            printf("%d\n", i);
    }
    return 0;
}
