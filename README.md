# Checkaroo - command-line to-do list manager
#### Video Demo:  <(https://youtu.be/eyNde8BRgcA)>
#### Description:
This is a C command-line program.

It is essentially a to-do list which takes arguments from the user on the command line.

The user can then do one of four things:
- list tasks
- add tasks
- update a task
- delete a task

There is a maximum number of tasks, currently set at 100.
The tasks are structs, which contain an id, name, priority, date due and status.
The tasks are stored in a text file, which are read at the start every time the program is used.
When the task is being read, the information is parsed into tasks and stored in a log, which is a list of all the tasks.
This list is also part of a struct called TaskList which alongside the list of tasks contains the task capacity and the current task length.

##### List
When the user calls list, they can specify by which order they want the tasks to be ordered.
This can id, name, priority, date due, or status. If no order is given, id is assumed.
The log of tasks is then ordered in the appropriate manner and printed in the terminal for the user to see.

##### Add
When the user calls add, they must give the name, priority, date due and optionally the status.
If no status is given, pending is assumed.
The values are all check and converted if necessary and then added to the tasks log.
The tasks are then saved into the text file.

##### Update
When the user calls update, they must give the field they wish to update, the new value and the id of the task they wish to update.
The updated value is checked and parsed if valid.
If the id is valid, the task of this id is then identified from the log and the field indicated is updated.
The tasks are then saved into the text file.

##### Remove
When the user calls delete, they must also give the id of the task they wish to delete.
If the id is valid, the task is then identified from the task log and is deleted from the log.
The tasks are then saved into the text file.

##### Usage
Should the user use the incorrect usage, a usage message is printed to indicate correct usage.
```
Usage: ./checkaroo <cmd>
cmd: add, remove, update, list
add <name> <priority> <date> (optional: <status>)
remove <id>
update <field> <updated_value> <id>
field: name, date, status, priority
list (optional: <order>)
order: id, name, date, status, priority
```
Should any other error occur, the program exits and prints an appropriate error message, indicating what went wrong.

##### Text file
The text file is also clearly readable, with priority and status using types enum.

Priority: 0 - High, 1 - Medium, 2 - Low

Status: 0 - Pending, 1 - Complete

````
id: 1 name: "painting" priority: 2 date: 2026-01-01 status: 0
id: 2 name: "sand" priority: 1 date: 2025-10-18 status: 1
id: 3 name: "hello world" priority: 5 date: 2025-08-11 status: 0
````

##### Unit Tests
This checks some simple usage of the program including:
- adding a task
- trying to add more than 100 tasks
- update a task
- delete a task
- list tasks ordered by priority

##### Compilation
The program is complied using a MAKEFILE.

To build everything: ```make```

To build the main program: ```make checkaroo```

To run the unit tests: ```make tests```

To clean up complied files: ```make clean```

###### Example Usage
````
make
./checkaroo add "Buy bread" 1 2025-10-25
./checkaroo list priority
./checkaroo update name "Buy milk" 1
./checkaroo delete 2
````
