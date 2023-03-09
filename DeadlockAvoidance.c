// James Miller
// March 5

// ProcessID Random Size Resource

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// Define the number of processes and wait time.
#define NUM_PROCESSES 3
#define RESOURCE_MAX_WAIT_TIME 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int resource = 0;
FILE *activityFile;

void *process(void *arg)
{
    int id = *(int *)arg;
    bool running = true;
    unsigned int seed = time(NULL) + id; // Initialize random seed for this thread.

    while (running)
    {
        // Try to acquire resource.
        pthread_mutex_lock(&mutex);
        if (resource == 0)
        {
            resource = id;
            fprintf(activityFile, "Process %d acquired resource\n", id);
            printf("ProcessID: %d | ResourceID: %d | Value: %d \n", id, id, seed);
        }
        else
        {
            fprintf(activityFile, "Process %d is waiting for resource\n", id);
            printf("ProcessID: %d | ResourceID: %d | Waiting\n", id, id);
            pthread_mutex_unlock(&mutex);
            sleep(rand_r(&seed) % RESOURCE_MAX_WAIT_TIME + 1);
            continue;
        }
        pthread_mutex_unlock(&mutex);

        // Use resource
        sleep(rand_r(&seed) % 3 + 1);

        // Release the resource.
        pthread_mutex_lock(&mutex);
        resource = 0;

        fprintf(activityFile, "Process %d released resource\n", id);
        printf("ProcessID: %d | ResourceID: %d | Value: 0 \n", id, id);
        pthread_mutex_unlock(&mutex);

        // Set exit condition.
        running = false;
    }

    return NULL;
}

int main()
{
    printf("Opening activity log. Avoiding deadlock process. \n");

    // Initialize the random number generator.
    srand(time(NULL));

    activityFile = fopen("/Users/jamesmiller/Desktop/School/Operating Systems/DeadlockAvoidance/activityLog.txt", "a");
    if (activityFile == NULL)
    {
        printf("Error opening the activity log file.");
        return 1;
    }

    // Create processes.
    pthread_t threads[NUM_PROCESSES];
    int ids[NUM_PROCESSES];

    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        ids[i] = i + 1; // Initialize the ids array.
        pthread_create(&threads[i], NULL, process, &ids[i]);
    }

    // Wait for processes to finish.
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        pthread_join(threads[i], NULL);
    }

    fclose(activityFile);

    return 0;
}
