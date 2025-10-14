#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "tasks.h"

void initialise_tasklist(TaskList *log)
{
    log->capacity = MAX_TASKS;
    log->length = 0;
    log->tasks = malloc(log->capacity * sizeof(Task));

    if (log->tasks == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void free_tasklist(TaskList *log)
{
    free(log->tasks);
    log->tasks = NULL;
    log->capacity = 0;
    log->length = 0;
}

// Returns the number of tasks logged. 
void log_tasks(TaskList *log, FILE *todos)
{
    char line[MAX_LINE];

    // Add all current tasks from the text file into an array 
    while (fgets(line, sizeof(line), todos))
    {
        // Add each line to the log array
        if (strlen(line) > 1)
        {
            log->tasks[log->length++] = read_task(line);
        }
    }
}

Task read_task(char *line)
{
    Task t;

    sscanf(line, "id: %d name: \"%49[^\"]\" priority: %d date: %s status: %d", &t.id, t.name, &t.priority, t.date, &t.status);

    return t;
}

void print_tasks(TaskList log)
{
    for (int i = 0; i < log.length; i++)
    {
        printf("%3d %-20s %-10s %-11s %-10s\n", log.tasks[i].id, log.tasks[i].name, priority_name(log.tasks[i].priority), log.tasks[i].date, status_name(log.tasks[i].status));
    }
}

void rewrite_tasks(TaskList *log, FILE *todos)
{
    if (ftruncate(fileno(todos), 0) != 0)
    {
        fprintf(stderr, "Could not empty the file todos.\n");
        exit(1);
    }

    rewind(todos);

    // Now append all tasks one by one
    for (int i = 0; i < log->length; i++) {
        add_task(todos, log->tasks[i]);
    }
}

// A function to write new tasks into the file 
void add_task(FILE *tasks, Task task)
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
        case PRIORITY_LOW: return "Low";
        case PRIORITY_MEDIUM: return "Medium";
        case PRIORITY_HIGH: return "High";
    }
}

char *status_name(Status s)
{
    switch(s)
    {
        case STATUS_PENDING: return "Pending";
        case STATUS_COMPLETE: return "Complete";
    }
}

// Format the information given on the commmand line to form a new Task
Task format_new_task(TaskList log, char *argv[])
{
    Task task; 
    strcpy(task.name, argv[2]);
    task.priority = determine_priority(argv[3]);

    char *date = date_format(argv[4]);
    strcpy(task.date, date);
    free(date);

    if (argv[5] != NULL)
    {
        task.status = determine_status(argv[5]);
    }
    else 
    {
        task.status = STATUS_PENDING;
    }

    if (log.length == 0)
    {
        task.id = 1;
    }
    else
    {
        task.id  = log.length + 1;
    }

    return task;
}

void order_tasks(TaskList *log, Order order)
{
    switch(order)
    {
        case ORDER_ID:
        {
            // No reording needed 
            break;
        }
        case ORDER_NAME:
        {
            // order alphabetically 
            bool swapped;
            for(int i = 0; i < log->length - 1; i++)
            {
                swapped = false;
                for (int j = 0; j < log->length - i - 1; j++)
                {
                    char name1[50];
                    char name2[50];
                    strcpy(name1, log->tasks[j].name);
                    strcpy(name2, log->tasks[j + 1].name);
                    int result = strcmp(name1, name2);
                    if (result > 0)
                    {
                        Task tmp = log->tasks[j];
                        log->tasks[j] = log->tasks[j + 1];
                        log->tasks[j + 1] = tmp;
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
        case ORDER_PRIORITY:
        {
            // All high first, then medium, then low
            for (int i = 0; i < log->length; i++)
            {
                for (int j = 0; j < log->length - i - 1; j++)
                {
                    if (log->tasks[j].priority > log->tasks[j + 1].priority)
                    {
                        swap_tasks(&log->tasks[j], &log->tasks[j + 1]);
                    }
                }
            }
            break;
        }
        case ORDER_DATE:
        {
            // Order by the date due 
            // Order by year first, then month, then day
            bool swapped;
            for(int i = 0; i < log->length - 1; i++)
            {
                swapped = false;
                for (int j = 0; j < log->length - i - 1; j++)
                {
                    char *date1 = log->tasks[j].date;
                    char *date2 = log->tasks[j + 1].date;
                    int result = compare_dates(date1, date2);
                    if (result > 0)
                    {
                        Task tmp = log->tasks[j];
                        log->tasks[j] = log->tasks[j + 1];
                        log->tasks[j + 1] = tmp;
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
        case ORDER_STATUS:
        {
            for (int i = 0; i < log->length; i++)
            {
                for (int j = 0; j < log->length - i - 1; j++)
                {
                    if (log->tasks[j].status > log->tasks[j + 1].status)
                    {
                        swap_tasks(&log->tasks[j], &log->tasks[j + 1]);
                    }
                }
            }

            break;
        }
    }

    return;
}

void delete_task(TaskList *log, int id)
{
    for (int i = id - 1; i < log->length; i++)
    {
        log->tasks[i] = log->tasks[i + 1];
    }

    log->length--;
}

void update_task(TaskList *log, int id, Order order, char *update)
{
    switch(order)
    {
        case(ORDER_ID):
        {
            fprintf(stderr, "Task id cannot be updated.\n");
            exit(1);
        }
        case(ORDER_NAME):
        {
            strcpy(log->tasks[id - 1].name, update);
            break;
        }
        case(ORDER_PRIORITY):
        {
            Priority priority = determine_priority(update);
            log->tasks[id - 1].priority = priority;
            break;
        }
        case(ORDER_DATE):
        {
            char *date = date_format(update);
            strcpy(log->tasks[id - 1].date, date);
            break;
        }
        case(ORDER_STATUS):
        {
            Status status = determine_status(update);
            log->tasks[id - 1].status = status;
            break;
        }
    }
}

int compare_dates(char *date1, char *date2)
{
    int d1, m1, y1, d2, m2, y2;

    sscanf(date1, "%4d-%2d-%2d", &y1, &m1, &d1);
    sscanf(date2, "%4d-%2d-%2d", &y2, &m2, &d2);

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
        priority = PRIORITY_LOW;
    }
    else if (strcmp(input, "MEDIUM") == 0)
    {
        priority = PRIORITY_MEDIUM;
    }    
    else if (strcmp(input, "HIGH") == 0)
    {
        priority = PRIORITY_HIGH;
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
        status = STATUS_PENDING;
    }
    else if (strcmp(input, "COMPLETE") == 0)
    {
        status = STATUS_COMPLETE;
    }
    else 
    {
        fprintf(stderr, "Invalid status: %s\n", input);
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
        order = ORDER_ID;
    }
    else if (strcmp(input, "NAME") == 0)
    {
        order = ORDER_NAME;
    }    
    else if (strcmp(input, "PRIORITY") == 0)
    {
        order = ORDER_PRIORITY;
    }
    else if (strcmp(input, "DATE") == 0)
    {
        order = ORDER_DATE;
    }
    else if (strcmp(input, "STATUS") == 0)
    {
        order = ORDER_STATUS;
    }
    else
    {
        fprintf(stderr, "Invalid order: %s\n", input);
        exit(1);
    }

    return order;
}

Command determine_command(char *input)
{
    input = str_upper(input);
    Command command;

    if (strcmp(input, "ADD") == 0)
    {
        command = COMMAND_ADD;
    }
    else if (strcmp(input, "LIST") == 0)
    {
        command = COMMAND_LIST;
    }    
    else if (strcmp(input, "UPDATE") == 0)
    {
        command = COMMAND_UPDATE;
    }
    else if (strcmp(input, "REMOVE") == 0)
    {
        command = COMMAND_REMOVE;
    }
    else
    {
        fprintf(stderr, "Invalid command: %s\n", input);
        exit(1);
    }

    return command;
}

char *date_format(char *input)
{
    int day, month, year;
    char *formatted = malloc(11);

    // Try parsing with different separators
    if (sscanf(input, "%d-%d-%d", &year, &month, &day) == 3 ||
        sscanf(input, "%d/%d/%d", &year, &month, &day) == 3 ||
        sscanf(input, "%d.%d.%d", &year, &month, &day) == 3) 
    {
        
        // Check day and month are the correct way around
        if (day < 1 || day > 31 || month < 1 || month > 12) {
            fprintf(stderr, "Invalid date format. Use YYY-MM-DD, YYYY/MM/DD, or YYYY.MM.DD");
            exit(1);
        }

        snprintf(formatted, 11, "%04d-%02d-%02d", year, month, day);
    } 
    else 
    {
        fprintf(stderr, "Invalid date format. Use YYY-MM-DD, YYYY/MM/DD, or YYYY.MM.DD");
        exit(1);
    }

    return formatted;
}

void swap_tasks(Task *a, Task *b)
{
    Task temp = *a;
    *a = *b;
    *b = temp;
}

void print_usage(void)
{
    printf("Usage: ./checkaroo <cmd>\n");
    printf("cmd: add, remove, update, list\n");
    printf("add <name> <priortiy> <date> (optional: <status>) \n");
    printf("remove <id>\n");
    printf("update <field> <updated_value> <id>\n");
    printf("field: name, date, status, priority\n");
    printf("list (optional: <order>)\n");
    printf("order: id, name, date, status, priority\n");
}