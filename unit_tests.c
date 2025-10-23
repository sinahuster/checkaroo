#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tasks.h"

Task create_task(int id, const char *date, const char *name, int priority, int status);
FILE *create_temp_file(const char *filename);
void test_add_task();
void test_update_task();
void test_delete_task();
void test_add_task_full();

void test_order_tasks();

int main() {
    test_add_task();
    test_add_task_full();
    test_update_task();
    test_delete_task();
    test_order_tasks();

    printf("All tests passed!\n");
    return 0;
}

// Create a Task
Task create_task(int id, const char *date, const char *name, int priority, int status) 
{
    Task task;
    task.id = id;
    strcpy(task.date, date);
    strcpy(task.name, name);
    task.priority = priority;
    task.status = status;
    return task;
}

// Add a task
void test_add_task() 
{
    TaskList log;
    initialise_tasklist(&log);

    FILE *todos = tmpfile();

    Task task = create_task(1, "2025-10-20", "Laundry", 1, 0);
    add_task(todos, task);
    rewind(todos);  // move file pointer to start
    load_tasks(&log, todos);

    assert(log.length == 1);
    assert(strcmp(log.tasks[0].name, "Laundry") == 0);

    fclose(todos);
    printf("test_add_task passed\n");
}

// Add tasks until full
void test_add_task_full() {
    TaskList log;
    initialise_tasklist(&log);

    FILE *todos = tmpfile();

    // Add tasks up to capacity
    for (int i = 0; i < 100; i++) {
        Task task = create_task(i + 1, "2025-10-20", "Task", 1, 0);
        add_task(todos, task);
    }

    rewind(todos);
    load_tasks(&log, todos);
    assert(log.length == 100);

    // Attempt to add 101st task
    Task extra = create_task(101, "2025-10-21", "Extra Task", 1, 0);
    if (log.length < log.capacity) {
        add_task(todos, extra);
        log.length++;  // keep in sync manually
    }
    assert(log.length == 100);  // should remain 100

    fclose(todos);
    printf("test_add_task_full passed\n");
}

// Update a task
void test_update_task() {
    TaskList log;
    initialise_tasklist(&log);

    FILE *todos = tmpfile();
    Task task = create_task(1, "2025-10-20", "Washing", 1, 0);
    add_task(todos, task);
    rewind(todos);
    load_tasks(&log, todos);

    update_task(&log, 1, ORDER_NAME, "Dishes");
    assert(strcmp(log.tasks[0].name, "Dishes") == 0);

    fclose(todos);
    printf("test_update_task passed\n");
}

// Delete a task
void test_delete_task() {
    TaskList log;
    initialise_tasklist(&log);

    FILE *todos = tmpfile();
    Task task_1 = create_task(1, "2025-10-20", "Washing", 1, 0);
    Task task_2 = create_task(2, "2025-10-21", "Dishes", 1, 0);
    add_task(todos, task_1);
    add_task(todos, task_2);
    rewind(todos);
    load_tasks(&log, todos);

    delete_task(&log, 1);
    assert(log.length == 1);
    assert(log.tasks[0].id == 2);

    fclose(todos);
    printf("test_delete_task passed\n");
}

// Ordering tasks by priority
void test_order_tasks() {
    TaskList log;
    initialise_tasklist(&log);

    FILE *todos = tmpfile();
    Task task_1 = create_task(1, "2025-10-20", "Washing", 2, 0);
    Task task_2 = create_task(2, "2025-10-21", "Dishes", 1, 0);
    add_task(todos, task_1);
    add_task(todos, task_2);

    rewind(todos);
    load_tasks(&log, todos);

    order_tasks(&log, ORDER_PRIORITY);
    assert(log.tasks[0].priority <= log.tasks[1].priority);

    fclose(todos);
    printf("test_order_tasks passed\n");
}