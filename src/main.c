#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>

static int *ptr_arr_len; 
static int *ptr_matrix;
static int x, y;
static int length, widht;

static int *ptr_arr_1; 
static sem_t empty;
static sem_t full;
static pthread_mutex_t mutex_deposit;
static pthread_mutex_t mutex_fetch;


int my_max(int *arr)
{
    int res = arr[0];
    for (int i = 1; i < 4; ++i) {
        if (arr[i] > res) {
            res = arr[i];
        }
    }
    return res;
}

void rm_max(int *arr)
{
    int len_max = my_max(arr);
    for (int i = 0; i < 4; ++i) {
        if (arr[i] == len_max) {
            arr[i] = 0;
            break;
        }
    }
}


void print_matrix(void)
{
    printf("%s\n", "Map:" );
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            printf(" %d  ", *(ptr_matrix + i * length + j));
        }
        printf("\n");
    }
}

void print_matrix_with_element(void)
{
    printf("%s\n", "Map and selected item:");
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            if (i == y && j == x) {
                printf("|%d| ", *(ptr_matrix + i * length + j));
            }
            else {
                printf(" %d  ", *(ptr_matrix + i * length + j));
            }
        }
        printf("\n");
    }
}

void init_matrix(void)
{
    int tmp = 0;
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            tmp = rand() % 2;
            *(ptr_matrix + i * length + j)  = tmp;
        }
    }
}

void select_element(void)
{
    x = rand() % length;
    y = rand() % widht;
    printf("Selected coordinates X: %d Y: %d\n", x + 1, y + 1 );
    print_matrix_with_element();
}

void choice_path_right(int number)
{
    int number_1 = 0;
    printf("Scout %d go right\n", number + 1);
    printf("-------------------------------------------------\n");
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            if (i == y && j > x) {
                if (*(ptr_matrix + i * length + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    sem_wait(&empty);

    ptr_arr_1[0] = number;
    ptr_arr_1[1] = number_1;
    sem_post(&full);
}

void choice_path_left(int number)
{
    printf("Scout %d go left\n", number + 1);
    printf("-------------------------------------------------\n");
    int number_1 = 0;
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            if (i == y && j < x) {
                if (*(ptr_matrix + i * length + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    sem_wait(&empty);
    ptr_arr_1[0] = number;
    ptr_arr_1[1] = number_1;
    sem_post(&full);
}

void choice_path_down(int number)
{
    printf("Scout %d go down\n" , number + 1);
    printf("-------------------------------------------------\n");
    int number_1 = 0;
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            if (i > y && j == x) {
                if (*(ptr_matrix + i * length + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    sem_wait(&empty);
    ptr_arr_1[0] = number;
    ptr_arr_1[1] = number_1;
    sem_post(&full);
}

void choice_path_up(int number)
{
    printf("Scout %d go up\n" , number + 1);
    printf("-------------------------------------------------\n");
    int number_1 = 0;
    for (int i = 0; i < widht; ++i) {
        for (int j = 0; j < length; ++j) {
            if (i < y && j == x) {
                if (*(ptr_matrix + i * length + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    sem_wait(&empty);
    ptr_arr_1[0] = number;
    ptr_arr_1[1] = number_1;
    sem_post(&full);
}

void *func_pthread(void *arg)
{
    int number_pthread = *((int*) arg);
    int len_max;
    int tmp_arr[4];

    for (int i = 0; i < 4; ++i) {
        tmp_arr[i] = ptr_arr_len[i];
    }

    for (int i = 0; i < number_pthread; ++i ) {
        rm_max(tmp_arr);
    }
    len_max = my_max(tmp_arr);

    if (len_max == tmp_arr[0]) {
        choice_path_right(number_pthread);
    }
    else if (len_max == tmp_arr[1]) {
        choice_path_left(number_pthread);
    }
    else if (len_max == tmp_arr[2]) {
        choice_path_down(number_pthread);
    }
    else if (len_max == tmp_arr[3]) {
        choice_path_up(number_pthread);
    }

    pthread_exit(NULL);
}

int main(void)
{   
    srand(time(NULL));

    length = 5 + rand() % 6;
    widht = 5 + rand() % 6;

    printf("Number of rows: %d\n", widht);
    printf("Number of columns: %d\n", length);

    int matrix[widht][length];
    ptr_matrix = (int*) matrix;

    init_matrix();
    print_matrix();
    printf("-------------------------------------------------\n");

    select_element();
    printf("-------------------------------------------------\n");

    int len[4];
    len[0] = length - (x + 1);
    len[1] = length - (len[0] + 1);
    len[2] = widht - (y + 1);
    len[3] = widht - (len[2] + 1);
    ptr_arr_len = len;

    printf("Lengths of paths: %d %d %d %d\n",
        ptr_arr_len[0],
        ptr_arr_len[1],
        ptr_arr_len[2],
        ptr_arr_len[3]);

    printf("-------------------------------------------------\n");

    int number_pthread = rand() % 5;
    pthread_t arr_pthread[number_pthread];
    int arr_number_1[2];
    ptr_arr_1 = arr_number_1;

    int in_arr[number_pthread];
    for (int i = 0; i < number_pthread; ++i) {
        in_arr[i] = i;
    }


    if (sem_init(&empty, 0, 1) ||
        sem_init(&full, 0, 0)) {
        fprintf(stderr, "Failed to init semaphore\n");
        return EXIT_FAILURE;
    }


    printf("Number of scouts: %d\n", number_pthread);
    for (int i = 0; i < number_pthread; ++i) { 
        pthread_create(&(arr_pthread[i]), NULL, func_pthread, (void *) (&in_arr[i]));
    }

    for (int i = 0; i < number_pthread; ++i) {
        sem_wait(&full);
        printf("Scout %d found %d items\n", ptr_arr_1[0] + 1, ptr_arr_1[1]);
        printf("-------------------------------------------------\n");
        sem_post(&empty);
    }


    for (int i = 0; i < number_pthread; ++i) {
        pthread_join(arr_pthread[i], NULL);
    }

    if (sem_destroy(&full) || sem_destroy(&empty)) {
        fprintf(stderr, "Failed to destroy semaphore\n");
        return EXIT_FAILURE;
    }

    return 0;
}
