#ifndef INCLUDE_TASKS_H
#define INCLUDE_TASKS_H

#include <stdio.h>

#define MAX_TASKS 100
#define MAX_LINE 200

typedef enum {
  PRIORITY_HIGH, 
  PRIORITY_MEDIUM,
  PRIORITY_LOW,
}Priority;

typedef enum {
    STATUS_PENDING,
    STATUS_COMPLETE,
}Status;

typedef enum {
    ORDER_ID,
    ORDER_NAME,
    ORDER_PRIORITY,
    ORDER_DATE,
    ORDER_STATUS,
}Order;

typedef enum {
    COMMAND_ADD,
    COMMAND_LIST,
    COMMAND_UPDATE,
    COMMAND_REMOVE,
}Command;

typedef struct {
    int id;
    char name[50];
    Priority priority;
    char date[11];    
    Status status;
}Task;

typedef struct {
    Task *tasks;
    int capacity;
    int length;
}TaskList;

// Initialise the TaskList, setting the values
void initialise_tasklist(TaskList *log);

// Frees the TaskList
void free_tasklist(TaskList *log);

// Loads the tasks from the file to the TaskList
void load_tasks(TaskList *log, FILE *todos);

// Retreives one task from a line in the file
Task line_to_task(char *line);  

// Formats and prints tasks
void print_tasks(TaskList log);

// Truncates the text file and fill it using the updated Tasklist 
void save_tasks(TaskList *log, FILE *todos);

// Write a new task into the file 
void add_task(FILE *tasks, Task task);

// returns the string which corresponds to the priority type
char *priority_name(Priority p);

// Returns the string which corresponds to the status type
char *status_name(Status s);

// Parse the information given on the commmand line to form a new Task
Task parse_task(TaskList log, char *argv[]);

// Changes the order of the tasks in the TaskList dependent on the order
void order_tasks(TaskList *log, Order order);

// Removes a task from the TaskList 
void delete_task(TaskList *log, int id);

// Changes the task in TaskList that needs updating 
void update_task(TaskList *log, int id, Order order, char *update);

// Determines which date is larger
int compare_dates(char *date1, char *date2);

// Changes a string to uppercase 
char *str_upper(char *input);

// Returns the priority that the string corresponds to 
Priority determine_priority(char *input);

// Returns the status that the string corresponds to 
Status determine_status(char *input);

// Returns the order that the string corresponds to 
Order determine_order(char *input);

// Returns the command that the string corresponds to
Command determine_command(char *input);

// Formats the date so they are uniform in the text file 
void format_date(char *input, char *date);

// Swaps two tasks, in order to change the ordering of the TaskList
void swap_tasks(Task *a, Task *b);

// Prints the usage required for the application 
void print_usage(void);

#endif /* INCLUDE_TASKS_H */