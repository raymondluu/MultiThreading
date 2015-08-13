//exercise4.c

/* Program: Multi-threading class
   Purpose: Exercise 4
   File: exercise4.c
   Programmer: Raymond Luu
   Date: 10/28/2013
   Description: To learn how to program multi-threading.
*/

#include <stdio.h>
#include <pthread.h>

void* producer();
void* consumer();

pthread_mutex_t cr_mutex = PTHREAD_MUTEX_INITIALIZER; // critical region mutex
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    int num_e, total_leng, end_of_file;
} bundle;

/* Function: main
   Uses library: stdio.h, pthread.h
   Recieves: void
   Returns: int
*/

int main(void)
{
    bundle the_info = {0, 0, 0};
    
    pthread_t prod_thread;
    pthread_t cons_thread;
    
    pthread_mutex_lock(&read_mutex);
    // for explicit reason even though initally mutex is unlocked
    pthread_mutex_unlock(&write_mutex);
    pthread_mutex_unlock(&cr_mutex);
    
    int prod_thread_ret = pthread_create(&prod_thread, NULL, producer, &the_info);
    int cons_thread_ret = pthread_create(&cons_thread, NULL, consumer, &the_info);
    
    if(prod_thread_ret != 0)
    {
        printf("Thread fail to create! Error: %d", prod_thread_ret);
        return 1;
    }
    if(cons_thread_ret != 0)
    {
        printf("Thread fail to create! Error: %d", cons_thread_ret);
        return 1;
    }
    
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    
    pthread_exit(NULL);
    
    return 0;
}

/* Function: Producer thread
   Uses library: stdio.h, pthread.h
   Recieves: void* parameter
   Returns: void*
*/
void* producer(bundle *the_info)
{
	int i, cur_char, temp_e, temp_leng = 0;
    FILE *my_file;

    my_file = fopen("randStrings.txt", "r");
    
    cur_char = fgetc(my_file);
    
    // produce product
    while (the_info->end_of_file != 1)
    {  
        
        while (cur_char != 10)
        {
            if (cur_char == 101)
            {
                temp_e++;
                temp_leng++;
                //printf("%c", cur_char);
            }
            else if (cur_char != 10 && cur_char != 13)
            {   
                temp_leng++;
                //printf("%c", cur_char);
            }
            //printf("%c", cur_char);
            cur_char = fgetc(my_file);
        }
        
        // lock write
        pthread_mutex_lock(&write_mutex);
        
        // lock critical region
        pthread_mutex_lock(&cr_mutex);
        
        // write to shared variable
        the_info->num_e = temp_e;
        the_info->total_leng = temp_leng;
        
        // unlock critical region
        pthread_mutex_unlock(&cr_mutex);
        
        // unlock read
        pthread_mutex_unlock(&read_mutex);
        
        // clear variables for next line
        temp_e = 0;
        temp_leng = 0;
        cur_char = fgetc(my_file);
        
        // check for end of file
        if (cur_char == EOF)
        {
            the_info->end_of_file = 1;
        }
    }
    
    return 0;
}

/* Function: Consumer thread
   Uses library: stdio.h, pthread.h
   Recieves: void* parameter
   Returns: void*
*/
void* consumer(bundle *the_info)
{   
    int i, temp_e, temp_leng = 0;
    
    FILE *my_file;

    my_file = fopen("resultStrings.txt", "w");
    
    while (the_info->end_of_file != 1)
    {
    
        // lock read
        pthread_mutex_lock(&read_mutex);
        
        // lock critical region
        pthread_mutex_lock(&cr_mutex);
        
        // read shared variable
        temp_e = the_info->num_e;
        temp_leng = the_info->total_leng;
        
        // unlock critical region
        pthread_mutex_unlock(&cr_mutex);
        
        // unlock write
        pthread_mutex_unlock(&write_mutex);
        
        // consume product
        if (temp_e == 0)
        {
            for (i = 0; i < temp_leng; i++)
            {
                //printf("-");
                fputc(45, my_file);
            }
            //printf("\n");
            fputc(13, my_file);
            fputc(10, my_file);
        }
        else
        {
            for (i = 0; i < temp_e; i++)
            {
                //printf("e");
                fputc(101, my_file);
            }
            //printf("\n");
            fputc(13, my_file);
            fputc(10, my_file);
        }
    }
    
    return 0;
}