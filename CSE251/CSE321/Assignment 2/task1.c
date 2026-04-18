#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *fib_seq = NULL;
int n = 0;
int *search_idx = NULL;
int s = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void input(void) {
    char buf[200];
    fgets(buf, sizeof(buf), stdin);
}

int get_integer(const char *prompt) {
    int value;
    int ret;
    while (1) {
        if (prompt) printf("%s", prompt);
        ret = scanf("%d", &value);
        input();
        if (ret == 1) return value;
        printf("Invalid input. Please enter an integer.\n");
    }
}

void *gen_fib(void *arg) {
    (void)arg;
    int *res = malloc((n + 1) * sizeof(int));
    if (!res) {
        pthread_exit(NULL);
    }
    res[0] = 0;
    if (n > 0) res[1] = 1;
    for (int i = 2; i <= n; ++i) res[i] = res[i - 1] + res[i - 2];
    pthread_mutex_lock(&mtx);
    fib_seq = res;
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

void *search_fib(void *arg) {
    (void)arg;
    for (int i = 0; i < s; ++i) {
        int idx = search_idx[i];
        int result = -1;
        pthread_mutex_lock(&mtx);
        if (fib_seq != NULL && idx >= 0 && idx <= n) result = fib_seq[idx];
        pthread_mutex_unlock(&mtx);
        printf("result of search #%d = %d\n", i + 1, result);
    }
    pthread_exit(NULL);
}

int main(void) {
    while (1) {
        n = get_integer("Enter the term of fibonacci sequence:\n");
        if (n < 0 || n > 40) printf("Please enter n between 0 and 40 (inclusive).\n");
        else break;
    }
    while (1) {
        s = get_integer("How many numbers you are willing to search?:\n");
        if (s <= 0) printf("Number of searches must be greater than 0.\n");
        else break;
    }
    search_idx = malloc(s * sizeof(int));
    if (!search_idx) return 1;
    for (int i = 0; i < s; ++i) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Enter search %d:\n", i + 1);
        search_idx[i] = get_integer(prompt);
    }
    pthread_t t1, t2;
    if (pthread_create(&t1, NULL, gen_fib, NULL) != 0) {
        free(search_idx);
        return 1;
    }
    pthread_join(t1, NULL);
    pthread_mutex_lock(&mtx);
    if (fib_seq == NULL) {
        pthread_mutex_unlock(&mtx);
        free(search_idx);
        return 1;
    }
    for (int i = 0; i <= n; ++i) printf("a[%d] = %d\n", i, fib_seq[i]);
    pthread_mutex_unlock(&mtx);
    if (pthread_create(&t2, NULL, search_fib, NULL) != 0) {
        free(search_idx);
        return 1;
    }
    pthread_join(t2, NULL);
    pthread_mutex_lock(&mtx);
    free(fib_seq);
    fib_seq = NULL;
    pthread_mutex_unlock(&mtx);
    free(search_idx);
    pthread_mutex_destroy(&mtx);
    return 0;
}
