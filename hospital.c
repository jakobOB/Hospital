#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include "definitions.h"
#include "resources.h"
#include "queue.h"

ssize_t num_doctors = 0;
ssize_t num_patients = 0;
int number_of_treated_patience = 0;
int registration_desks[NUM_RECEPTIONIST_STAFF];

sem_t registration_desks_lock_sem;
pthread_mutex_t number_of_treated_patience_lock;
pthread_mutex_t registration_desk_lock;
pthread_mutex_t queue_lock;


//------------------------------------------------------------------------------
void checkInAtRegistrationDesk(Patient* patient)
{
  printf("Patient %zd is waiting for a free registration desk!\n", patient->id);

  size_t desks_i = 0;

  sem_wait(&registration_desks_lock_sem);
  pthread_mutex_lock(&registration_desk_lock);
  while(registration_desks[desks_i] != 0) 
  {
    desks_i++;
  }

  registration_desks[desks_i] = 1;
  pthread_mutex_unlock(&registration_desk_lock);
  printf("Patient %zd registers at the registration desk %zd!\n", patient->id, desks_i);
  fillOutTheRegistrationForm();

  pthread_mutex_lock(&registration_desk_lock);
  registration_desks[desks_i] = 0;
  pthread_mutex_unlock(&registration_desk_lock);
  sem_post(&registration_desks_lock_sem);
}

//------------------------------------------------------------------------------
void moveToWaitingRoom(Patient* patient)
{
  Waiting waiting = {patient};
  pthread_mutex_lock(&queue_lock);
  queue_push_back(waiting);
  pthread_mutex_unlock(&queue_lock);
}

//------------------------------------------------------------------------------
void waitUnitlTreatmentIsDone(Patient* patient)
{
  pthread_mutex_lock(&patient->patient_lock);
  while (patient->was_treated == 0)
  {
    pthread_cond_wait(&patient->patient_is_waiting, &patient->patient_lock);
  }
  pthread_mutex_unlock(&patient->patient_lock);
}

//------------------------------------------------------------------------------
void* patient_do(void* param_patient)
{
  Patient* patient = (Patient*) param_patient;
  printf("Patient %zd is comming into the Hospital!\n", patient->id);

  moveIntoHospital(); // wait

  checkInAtRegistrationDesk(patient); // sem

  moveToWaitingRoom(patient); // queue

  waitUnitlTreatmentIsDone(patient); // cond

  // leave hospital
  if (patient->was_treated == 1)
  {
    printf("Patient %zd was treated so he goes home healthy and happy :-)!\n", patient->id);
  }
  else
  {
    printf("Patient %zd was NOT treated so he goes home ill and may die :-(!\n", patient->id);
  }
  
  printf("Patient %zd is now leaving the Hospital!\n", patient->id);
  return NULL;
}


//------------------------------------------------------------------------------
void* doctor_do(void* param_doctor)
{
  Doctor* doctor = (Doctor*) param_doctor;
  while (1)
  {
    pthread_mutex_lock(&number_of_treated_patience_lock);
    
    if (number_of_treated_patience >= num_patients)
    {
      pthread_mutex_unlock(&number_of_treated_patience_lock);
      break;
    }
    pthread_mutex_unlock(&number_of_treated_patience_lock);
    // take patient from waiting room (queue) into treatment room // lock
    Waiting w;
    pthread_mutex_lock(&queue_lock);
    bool got_new_patient = queue_pop_front(&w);
    pthread_mutex_unlock(&queue_lock);

    if(!got_new_patient)
    {
      printf("Doctor %zd has no patient to treat, so he takes a short nap!\n", doctor->id);
      takeAShortBreak();
      continue;
    }

    // examine patient to find illess and cure illness // wait
    Patient* patient = w.patient;
    printf("Doctor %zd starts treating Patient %zd!\n", doctor->id, patient->id);

    findIllnessAndTreadIt(); // wait
    pthread_mutex_lock(&patient->patient_lock);
    patient->was_treated = 1;

    // inform patient about its treatment
    pthread_cond_signal(&patient->patient_is_waiting);

    // increment treated patient counter for statistics
    printf("Doctor %zd has finished treating Patient %zd\n", doctor->id, patient->id);
    pthread_mutex_unlock(&patient->patient_lock);
    pthread_mutex_lock(&number_of_treated_patience_lock);
    number_of_treated_patience++;
    pthread_mutex_unlock(&number_of_treated_patience_lock);
  }
  return NULL;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    fprintf(stderr, "Usage: %s <num_doctors> <num_patients>\n", argv[0]);
    exit(-1);
  }

  num_doctors = atoi(argv[1]);
  if(num_doctors < 1 || num_doctors > 5)
  {
    fprintf(stderr, "num_doctors allowed range: [1,5].\n");
    exit(-1);
  }
  num_patients = atoi(argv[2]);
  if(num_patients < 1 || num_patients > 200)
  {
    fprintf(stderr, "num_patients allowed range: [1,200].\n");
    exit(-1);
  }

  printf("Hospital is opening!\n");
  Patient* patients = malloc(num_patients*sizeof(Patient)); 
  Doctor* doctors = malloc(num_doctors*sizeof(Doctor));
  if(!patients || !doctors)
  {
    free(patients);
    free(doctors);
    fprintf(stderr, "Could not allocate memory!\n");
    exit(-1);
  }

  queue_init();

  // * initialise needed variables
  sem_init(&registration_desks_lock_sem, 0, NUM_RECEPTIONIST_STAFF);
  pthread_mutex_init(&number_of_treated_patience_lock, 0);
  pthread_mutex_init(&registration_desk_lock, 0);
  pthread_mutex_init(&queue_lock, 0);

  // create threads
  for(ssize_t i = 0; i < num_patients; i++)
  {
    patients[i].id = i;
    patients[i].was_treated = 0;

    // * initialize new patients members
    pthread_mutex_init(&patients[i].patient_lock, 0);
    pthread_cond_init(&patients[i].patient_is_waiting, 0);
  
    pthread_create(&patients[i].tid, NULL, patient_do, (void *) &patients[i]);
  }
  for(ssize_t i = 0; i < num_doctors; i++)
  {
    // * initialize new doctors members
    doctors[i].id = i;

    pthread_create(&doctors[i].tid, NULL, doctor_do, (void *) &doctors[i]);
  }


  // end -> join all threads
  for(ssize_t i = 0; i < num_patients; i++)
  {
    pthread_join(patients[i].tid, NULL);
  }
  for(ssize_t i = 0; i < num_doctors; i++)
  {
    pthread_join(doctors[i].tid, NULL);
  }

  printf("Number of treated patience %d!\n", number_of_treated_patience);
  printf("Hospital is now closed!\n");

  free(patients);
  free(doctors);

  //* clean up
  sem_destroy(&registration_desks_lock_sem);
  pthread_mutex_destroy(&number_of_treated_patience_lock);
  pthread_mutex_destroy(&registration_desk_lock);
  pthread_mutex_destroy(&queue_lock);


  return 0;
}

