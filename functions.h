#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_TASKS 100
#define MAX_LINE 200

typedef enum {
  HIGH,
  MEDIUM,
  LOW
}Priority;

typedef enum {
    PENDING,
    COMPLETE
}Status;

typedef enum {
    ID,
    NAME,
    PRIORITY,
    DATE,
    STATUS,
}Order;

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
Task format_new_task(char *argv[], int num);
void order_tasks(Task log[], Order order, int num);

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

Task format_new_task(char *argv[], int num)
{
    Task task; 
    strcpy(task.name, argv[2]);
    strcpy(task.date, argv[4]);

    task.priority = strupr(argv[3]);
    
    if (argv[5] != '\0')
    {
        task.status = strupr(argv[5]);
    }
    else 
    {
        task.status = PENDING;
    }

    if (num == 0)
    {
        task.id = 1;
    }
    else
    {
        task.id  = num + 1;
    }

    return task;
}

void order_tasks(Task log[], Order o, int num)
{
    switch(o)
    {
        case ID:
        {
            // No reording needed 
            break;
        }
        Task *sorted = malloc(sizeof(Task) * num);
        int index = 0;
        case NAME:
        {
            // order alphabetically 
            break;
        }
        case PRIORITY:
        {
            // All high first, then medium, then low
            for (int p = 1; p <= 3; p++)
            {
                for (int i = 0; i < num; i++)
                {
                    if (log[i].priority == p)
                    {
                        sorted[index++] = log[i];
                    }
                }
            }
            
            for (int i = 0; i < num; i++)
            {
                log[i] = sorted[i];
            }

            free(sorted);
            break;
        }
        case DATE:
        {
            // Order by the date due 
            break;
        }
        case STATUS:
        {
            // Order pending first, then complete
            break;
        }
    }

    for (int i = 0; i < num; i++)
    {
        print_task(log[i]);
    }

    return;
}