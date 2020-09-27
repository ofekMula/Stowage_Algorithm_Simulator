# Objective

An implementation of a stowage model for ships and a simulation to check stowage models.
the program includes two modules:

### 1. Simulation
A module that loads data from files and then runs several stowage algorithms on the same data to compare the efficiency of the algorithms. It should also be possible to compare algorithms on several “travels” (different routes and cargo). Of course the simulation should also check that stowage algorithms are correct, i.e. they do not miss containers loading or unloading and follow all the rules in general. The simulation would also initiate the ship with its weight calculator, so it can control simulation of weight triggers and check that the stowage algorithms comply.

### 2. Stowage Algorithm
A strategy of loading and unloading containers off from a ship when reaching to a port during a travel.

## Input files :
Travel: a folder which presents a path of ports the ship will stop at. each travel contains:
1.ShipPlan file - the structure of the ship.
2.Route file - which port (and in which order) the ship will visit at
3.cargo_data - container awaiting at each port

## output files: 
A CSV table format which contains :
for each algorithm , the number of operations for eaxch travel, and number of errors (the algorthm did).
the best algorithm will be the one without Errors and the smallest number of operations needed for loading and unloading the containers during the travel.


## Multi-threading:
1. Our multi threading model is based on the ThreadPoolExecutor example seen in class.
   If the number of threads parameter is 1 or less, the simulation stays sequential and does not spawn any new threads (ThreadPoolExecutor is not called).

2.  In our implementation the Simulation class itself is the task producer and
    implements the get_task method. It keeps atomic indices for the algorithm
    and travel and the task itself accesses the travel and creates the algorithm
    using these indices and then runs the run_single_travel method.
    The run_simulations method is the main run method of Simulation and it calls
    ThreadPoolExecutor in multithreaded mode or does the cartesian loop calling
    run_single_travel in single threaded mode.
    When the simulation finishes creating all tasks, and the threads complete their tasks,
    the simulation writes the results of the algorithms and finishes.

3.  How we managed access of shared resources in our multi threading program :
    A data race problem can occur in resources that can be written to by multiple threads
    simultaneously.
    In our implementation it can happen in the statistics field of simulation and in writing to the errors files
    during algorithm execution.
    a. Updating the statistics table :
     -  Our statistics table is implemented by a mapping of algorithm names to TravelsResults object which holds
        a mapping of travel name to a TravelStats object which holds the statistics of a travel made by an algorithm.
        Before we execute the ThreadPoolExecutor and the program splits,
        we initialize the statistics table, which ensures that every <algorithm,travel> pair will have its own cell in the table.

     -  In practice, each thread gets a unique <algorithm,travel> pair to execute.
        When the thread updates the stats table, it updates only its own cell in the statistics table,
        and no other thread accesses this cell.
        Since no new items are inserted and only values and not keys are changed,
        we know for certain that the underlying data structure doesn't change.

     -  Only after the simulation created all the tasks, and all the threads finished their tasks,
        the program returns to be sequential and we writes the simulation.result file.

    b. Writing to errors files:

     -  We hold a mutex for each error file we have (5 in total). How do we manage them?
        The file parser holds a private field - a pointer of ErrorsFilesMutex object (a.k.a - error_file_to_mutex)
        this object holds the above mutexes and knows how to map the file name to its unique mutex.
     -  When an error is written during a thread execution, before we write the message to the file, we use the above mutex and
        lock it using a lock_guard.


## Algorithms implementation:
The algorithms are improved and once again are different but only by a little.
As before, both algorithms inherit the StowageAlgorithm base class and
only the differences are implemented in their own classes.
Algorithm A is fully improved and algorithm B lacks improvement number 2
in the following list.

## Algorithm improvements:
1.  Choosing a tower - The algorithm now avoids loading containers to towers
    with many containers headed to the different ports, thus avoiding situations
    where containers whose destination is not the current port need to be moved
    or unloaded and reloaded to reach containers that need to be unloaded.
    The algorithm does this by counting how many different destination ports
    are in each tower and trying to minimize that number.

2.  Loading order - The algorithm sorts the containers loaded by distance of
    their destination port where farther ports are loaded first, making sure
    they are more likely to be placed lower in the ship and don't need to be
    moved when unloading in the closer ports.

3.  Move operations - The algorithm now performs move operations when there are
    containers that need to be unloaded in the port under containers that don't.
    The move destinations are chosen in the same manner as when choosing load
    destinations in improvement 1, making sure that they are moved to destinations
    that will require minimal move operations in next ports.
    If the ship is full and there are no possible move destinations,
    the algorithm resorts to unloads and loads.
    Note that improvements 1 and 2 are intended to avoid situations where move
    operations are needed.

The improvements can be seen in functions
1. get_next_load_instruction which uses get_idx_of_best_tower_for_load
2. order_containers_for_Load which sorts the containers
3. add_instructions_remove_all_above_containers which uses the move operation

## Error handling  :
Error file structure:
The program generates the errors directory if and only if an error was detected.
The file / folder will be created in the location specified by the -output option or in the run directory if the argument was not given.

We mapped each error to one of the the following files :
1. "fatal_errors.txt" - Errors that prevent the run of the entire simulation
2. "travel_errors.txt" - Errors that made the simulator skip a travel
3. "bad_algorithm_behavior_errors.txt" - Errors detected by the simulator when an algorithm performs an illegal instruction or fails to load/unload containers.
4. "ignorable_errors.txt" - Any error which can be "fixed" and not hurt the run of the simulator 5. "Error code returned by the Algorithm.txt" - All the errors detected and returned by an algorithm while its simulated on a travel.

The errors are displayed in a special readable format, as explained in the previous exercise.


## What I learned :
* Using Smart pointers - unique pointer and shared pointer
* Thread Pool design pattern.
* working with .so files.
* Program accordingly to Document of Requirements.
* use templates.
* Proper design of modules and classes,and divide the functionallity between them.
* correct use of OOP.
* ifstream and ostream.
* Writing a makefile.
