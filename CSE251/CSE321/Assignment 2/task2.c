#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define TOTAL_STUDENTS 10
#define CHAIRS 3

sem_t students_sem;                  
sem_t ready_sem[TOTAL_STUDENTS];      
sem_t consult_done;
pthread_mutex_t mutex;                

int waiting = 0;     
int served = 0;     
int left_count = 0;   

int q[TOTAL_STUDENTS];
int qh = 0, qt = 0;

static void enqueue(int id) { q[qt] = id; qt = (qt + 1) % TOTAL_STUDENTS; }
static int  dequeue(void)   { int id = q[qh]; qh = (qh + 1) % TOTAL_STUDENTS; return id; }

void *student_thread(void *arg) {
    int id = *(int *)arg;
    free(arg);

    usleep((rand() % 500 + 100) * 1000);

    pthread_mutex_lock(&mutex);
    if (waiting < CHAIRS) {
        enqueue(id);
        waiting++;
        printf("Student %d started waiting for consultation\n", id);
        sem_post(&students_sem);    
        pthread_mutex_unlock(&mutex);
        sem_wait(&ready_sem[id]);   
        printf("Student %d is getting consultation\n", id);
        usleep((rand() % 700 + 200) * 1000);
        printf("Student %d finished getting consultation and left\n", id);
        pthread_mutex_lock(&mutex);
        served++;
        printf("Number of served students: %d\n", served);
        pthread_mutex_unlock(&mutex);
        sem_post(&consult_done);
       } else {
        printf("No chairs remaining in lobby. Student %d Leaving.....\n", id);
        pthread_mutex_unlock(&mutex);

        printf("Student %d finished getting consultation and left\n", id);
        pthread_mutex_lock(&mutex);
        served++;
        printf("Number of served students: %d\n", served);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
void *st_thread(void *arg) {
    (void)arg;
    while (1) {
        sem_wait(&students_sem);

        pthread_mutex_lock(&mutex);

        if (waiting == 0 && (served + left_count) >= TOTAL_STUDENTS) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        if (waiting > 0) {
            int sid = dequeue();
            waiting--;
            printf("A waiting student started getting consultation\n");
            printf("Number of students now waiting: %d\n", waiting);
            pthread_mutex_unlock(&mutex);
            printf("ST giving consultation\n");
            sem_post(&ready_sem[sid]);
            sem_wait(&consult_done);
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}
int main(void) {
    srand((unsigned)time(NULL));

    pthread_t st;
    pthread_t students[TOTAL_STUDENTS];
    sem_init(&students_sem, 0, 0);
    sem_init(&consult_done, 0, 0);
    for (int i = 0; i < TOTAL_STUDENTS; ++i) {
        sem_init(&ready_sem[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&st, NULL, st_thread, NULL);
    for (int i = 0; i < TOTAL_STUDENTS; ++i) {
        int *id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&students[i], NULL, student_thread, id);
        usleep((rand() % 150 + 50) * 1000);
    }
    for (int i = 0; i < TOTAL_STUDENTS; ++i) {
        pthread_join(students[i], NULL);
    }
    sem_post(&students_sem);
    pthread_join(st, NULL);
    sem_destroy(&students_sem);
    sem_destroy(&consult_done);
    for (int i = 0; i < TOTAL_STUDENTS; ++i) sem_destroy(&ready_sem[i]);
    pthread_mutex_destroy(&mutex);

    return 0;
}
