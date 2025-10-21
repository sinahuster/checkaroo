/*
A to-do list problem
The user will be able to add, complete or delete tasks, as well as getting a list of the to-dos. 
Each task has an id, name, priority level, date due and status.  
*/

#include <stdio.h>
#include <stdlib.h>

#include "tasks.h"

int main(int argc, char *argv[])
{
    TaskList log;

    initialise_tasklist(&log);

    // Check if any arguements except the program name is given. 
    if (argc == 1)
    {
        free_tasklist(&log);
        print_usage();
        return 1;
    }

    // Open a file for the to-do list 
    FILE *todos = fopen("to-dos.txt", "a+");
    if (todos == NULL)
    {
        fprintf(stderr, "Error opening tasks file\n");
        return 1;
    }

    // Move pointer back to start for reading
    rewind(todos);

    load_tasks(&log, todos);

    // Determine the command
    Command cmd = determine_command(argv[1]);

    switch(cmd)
    {
        case(COMMAND_ADD):
        {
            // Check if the to-do list is full
            if (log.length == log.capacity)
            {
                fprintf(stderr,"To-do list is full, cannot add anymore tasks.\n");
                return 1;
            }

            // Check all information that's required is present 
            if (argc < 5 || argc > 6)
            {
                print_usage();
                return 1;
            }


            // Format the new task from the command line
            Task new = parse_task(log, argv);

            // Write the new task into the text file 
            add_task(todos, new);
            log.length++;

            break;
        }
        case(COMMAND_LIST):
        {
            // Check if the to-do list is empty
            if (log.length == 0)
            {
                printf("There are currently no to-dos\n");
                return 0;
            }
            
            // Determine the order in which to print the tasks
            Order order = 0;
            if (argc == 3)
            {
                order = determine_order(argv[2]);
            }
            order_tasks(&log, order);

            // Print headers then print the tasks
            printf("\033[1m%3s %-20s %-10s %-11s %-10s\033[0m\n", "id", "name", "priority", "date", "status");
            print_tasks(log);
            
            break;
        }
        case(COMMAND_UPDATE):
        {
            // Check the usage is correct 
            if (argc != 5)
            {
                print_usage();
                return 1;
            }

            // Check the id is valid
            int id = atoi(argv[4]);
            if (id <= 0 || id > log.length) 
            {
                fprintf(stderr, "This task id does not exist.\n");
                return 1;
            }
            // Find the field to be updated 
            Order order = determine_order(argv[2]);

            // Update task and rewrite
            update_task(&log, id, order, argv[3]);
            save_tasks(&log, todos);

            break;
        }
        case(COMMAND_REMOVE):
        {
            // Check we have the correct usage
            if (argc < 3)
            {
                print_usage();
                return 1;
            }

            // Check that the id exsits
            int id = atoi(argv[2]);
            if (id <= 0 || id > log.length) 
            {
                fprintf(stderr, "This task id does not exist.\n");
                return 1;
            }       

            // Delete and rewrite tasks
            delete_task(&log, id);
            save_tasks(&log, todos);

            break;
        }
        default:
        {
            fprintf(stderr, "Command is not valid\n");
            print_usage();
            return 1;
        }
    }

    fclose(todos);

    free_tasklist(&log);

    return 0;
}
