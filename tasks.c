#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "tasks.h"

void initialise_tasklist(TaskList *log)
{
    log->capacity = MAX_TASKS;
    log->length = 0;
    log->tasks = malloc(log->capacity * sizeof(Task));

    if (log->tasks == NULL)
    {
        fprintf(stderr, "Error: could not allocate memory for task list\n");
        exit(1);
    }
}

void free_tasklist(TaskList *log)
{
    if (log == NULL)
    {
        return;
    }
    free(log->tasks);
    log->tasks = NULL;
    log->capacity = 0;
    log->length = 0;
}

void load_tasks(TaskList *log, FILE *todos)
{
    char line[MAX_LINE];

    // Add all current tasks from the text file into TaskList
    while (fgets(line, sizeof(line), todos))
    {
        if (log->length >= log->capacity)
        {
            free_tasklist(log);
            fprintf(stderr, "Error: max tasks reached.\n");
            break;
        }
        // Add each line to the TaskList
        if (strlen(line) > 1)
        {
            log->tasks[log->length++] = line_to_task(line);
        }
    }

    if (!feof(todos))
    {
        free_tasklist(log);
        fclose(todos);
        fprintf(stderr, "Error: todos file did not end as expected.\n");
        exit(1);
    }
    if (ferror(todos))
    {
        free_tasklist(log);
        fclose(todos);
        fprintf(stderr, "Error: reading from todos file failed.\n");
        exit(1);
    }
}

Task line_to_task(char *line)
{
    Task t;

    if (sscanf(line, "id: %d name: \"%49[^\"]\" priority: %d date: %s status: %d", &t.id, t.name,(int *)&t.priority, t.date, (int *)&t.status) != 5)
    {
        fprintf(stderr, "Error: unable to add task to log.\n");
        exit(1);
    }

    return t;
}

void print_tasks(TaskList log)
{
    for (int i = 0; i < log.length; i++)
    {
        printf("%3d %-20s %-10s %-11s %-10s\n", log.tasks[i].id, log.tasks[i].name, priority_name(log.tasks[i].priority), log.tasks[i].date, status_name(log.tasks[i].status));
    }
}

void save_tasks(TaskList *log, FILE *todos)
{
    if (ftruncate(fileno(todos), 0) != 0)
    {
        free_tasklist(log);
        fclose(todos);
        fprintf(stderr, "Error: could not delete contents of todo file.\n");
        exit(1);
    }

    rewind(todos);

    // Now append all tasks one by one
    for (int i = 0; i < log->length; i++) {
        add_task(todos, log->tasks[i]);
    }
}

void add_task(FILE *tasks, Task task)
{
    if (fprintf(tasks, "id: %d ", task.id) < 0 ||
        fprintf(tasks, "name: \"%s\" ", task.name) < 0 ||
        fprintf(tasks, "priority: %d ", task.priority) < 0 ||
        fprintf(tasks, "date: %s ", task.date) < 0 ||
        fprintf(tasks, "status: %d \n", task.status) < 0)
    {
        fclose(tasks);
        fprintf(stderr, "Error: unable to add task to file.\n");
        exit(1);
    }

    return;
}

char *priority_name(Priority p)
{
    switch(p)
    {
        case PRIORITY_LOW: return "Low";
        case PRIORITY_MEDIUM: return "Medium";
        case PRIORITY_HIGH: return "High";
        default:
        {
            fprintf(stderr, "Error: priority value is not valid.\n Priority: low, medium or high.\n");
            exit(1);
        }
    }
}

char *status_name(Status s)
{
    switch(s)
    {
        case STATUS_PENDING: return "Pending";
        case STATUS_COMPLETE: return "Complete";
        default:
        {
            fprintf(stderr, "Error: status value is not valid.\n Status: pending or complete.\n");
            exit(1);
        }
    }
}

Task parse_task(TaskList log, char *argv[])
{
    Task task;
    if (strlen(argv[2]) > 50)
    {
        free_tasklist(&log);
        fprintf(stderr, "Error: name is too long. Name must be less than 50 characters.\n");
        exit(1);
    }
    strcpy(task.name, argv[2]);
    task.priority = determine_priority(argv[3]);

    if (strlen(argv[4]) > 11)
    {
        free_tasklist(&log);
        fprintf(stderr, "Error: invalid date format. Use YYYY-MM-DD, YYYY/MM/DD, or YYYY.MM.DD");
    }
    char date[11];
    format_date(argv[4], date);
    strcpy(task.date, date);

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
            // Order alphabetically
            bool swapped;
            for(int i = 0; i < log->length - 1; i++)
            {
                swapped = false;
                for (int j = 0; j < log->length - i - 1; j++)
                {
                    char *name1 = log->tasks[j].name;
                    char *name2 = log->tasks[j + 1].name;
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
        default:
        {
            free_tasklist(log);
            fprintf(stderr, "Error: order value is not valid.\n Order: id, name, priority, date or status.\n");
            exit(1);
        }
    }

    return;
}

void delete_task(TaskList *log, int id)
{
    if (id < 1 || id > log->length)
    {
        free_tasklist(log);
        fprintf(stderr, "Error: %d id is not valid.\n", id);
        exit(1);
    }
    memmove(&log->tasks[id - 1], &log->tasks[id], (log->length - id) * sizeof(Task));

    log->length--;
}

void update_task(TaskList *log, int id, Order order, char *update)
{
    switch(order)
    {
        case(ORDER_ID):
        {
            free_tasklist(log);
            fprintf(stderr, "Error: task ID update not supported.\n");
            exit(1);
        }
        case(ORDER_NAME):
        {
            if (strlen(update) > 50)
            {
                free_tasklist(log);
                fprintf(stderr, "Error: new name is too long, must be less than 50 characters.\n");
                exit(1);
            }
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
            if (strlen(update) > 11)
            {
                free_tasklist(log);
                fprintf(stderr, "Error: invalid date format. Use YYYY-MM-DD, YYYY/MM/DD, or YYYY.MM.DD");
                exit(1);
            }
            char date[11];
            format_date(update, date);
            strcpy(log->tasks[id - 1].date, date);
            break;
        }
        case(ORDER_STATUS):
        {
            Status status = determine_status(update);
            log->tasks[id - 1].status = status;
            break;
        }
        default:
        {
            free_tasklist(log);
            fprintf(stderr, "Error: order value is not valid\n");
            exit(1);
        }
    }
}

int compare_dates(char *date1, char *date2)
{
    int d1, m1, y1, d2, m2, y2;

    if (sscanf(date1, "%4d-%2d-%2d", &y1, &m1, &d1) == 3 &&
        sscanf(date2, "%4d-%2d-%2d", &y2, &m2, &d2) == 3)
        {
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
    else
    {
        fprintf(stderr, "Error: unable to compare dates. Re-format dates using YYYY-MM-DD, YYYY/MM/DD, or YYYY.MM.DD\n");
        exit(1);
    }

}

void str_upper(char *input)
{
    for (char *c = input; *c; c++)
    {
        *c = toupper((unsigned char) *c);
    }
}

// Returns the priority that the string corresponds to
Priority determine_priority(char *input)
{
    str_upper(input);
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
        fprintf(stderr, "Error: invalid priority: %s\n", input);
        exit(1);
    }

    return priority;
}

Status determine_status(char *input)
{
    str_upper(input);
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
        fprintf(stderr, "Error: invalid status: %s\n", input);
        exit(1);
    }

    return status;
}

Order determine_order(char *input)
{
    str_upper(input);
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
        fprintf(stderr, "Error: invalid order: %s\n", input);
        exit(1);
    }

    return order;
}

Command determine_command(char *input)
{
    str_upper(input);
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
        fprintf(stderr, "Error: invalid command: %s\n", input);
        print_usage();
        exit(1);
    }

    return command;
}

void format_date(char *input, char *date)
{
    int day, month, year;

    // Try parsing with different separators
    if (sscanf(input, "%d-%d-%d", &year, &month, &day) == 3 ||
        sscanf(input, "%d/%d/%d", &year, &month, &day) == 3 ||
        sscanf(input, "%d.%d.%d", &year, &month, &day) == 3)
    {
        // Validate year
        if (year < 1000 || year > 9999)
        {
            fprintf(stderr, "Error: year must be 4 digits (YYYY)\n");
            exit(1);
        }

        // Validate month
        if (month < 1 || month > 12)
        {
            fprintf(stderr, "Error: month must be between 1 and 12\n");
            exit(1);
        }

        // Determine max days in the month
        int max_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        // Handle leap year for February
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            max_days[1] = 29;
        }

        // Validate day
        if (day < 1 || day > max_days[month - 1])
        {
            fprintf(stderr, "Error: day %d is invalid for month %02d\n", day, month);
            exit(1);
        }

        // Format into YYYY-MM-DD
        snprintf(date, 11, "%04d-%02d-%02d", year, month, day);
    }
    else
    {
        fprintf(stderr, "Error: invalid date format. Use YYYY-MM-DD, YYYY/MM/DD, or YYYY.MM.DD\n");
        exit(1);
    }
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
    printf("add <name> <priority> <date> (optional: <status>) \n");
    printf("remove <id>\n");
    printf("update <field> <updated_value> <id>\n");
    printf("field: name, date, status, priority\n");
    printf("list (optional: <order>)\n");
    printf("order: id, name, date, status, priority\n");
}
