#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define debug
#define max_range 2

pthread_mutex_t lock;
sem_t locked_value;

int m_row = 0;

int **matrix_1;
int **matrix_2;

int matrix_1_size[2];
int matrix_2_size[2];

int **matrix_result;


void *matrix_multiply();
void matrix_fill(int **matrix, int *size);


int main(int argc, char **argv)
{
    pthread_t *threads;
    int threads_amount;

    if (argc != 6)
    {
        printf("usage: [szerokosc 1] [wysokosc 1] [szerokosc 2] [wysokosc 2] [ilosc watkow]\n");
        exit(1);
    }

    srand(time (NULL));

    matrix_1_size[0] = atoi(argv[1]);
    matrix_1_size[1] = atoi(argv[2]);

    matrix_2_size[0] = atoi(argv[3]);
    matrix_2_size[1] = atoi(argv[4]);

    threads_amount = atoi(argv[5]);

    if (matrix_1_size[0] != matrix_2_size[1])
        exit(1);

    matrix_1 = malloc(sizeof (int *) * matrix_1_size[0]);

    for (int i = 0; i < matrix_1_size[0]; i++)
     matrix_1[i] = malloc(sizeof (int) * matrix_1_size[1]);

    matrix_2 = malloc(sizeof (int *) * matrix_2_size[0]);

    for (int i = 0; i < matrix_2_size[0]; i++)
     matrix_2[i] = malloc(sizeof (int) * matrix_2_size[1]);

    matrix_result = malloc(sizeof (int *) * matrix_1_size[0]);

    for (int i = 0; i < matrix_1_size[0]; i++)
     matrix_result[i] = malloc (sizeof (int) * matrix_2_size[1]);

    matrix_fill(matrix_1, matrix_1_size);
    matrix_fill(matrix_2, matrix_2_size);
    printf("DEBUG\n");
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("failed\n");
        exit(1);
    }
printf("DEBUG\n");

    threads = malloc(sizeof (pthread_t) * threads_amount);
printf("DEBUG\n");

    for (int i = 0; i < threads_amount; i++)
    {
        if (pthread_create(&(threads[i]), NULL, (void *(*)(void *))matrix_multiply, NULL) != 0)
        {
            printf("cant create a thread!\n");
            exit(1);
        }
    }
printf("DEBUG\n");

    for (int i = 0; i < threads_amount; i++)
    {
    //    pthread_join(threads[i], NULL);
    }
printf("DEBUG %d %d\n", matrix_1_size[0], matrix_2_size[1]);

    pthread_mutex_destroy(&lock);

    for (int row = 0; row < matrix_1_size[0]; row++)
    {
        for (int col = 0; col < matrix_1_size[1]; col++)
        {
            printf("%d ", matrix_1[row][col]);
        }
        printf("\n");
    }

        printf("\n");

    for (int row = 0; row < matrix_2_size[0]; row++)
    {
        for (int col = 0; col < matrix_2_size[1]; col++)
        {
            printf("%d ", matrix_2[row][col]);
        }
        printf("\n");
    }

        printf("\n");

    for (int row = 0; row < matrix_1_size[0]; row++)
    {
        for (int col = 0; col < matrix_2_size[1]; col++)
        {
            printf("%d ", matrix_result[row][col]);
        }
        printf("\n");
    }




    return 0;
}

void matrix_fill(int **matrix, int *size)
{
    for (int i = 0; i < size[0]; i++)
    {
        for (int j = 0; j < size[1]; j++)
        {
            matrix[i][j] = rand() % max_range;
        }
    }
}

void *matrix_multiply()
{
    int row;

    while (1)
    {
        pthread_mutex_lock(&lock);

        //sem_getvalue(&locked_value, &row);
        //sem_post(&locked_value);
        row = m_row++;
        pthread_mutex_unlock(&lock);

        if (row > matrix_2_size[1])
            break;

        for (int i = 0; i < matrix_1_size[0]; i++)
        {

            for(int j = 0; j < matrix_2_size[1]; j++)
            {
                matrix_result[row][i] = matrix_1[row][i] * matrix_2[j][i];

                if(matrix_result[row][i] == 1)
                    break;
            }
        }
    }

    return NULL;
}
