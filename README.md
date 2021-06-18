# Hospital

A short program for multithreading and locking.   
It takes two arguments: number of doctors and number of patients. In the `definition.h` 
file you can change the number of receptionist staff.   
Patients and doctors are threads.
Before a doctor can treat a patient, a patient needs do register at a receptionist. 
Each doctor and each receptionist can only handle one person at a time.   
It requires two different types of locks: Semaphores ans mutexes.


# Execution
To compile use:  
`gcc -Wall -Werror -std=gnu11  -o hospital definitions.h hospital.c queue.c queue.h resources.h -pthread -D_GNU_SOURCE 
-D_DEFAULT_SOURCE -I/usr/include/ncursesw -lncursesw -ltinfo`

To execute run:   
`./hospital <num_doctors> <num_patients>`
