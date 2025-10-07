/*
A to-do list problem
The user will be able to add, complete or delete tasks, as well as getting a list of the to-dos. 
Each task has an id, name, priority level, date due and status.  
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

#define MAX_TASKS 100
#define MAX_LINE 200

int main(int argc, char *argv[])
{
    Task log[MAX_TASKS];

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

    int num = log_tasks(tasks, log);

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
        Order order = 0;
        if (argc == 3)
        {
            order = strupr(argv[3]);
        }

        printf("\033[1m%3s %50s %10s %11s %10s\033[0m\n", "id", "name", "priority", "date", "status");
        order_tasks(log, order, num);

 /*     printf("\033[1m%3s %50s %10s %11s %10s\033[0m\n", "id", "name", "priority", "date", "status");
        for (int i = 0; i < num; i++)
        {
            print_task(log[i]);
        } */


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

        // format the new task from the command line
        Task t = format_new_task(argv, num);

        // Write the new task into the text file 
        add_task(t, tasks);
        num++;
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

        rewrite_tasks(log, num, tasks);
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

        rewrite_tasks(log, num, tasks);
    }

    fclose(tasks);

    return 0;
}
