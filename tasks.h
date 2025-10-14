#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TASKS 100
#define MAX_LINE 200

typedef enum {
  PRIORITY_HIGH, 
  PRIORITY_MEDIUM,
  PRIORITY_LOW, 
}Priority;

typedef enum {
    STATUS_PENDING,
    STATUS_COMPLETE
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

void initialise_tasklist(TaskList *log);
void free_tasklist(TaskList *log);
void log_tasks(TaskList *log, FILE *todos);
Task read_task(char *line);  
void print_tasks(TaskList log);
void rewrite_tasks(TaskList *log, FILE *todos);
void add_task(FILE *tasks, Task task);
char *priority_name(Priority p);
char *status_name(Status s);
Task format_new_task(TaskList log, char *argv[]);
void order_tasks(TaskList *log, Order order);
void delete_task(TaskList *log, int id);
void update_task(TaskList *log, int id, Order order, char *update);
int compare_dates(char *date1, char *date2);
char *str_upper(char *input);
Priority determine_priority(char *input);
Status determine_status(char *input);
Order determine_order(char *input);
Command determine_command(char *input);
char *date_format(char *input);
void swap_tasks(Task *a, Task *b);
void print_usage(void);