/*
A to-do list problem
The user will be able to add, complete or delete tasks, as well as getting a list of the to-dos. 
Each task has an id, name, priority level, date due and status.  
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TASKS 100
#define MAX_LINE 200

typedef struct 
{
    int id;
    char name[20];
    char priority[8]; // TODO: how could we do this? look up enum
    char date[11];    
    char status[10];  // TODO: see above priority
} Task;

void add_task(Task variable);
Task read_task(char *line);   // TODO: naming
void print_task(Task t);
void rewrite_tasks(Task log[], int logSize);
// TODO: maybe read_tasks

int main(int argc, char *argv[])
{
    Task log[MAX_TASKS];
    int num = 0;
    char line[MAX_LINE];

    // Check if any arguements except the program name is given. 
    if (argc == 1)
    {
        printf("Please input value\n");
        return 0;
    }

    // Open a file for the to-do list 
    FILE *tasks = fopen("tasks.txt", "a+");
    if (tasks == NULL)
    {
        printf("Error opening the file\n");
        return 1;
    }

    // Move pointer back to start for reading
    rewind(tasks);

    // Add all current tasks from the text file into an array 
    while (fgets(line, sizeof(line), tasks))
    {
        // Add each line to the log array
        if (strlen(line) > 1)
        {
            log[num++] = read_task(line);
        }
    }
    fclose(tasks);

    // Change the command to lowercase to ignore case sensitivity 
    for (int i = 0; i < strlen(argv[1]); i++) 
    {
        argv[1][i] = tolower(argv[1][i]);
    }

    // If command list is given, read and print the contents in the order specified
    if (strcmp(argv[1], "list") == 0)
    {
        if (num == 0)
        {
            printf("There are currently no to-dos\n");
            return 0;
        }
        printf("\033[1m%3s %20s %10s %11s %10s\033[0m\n", "id", "name", "priority", "date", "status");
        for (int i = 0; i < num; i++)
        {
            print_task(log[i]);
        }
    }

    // If add given, add the new to-do to the file 
    if (strcmp(argv[1], "add") == 0)
    {
        // Check all information that's required is present 
        if (argc < 5)
        {
            printf("usage: %s add <name> <priortiy> <date> (optional: <status>) \n", argv[0]);
            return 1;
        }

        // Define a new variable and asign the given variables
        Task var;
    
        strcpy(var.name, argv[2]);
        strcpy(var.priority, argv[3]);
        strcpy(var.date, argv[4]);
        
        // Check if status is given, else assume pending
        if (argc == 6)
        {
            strcpy(var.status, argv[5]);
        }
        else 
        {
            strcpy(var.status, "pending");
        }

        // Find the id of the last task in order to assign the id of the current task
        if (num == 0)
        {
            var.id = 1;
        }
        else 
        {
            var.id = log[num - 1].id + 1;
        }

        // Write the new task into the text file 
        add_task(var);
    }

    // If delete given, remove that to-do from the file
    if (strcmp(argv[1], "delete") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: ./checkaroo delete <task_id>\n");
            return 1;
        }

        int id = atoi(argv[2]);
        int found = 0;

        for (int i = 0; i < num; i++) {
            if (log[i].id == id) {
                // Shift all tasks after i to the left
                for (int j = i; j < num - 1; j++) {
                    log[j] = log[j + 1];
                }
                num--;
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Task ID %d not found.\n", id);
            return 1;
        }

        rewrite_tasks(log, num);
    }
    // If complete given, change the status of the to-do from working to complete
    if (strcmp(argv[1], "complete") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: ./checkaroo complete <task_id>\n");
        }

        int id = atoi(argv[2]);

        int found = 0;
        for (int i = 0; i < num; i++) {
            if (log[i].id == id) {
                strcpy(log[i].status, "complete");
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Task ID %d not found.\n", id);
            return 1;
        }

        rewrite_tasks(log, num);
    }

    return 0;
}

// A function to write new tasks into the file 
void add_task(Task variable)
{
    // Open a file for the to-do list 
    FILE *tasks = fopen("tasks.txt", "a");
    if (tasks == NULL)
    {
        printf("Error opening the file\n");
        return;
    }

    fprintf(tasks, "id:%d ", variable.id);
    fprintf(tasks, "name:%s ", variable.name);
    fprintf(tasks, "priority:%s ", variable.priority);
    fprintf(tasks, "date:%s ", variable.date);
    fprintf(tasks, "status:%s \n", variable.status);

    fclose(tasks);
}

Task read_task(char *line)
{
    Task t;

    sscanf(line, "id:%d name:%s priority:%s date:%s status:%s", &t.id, t.name, t.priority, t.date, t.status);

    return t;
}

void print_task(Task t)
{
    printf("%3d %20s %10s %11s %10s\n", t.id, t.name, t.priority, t.date, t.status);
}

void rewrite_tasks(Task log[], int logSize)
{
    // First clear the file
    FILE *tasks = fopen("tasks.txt", "w");
    if (!tasks) {
        perror("Error opening tasks.txt");
        return;
    }
    fclose(tasks); // Close right away, just to reset/empty the file

    // Now append all tasks one by one
    for (int i = 0; i < logSize; i++) {
        add_task(log[i]);
    }
}