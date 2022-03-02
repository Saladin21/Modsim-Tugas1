/* External definitions for job-shop model. */

#include "simlib.h"		/* Required for use of simlib.c. */

#define EVENT_ARRIVAL         1	/* Event type for arrival of a job to the system. */
#define EVENT_ARRIVAL_1       2
#define EVENT_ARRIVAL_2       3
#define EVENT_DEPARTURE_1     4	/* Event type for departure of a job from a particular station. */
#define EVENT_DEPARTURE_2     5 
#define EVENT_ARRIVAL_3       6
#define EVENT_DEPARTURE_3     7
#define EVENT_END_SIMULATION  8	/* Event type for end of the simulation. */
#define STREAM_INTERARRIVAL   1	/* Random-number stream for interarrivals. */
#define STREAM_JOB_TYPE       2	/* Random-number stream for job types. */
#define STREAM_SERVICE        3	/* Random-number stream for service times. */
#define STREAM_JOB_SHOP       4 /* Random-number stream for job shop. */
#define MAX_NUM_STATIONS      5	/* Maximum number of stations. */
#define MAX_NUM_JOB_TYPES     3	/* Maximum number of job types. */
#define NUM_JOB_SHOP          3 /* Banyaknya job shop */

/* Declare non-simlib global variables. */

int num_stations, num_job_types, i, j, num_machines[NUM_JOB_SHOP][MAX_NUM_STATIONS + 1],
  num_tasks[MAX_NUM_JOB_TYPES + 1],
  route[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1], num_machines_busy[NUM_JOB_SHOP][MAX_NUM_STATIONS + 1], job_type, task, job_type1, task1,job_type2, task2,
  job_type3, task3;
double mean_interarrival, length_simulation, prob_distrib_job_type[26],
  mean_service[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1];
double prob_Job_Shop[2];
FILE *infile, *outfile;


void
arrive (int new_job)		/* Function to serve as both an arrival event of a job
				   to the system, as well as the non-event of a job's
				   arriving to a subsequent station along its
				   route. */
{
  
  int job_shop;

  /* If this is a new arrival to the system, generate the time of the next
     arrival and determine the job type and task number of the arriving
     job. */

    event_schedule (sim_time + expon (mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);

    //Probability 0,5
    prob_Job_Shop[0] = 0.5;
    prob_Job_Shop[1] = 1;
    job_shop = random_integer (prob_Job_Shop, STREAM_JOB_SHOP);

    if (job_shop == 0){
      event_schedule (sim_time, EVENT_ARRIVAL_1);
    }else{
      event_schedule (sim_time, EVENT_ARRIVAL_2);
    }
}

void arrive_1(int new_job){
  int station;

  if (new_job == 1)
    {

      event_schedule (sim_time + expon (mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);
      job_type1 = random_integer (prob_distrib_job_type, STREAM_JOB_TYPE);
      task1 = 1;
    }

 
  /* Determine the station from the route matrix. */

  station = route[job_type1][task1];

  /* Check to see whether all machines in this station are busy. */

  if (num_machines_busy[0][station] == num_machines[0][station])
    {

      /* All machines in this station are busy, so place the arriving job at
         the end of the appropriate queue. Note that the following data are
         stored in the record for each job:
         1. Time of arrival to this station.
         2. Job type.
         3. Current task number. */

      transfer[1] = sim_time;
      transfer[2] = job_type1;
      transfer[3] = task1;
      list_file (LAST, station);
    }

  else
    {

      /* A machine in this station is idle, so start service on the arriving
         job (which has a delay of zero). */

      sampst (0.0, station);	/* For station. */
      sampst (0.0, num_stations + job_type);	/* For job type. */
      ++num_machines_busy[0][station];
      timest ((double) num_machines_busy[0][station], station);

      /* Schedule a service completion.  Note defining attributes beyond the
         first two for the event record before invoking event_schedule. */

      transfer[3] = job_type1;
      transfer[4] = task1;
      event_schedule (sim_time + erlang (2, mean_service[job_type1][task1], STREAM_SERVICE), EVENT_DEPARTURE_1);
    }
}

void
depart_1 (void)			/* Event function for departure of a job from a particular
				   station. */
{
  int station, job_type_queue, task_queue;

  /* Determine the station from which the job is departing. */

  job_type1 = transfer[3];
  task1 = transfer[4];
  station = route[job_type1][task1];

  /* Check to see whether the queue for this station is empty. */

  if (list_size[station] == 0)
    {

      /* The queue for this station is empty, so make a machine in this
         station idle. */

      --num_machines_busy[0][station];
      timest ((double) num_machines_busy[0][station], station);
    }

  else
    {

      /* The queue is nonempty, so start service on first job in queue. */

      list_remove (FIRST, station);

      /* Tally this delay for this station. */

      sampst (sim_time - transfer[1], station);

      /* Tally this same delay for this job type. */

      job_type_queue = transfer[2];
      task_queue = transfer[3];
      sampst (sim_time - transfer[1], num_stations + job_type_queue);

      /* Schedule end of service for this job at this station.  Note defining
         attributes beyond the first two for the event record before invoking
         event_schedule. */

      transfer[3] = job_type_queue;
      transfer[4] = task_queue;
      event_schedule (sim_time + erlang (2, mean_service[job_type_queue][task_queue], STREAM_SERVICE), EVENT_DEPARTURE_1);
    }

  /* If the current departing job has one or more tasks yet to be done, send
     the job to the next station on its route. */

  if (task1 < num_tasks[job_type1])
    {
      ++task1;
      arrive (2);
    }else{
      transfer[3] = job_type1;
      event_schedule (sim_time, EVENT_ARRIVAL_3);
    }
}

void arrive_2(int new_job){
  int station;

  if (new_job == 1)
    {

      event_schedule (sim_time + expon (mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);
      job_type2 = random_integer (prob_distrib_job_type, STREAM_JOB_TYPE);
      task2 = 1;
    }

 
  /* Determine the station from the route matrix. */

  station = route[job_type2][task2];

  /* Check to see whether all machines in this station are busy. */

  if (num_machines_busy[1][station] == num_machines[1][station])
    {

      /* All machines in this station are busy, so place the arriving job at
         the end of the appropriate queue. Note that the following data are
         stored in the record for each job:
         1. Time of arrival to this station.
         2. Job type.
         3. Current task number. */

      transfer[1] = sim_time;
      transfer[2] = job_type2;
      transfer[3] = task2;
      list_file (LAST, station + num_stations);
    }

  else
    {

      /* A machine in this station is idle, so start service on the arriving
         job (which has a delay of zero). */

      sampst (0.0, station);	/* For station. */
      sampst (0.0, num_stations + job_type);	/* For job type. */
      ++num_machines_busy[1][station];
      timest ((double) num_machines_busy[1][station], station);

      /* Schedule a service completion.  Note defining attributes beyond the
         first two for the event record before invoking event_schedule. */

      transfer[3] = job_type2;
      transfer[4] = task2;
      event_schedule (sim_time + erlang (2, mean_service[job_type2][task2], STREAM_SERVICE), EVENT_DEPARTURE_2);
    }
}

void
depart_2 (void)			/* Event function for departure of a job from a particular
				   station. */
{
  int station, job_type_queue, task_queue;

  /* Determine the station from which the job is departing. */

  job_type2 = transfer[3];
  task2 = transfer[4];
  station = route[job_type2][task2];

  /* Check to see whether the queue for this station is empty. */

  if (list_size[station + num_stations] == 0)
    {

      /* The queue for this station is empty, so make a machine in this
         station idle. */

      --num_machines_busy[1][station];
      timest ((double) num_machines_busy[1][station], station);
    }

  else
    {

      /* The queue is nonempty, so start service on first job in queue. */

      list_remove (FIRST, station);

      /* Tally this delay for this station. */

      sampst (sim_time - transfer[1], station);

      /* Tally this same delay for this job type. */

      job_type_queue = transfer[2];
      task_queue = transfer[3];
      sampst (sim_time - transfer[1], num_stations + job_type_queue);

      /* Schedule end of service for this job at this station.  Note defining
         attributes beyond the first two for the event record before invoking
         event_schedule. */

      transfer[3] = job_type_queue;
      transfer[4] = task_queue;
      event_schedule (sim_time + erlang (2, mean_service[job_type_queue][task_queue], STREAM_SERVICE), EVENT_DEPARTURE_2);
    }

  /* If the current departing job has one or more tasks yet to be done, send
     the job to the next station on its route. */

  if (task2 < num_tasks[job_type2])
    {
      ++task2;
      arrive (2);
    }else{
      transfer[3] = job_type2;
      event_schedule (sim_time, EVENT_ARRIVAL_3);
    }
}

void arrive_3(int new_job){
  int station;

  if (new_job == 1)
    {
      job_type3 = transfer[3];
      task3 = 1;
    }

 
  /* Determine the station from the route matrix. */

  station = route[job_type3][task3];

  /* Check to see whether all machines in this station are busy. */

  if (num_machines_busy[2][station] == num_machines[2][station])
    {

      /* All machines in this station are busy, so place the arriving job at
         the end of the appropriate queue. Note that the following data are
         stored in the record for each job:
         1. Time of arrival to this station.
         2. Job type.
         3. Current task number. */

      transfer[1] = sim_time;
      transfer[2] = job_type3;
      transfer[3] = task3;
      list_file (LAST, station + num_stations*2);
    }

  else
    {

      /* A machine in this station is idle, so start service on the arriving
         job (which has a delay of zero). */

      sampst (0.0, station);	/* For station. */
      sampst (0.0, num_stations + job_type);	/* For job type. */
      ++num_machines_busy[2][station];
      timest ((double) num_machines_busy[2][station], station);

      /* Schedule a service completion.  Note defining attributes beyond the
         first two for the event record before invoking event_schedule. */

      transfer[3] = job_type3;
      transfer[4] = task3;
      event_schedule (sim_time + erlang (2, mean_service[job_type3][task3], STREAM_SERVICE), EVENT_DEPARTURE_3);
    }
}

void
depart_3 (void)			/* Event function for departure of a job from a particular
				   station. */
{
  int station, job_type_queue, task_queue;

  /* Determine the station from which the job is departing. */

  job_type3 = transfer[3];
  task3 = transfer[4];
  station = route[job_type3][task3];

  /* Check to see whether the queue for this station is empty. */

  if (list_size[station + num_stations*2] == 0)
    {

      /* The queue for this station is empty, so make a machine in this
         station idle. */

      --num_machines_busy[2][station];
      timest ((double) num_machines_busy[2][station], station);
    }

  else
    {

      /* The queue is nonempty, so start service on first job in queue. */

      list_remove (FIRST, station);

      /* Tally this delay for this station. */

      sampst (sim_time - transfer[1], station);

      /* Tally this same delay for this job type. */

      job_type_queue = transfer[2];
      task_queue = transfer[3];
      sampst (sim_time - transfer[1], num_stations + job_type_queue);

      /* Schedule end of service for this job at this station.  Note defining
         attributes beyond the first two for the event record before invoking
         event_schedule. */

      transfer[3] = job_type_queue;
      transfer[4] = task_queue;
      event_schedule (sim_time + erlang (2, mean_service[job_type_queue][task_queue], STREAM_SERVICE), EVENT_DEPARTURE_3);
    }

  /* If the current departing job has one or more tasks yet to be done, send
     the job to the next station on its route. */

  if (task3 < num_tasks[job_type3])
    {
      ++task2;
      arrive (2);
    }
}

// void
// report (void)			/* Report generator function. */
// {
//   int i;
//   double overall_avg_job_tot_delay, avg_job_tot_delay, sum_probs;

//   /* Compute the average total delay in queue for each job type and the
//      overall average job total delay. */

//   fprintf (outfile, "\n\n\n\nJob type     Average total delay in queue");
//   overall_avg_job_tot_delay = 0.0;
//   sum_probs = 0.0;
//   for (i = 1; i <= num_job_types; ++i)
//     {
//       avg_job_tot_delay = sampst (0.0, -(num_stations + i)) * num_tasks[i];
//       fprintf (outfile, "\n\n%4d%27.3f", i, avg_job_tot_delay);
//       overall_avg_job_tot_delay += (prob_distrib_job_type[i] - sum_probs) * avg_job_tot_delay;
//       sum_probs = prob_distrib_job_type[i];
//     }
//   fprintf (outfile, "\n\nOverall average job total delay =%10.3f\n", overall_avg_job_tot_delay);

//   /* Compute the average number in queue, the average utilization, and the
//      average delay in queue for each station. */

//   fprintf (outfile, "\n\n\n Work      Average number      Average       Average delay");
//   fprintf (outfile, "\nstation       in queue       utilization        in queue");
//   for (j = 1; j <= num_stations; ++j)
//     fprintf (outfile, "\n\n%4d%17.3f%17.3f%17.3f", j, filest (j), timest (0.0, -j) / num_machines[j], sampst (0.0, -j));
// }

int
main ()				/* Main function. */
{
  /* Open input and output files. */

  infile = fopen ("jobshop.in", "r");
  outfile = fopen ("jobshop.out", "w");

  /* Read input parameters. */

  fscanf (infile, "%d %d %lg %lg", &num_stations, &num_job_types, &mean_interarrival, &length_simulation);
  for (j = 1; j <= num_stations; ++j)
    fscanf (infile, "%d", &num_machines[0][j]);
    num_machines[1][j] = num_machines[0][j];
    num_machines[2][j] = num_machines[0][j];
  for (i = 1; i <= num_job_types; ++i)
    fscanf (infile, "%d", &num_tasks[i]);
  for (i = 1; i <= num_job_types; ++i)
    {
      for (j = 1; j <= num_tasks[i]; ++j)
	fscanf (infile, "%d", &route[i][j]);
      for (j = 1; j <= num_tasks[i]; ++j)
	fscanf (infile, "%lg", &mean_service[i][j]);
    }
  for (i = 1; i <= num_job_types; ++i)
    fscanf (infile, "%lg", &prob_distrib_job_type[i]);

  /* Write report heading and input parameters. */

  //Option set buffering to null
  setbuf(outfile,NULL);

  fprintf (outfile, "Job-shop model\n\n");
  fprintf (outfile, "Number of work stations%21d\n\n", num_stations);
  fprintf (outfile, "Number of machines in each station     ");
  for (j = 1; j <= num_stations; ++j)
    //Number of stations identic per jobshop
    fprintf (outfile, "%5d", num_machines[0][j]);
  fprintf (outfile, "\n\nNumber of job types%25d\n\n", num_job_types);
  fprintf (outfile, "Number of tasks for each job type      ");
  for (i = 1; i <= num_job_types; ++i)
    fprintf (outfile, "%5d", num_tasks[i]);
  fprintf (outfile, "\n\nDistribution function of job types  ");
  for (i = 1; i <= num_job_types; ++i)
    fprintf (outfile, "%8.3f", prob_distrib_job_type[i]);
  fprintf (outfile, "\n\nMean interarrival time of jobs%14.2f hours\n\n", mean_interarrival);
  fprintf (outfile, "Length of the simulation%20.1f eight-hour days\n\n\n", length_simulation);
  fprintf (outfile, "Job type     Work stations on route");
  for (i = 1; i <= num_job_types; ++i)
    {
      fprintf (outfile, "\n\n%4d        ", i);
      for (j = 1; j <= num_tasks[i]; ++j){
        fprintf (outfile, "%5d", route[i][j]);
      }
    }
  fprintf (outfile, "\n\n\nJob type     ");
  fprintf (outfile, "Mean service time (in hours) for successive tasks");
  for (i = 1; i <= num_job_types; ++i)
    {
      fprintf (outfile, "\n\n%4d    ", i);
      for (j = 1; j <= num_tasks[i]; ++j){
        fprintf (outfile, "%9.2f", mean_service[i][j]);
      }
    }

  /* Initialize all machines in all stations to the idle state. */

  for (j = 1; j <= num_stations; ++j){
    num_machines_busy[0][j] = 0;
    num_machines_busy[1][j] = 0;
    num_machines_busy[2][j] = 0;
  }
    
  /* Initialize simlib */

  init_simlib ();

  /* Set maxatr = max(maximum number of attributes per record, 4) */

  maxatr = 4;			/* NEVER SET maxatr TO BE SMALLER THAN 4. */

  /* Schedule the arrival of the first job. */

  event_schedule (expon (mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);

  /* Schedule the end of the simulation.  (This is needed for consistency of
     units.) */

  event_schedule (8 * length_simulation, EVENT_END_SIMULATION);

  /* Run the simulation until it terminates after an end-simulation event
     (type EVENT_END_SIMULATION) occurs. */

  //Start of the simulation DEBUG message
  fprintf(outfile,"\n\n\nSTART OF THE SIMULATION:");
  do
    {

      /* Determine the next event. */

      timing ();

      /* Invoke the appropriate event function. */

      switch (next_event_type)
	{
	case EVENT_ARRIVAL:
	  arrive (1);
	  break;
  case EVENT_ARRIVAL_1:
    arrive_1(1);
    break;
  case EVENT_ARRIVAL_2:
    arrive_2(1);
    break;
  case EVENT_DEPARTURE_1:
    depart_1();
    break;
  case EVENT_DEPARTURE_2:
    depart_2();
    break;
  case EVENT_ARRIVAL_3:
    arrive_3(1);
    break;
  case EVENT_DEPARTURE_3:
    depart_3();
    break;
	case EVENT_END_SIMULATION:
	  //report ();
	  break;
	}

      /* If the event just executed was not the end-simulation event (type
         EVENT_END_SIMULATION), continue simulating.  Otherwise, end the
         simulation. */

    }
  while (next_event_type != EVENT_END_SIMULATION);

  fclose (infile);
  fclose (outfile);

  //End of the simulation DEBUG message
  fprintf(outfile,"\n\n\nEND OF THE SIMULATION:");
  return 0;
}