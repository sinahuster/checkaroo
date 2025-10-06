#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_TASKS 100
#define MAX_LINE 200

typedef enum {
  LOW,
  MEDIUM,
  HIGH
}Priority;

typedef enum {
    PENDING,
    COMPLETE
}Status;

typedef struct 
{
    int id;
    char name[50];
    Priority priority;
    char date[11];    
    Status status;
} Task;

int log_tasks(FILE *tasks, Task log[]);
Task read_task(char *line);  
void print_task(Task t);
void rewrite_tasks(Task log[], int logSize, FILE *tasks);
void add_task(Task task, FILE *tasks);
char *priority_name(Priority p);
char *status_name(Status s);

// Returns the number of tasks logged. 
int log_tasks(FILE *tasks, Task log[])
{
    char line[MAX_LINE];
    int num = 0;

    // Add all current tasks from the text file into an array 
    while (fgets(line, sizeof(line), tasks))
    {
        // Add each line to the log array
        if (strlen(line) > 1)
        {
            log[num++] = read_task(line);
        }
    }

    return num;
}

Task read_task(char *line)
{
    Task t;

    sscanf(line, "id: %d name: %49[^0-9] priority: %s date: %s status: %s", &t.id, t.name, t.priority, t.date, t.status);

    return t;
}

void print_task(Task t)
{
    printf("%3d %50s %10s %11s %10s\n", t.id, t.name, priority_name(t.priority), t.date, t.status);
}

void rewrite_tasks(Task log[], int logSize, FILE *tasks)
{
    if (ftruncate(tasks, 0) != 0)
    {
        perror("ftruncate() error");
    }

    rewind(tasks);

    // Now append all tasks one by one
    for (int i = 0; i < logSize; i++) {
        add_task(log[i], tasks);
    }
}

// A function to write new tasks into the file 
void add_task(Task task, FILE *tasks)
{
    fprintf(tasks, "id: %d ", task.id);
    fprintf(tasks, "name: %s ", task.name);
    fprintf(tasks, "priority: %s ", task.priority);
    fprintf(tasks, "date: %s ", task.date);
    fprintf(tasks, "status: %s \n", task.status);

    return;
}

char *priority_name(Priority p)
{
    switch(p)
    {
        case LOW: return "Low";
        case MEDIUM: return "Medium";
        case HIGH: return "High";
    }
}

char *status_name(Status s)
{
    switch(s)
    {
        case PENDING: return "Pending";
        case COMPLETE: return "Complete";
    }
}