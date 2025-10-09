#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

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
int compare_dates(char *date1, char *date2);
char *str_upper(char *input);
Priority determine_priority(char *input);
Status determine_status(char *input);
Order determine_order(char *input);
char *format_date(char *input);
 
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

    sscanf(line, "id: %d name: \"%49[^\"]\" priority: %d date: %s status: %d", &t.id, t.name, &t.priority, t.date, &t.status);

    return t;
}

void print_task(Task t)
{
    printf("%3d %-20s %-10s %-11s %-10s\n", t.id, t.name, priority_name(t.priority), t.date, status_name(t.status));
}

void rewrite_tasks(Task log[], int logSize, FILE *tasks)
{
    if (ftruncate(fileno(tasks), 0) != 0)
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
    fprintf(tasks, "name: \"%s\" ", task.name);
    fprintf(tasks, "priority: %d ", task.priority);
    fprintf(tasks, "date: %s ", task.date);
    fprintf(tasks, "status: %d \n", task.status);

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

// Format the information given on the commmand line to form a new Task
Task format_new_task(char *argv[], int num)
{
    Task task; 
    strcpy(task.name, argv[2]);
    task.priority = determine_priority(argv[3]);

    char *date = format_date(argv[4]);
    strcpy(task.date, date);
    free(date);

    if (argv[5] != NULL)
    {
        task.status = determine_status(argv[5]);
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

void order_tasks(Task log[], Order order, int num)
{
    switch(order)
    {
        case ID:
        {
            // No reording needed 
            break;
        }
        case NAME:
        {
            // order alphabetically 
            bool swapped;
            for(int i = 0; i < num - 1; i++)
            {
                swapped = false;
                for (int j = 0; j < num - i - 1; j++)
                {
                    char name1[50];
                    char name2[50];
                    strcpy(name1, log[j].name);
                    strcpy(name2, log[j + 1].name);
                    int result = strcmp(name1, name2);
                    if (result > 0)
                    {
                        Task tmp = log[j];
                        log[j] = log[j + 1];
                        log[j + 1] = tmp;
                        swapped = true;
                    }
                }
                if (swapped == false)
                {
                    break;
                }
            }
            break;
    
        }
        case PRIORITY:
        {
            // All high first, then medium, then low
            Task *sorted = malloc(sizeof(Task) * num);
            int index = 0;

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
            // Order by year first, then month, then day
            bool swapped;
            for(int i = 0; i < num - 1; i++)
            {
                swapped = false;
                for (int j = 0; j < num - i - 1; j++)
                {
                    char *date1 = log[j].date;
                    char *date2 = log[j + 1].date;
                    int result = compare_dates(date1, date2);
                    if (result < 0)
                    {
                        Task tmp = log[j];
                        log[j] = log[j + 1];
                        log[j + 1] = tmp;
                        swapped = true;
                    }
                }
                if (swapped == false)
                {
                    break;
                }

            }
            break;
        }
        case STATUS:
        {
            Task *sorted = malloc(sizeof(Task) * num);
            int index = 0;
            // Order pending first, then complete
            for (int s = 1; s <= 2; s++)
            {
                for (int i = 0; i < num; i++)
                {
                    if (log[i].status == s)
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
    }

    for (int i = 0; i < num; i++)
    {
        print_task(log[i]);
    }

    return;
}


int compare_dates(char *date1, char *date2)
{
    int d1, m1, y1, d2, m2, y2;

    sscanf(date1, "%2d-%2d-%4d", &d1, &m1, &y1);
    sscanf(date2, "%2d-%2d-%4d", &d2, &m2, &y2);

    if (y1 != y2)
    {
        return y1 - y2;
    }
    if (m1 != m2)
    {
        return m1 - m2;
    }
    return d1 - d2;
}

char *str_upper(char *input)
{
    for (char *c = input; *c; c++)
    {
        *c = toupper((unsigned char) *c);
    }
    return input;
}


Priority determine_priority(char *input)
{
    input = str_upper(input);
    Priority priority;

    if (strcmp(input, "LOW") == 0)
    {
        priority = LOW;
    }
    else if (strcmp(input, "MEDIUM") == 0)
    {
        priority = MEDIUM;
    }    
    else if (strcmp(input, "HIGH") == 0)
    {
        priority = HIGH;
    }
    else
    {
        fprintf(stderr, "Invalid priority: %s\n", input);
        exit(1);
    }

    return priority;
}

Status determine_status(char *input)
{
    input = str_upper(input);
    Status status;

    if (strcmp(input, "PENDING") == 0)
    {
        status = PENDING;
    }
    else if (strcmp(input, "COMPLETE") == 0)
    {
        status = COMPLETE;
    }
    else 
    {
        fprintf(stderr, "Invalid status %s\n", input);
        exit(1);
    }

    return status;
}

Order determine_order(char *input)
{
    input = str_upper(input);
    Order order;

    if (strcmp(input, "ID") == 0)
    {
        order = ID;
    }
    else if (strcmp(input, "NAME") == 0)
    {
        order = NAME;
    }    
    else if (strcmp(input, "PRIORITY") == 0)
    {
        order = PRIORITY;
    }
    else if (strcmp(input, "DATE") == 0)
    {
        order = DATE;
    }
    else if (strcmp(input, "STATUS") == 0)
    {
        order = STATUS;
    }
    else
    {
        fprintf(stderr, "Invalid order: %s\n", input);
        exit(1);
    }

    return order;
}

char *format_date(char *input)
{
    int day, month, year;
    char *formatted = malloc(11);

    // Try parsing with different separators
    if (sscanf(input, "%d-%d-%d", &day, &month, &year) == 3 ||
        sscanf(input, "%d/%d/%d", &day, &month, &year) == 3 ||
        sscanf(input, "%d.%d.%d", &day, &month, &year) == 3) 
    {
        
        // Check day and month are the correct way around
        if (day < 1 || day > 31 || month < 1 || month > 12) {
            perror("Invalid day or month.");
        }

        snprintf(formatted, 11, "%02d-%02d-%04d", day, month, year);
    } 
    else 
    {
        perror("Invalid date format. Use DD-MM-YYYY, DD/MM/YYYY, or DD.MM.YYYY");
    }

    return formatted;
}