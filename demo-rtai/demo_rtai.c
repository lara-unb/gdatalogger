#include <sched.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/io.h>
#include <rtai_lxrt.h>

#include "rdtsctime.h"
#include "../gqueue.h"
#include "../gmatlabdatafile.h"
#include "../gdatalogger.h"

// período de amostragem
#define MAIN_PERIOD_IN_NANO  1000000  /* deve ser submultiplo do menor dos periodos */
#define TASK1_PERIOD_IN_NANO 1000000
#define TASK2_PERIOD_IN_NANO 5000000

#define SCHEDMODE_ONESHOT	0
#define SCHEDMODE_PERIODIC	1

//int schedmode = SCHEDMODE_ONESHOT;
int schedmode = SCHEDMODE_PERIODIC;

static int quittask = 0;
GDATALOGGER gDataLogger;
CLOCK_COUNTER ckstart;

//Funcao periodica
static void *periodicThread_handler1(void *arg)
{
	CLOCK_COUNTER ckcurrent,ckprevious;
	CLOCK_COUNTER ckcurrent_rtsleep,ckprevious_rtsleep;
	RT_TASK *periodicTask1; //Stores a handle
	int priority = 0;   // Highest
	int stack_size = 0; // Use default (512)
	int msg_size = 0;   // Use default (256)
	int period1 = 0;
	int status;
	double tempo,y,T,Tsleep;

	printf("\n**** Tarefa 1.");

	periodicTask1 = rt_task_init( nam2num("TASK01"), priority, stack_size, msg_size);
	rt_make_hard_real_time();
	period1 = (int) nano2count((RTIME)TASK1_PERIOD_IN_NANO);
	status = rt_task_make_periodic(periodicTask1, rt_get_time() + period1, period1);
	printf("\n**** Tarefa 1: status = %i, period = %i",status,period1);
		
	/* A funcao periodica eh executada dentro desse while. 	period = (int) nano2count((RTIME)MAIN_PERIOD_IN_NANO);
	   A partir de quittask, controlada a partir do main, decretamos o seu termino*/
	RDTSC_GetClock(ckprevious);
	
	while(!quittask)
	{	
		// Capturar o tempo atual
		RDTSC_GetClock(ckcurrent);	
		
		// Calculo das variaveis
		T = RDTSC_ComputeTimeInterval(&ckprevious, &ckcurrent); // Lapso de tempo desde a ultima execução.
		tempo = RDTSC_ComputeTimeInterval(&ckstart, &ckcurrent);
		y = sin(2*3.1415926*tempo);
	
		// usleep
		RDTSC_GetClock(ckprevious_rtsleep);	
		rt_sleep((int) nano2count((RTIME)10000));
		RDTSC_GetClock(ckcurrent_rtsleep);	
		Tsleep = RDTSC_ComputeTimeInterval(&ckprevious_rtsleep, &ckcurrent_rtsleep);

		// Inserir na fila
		gDataLogger_InsertVariable(&gDataLogger,"Tsleep1",&Tsleep,1);
		gDataLogger_InsertVariable(&gDataLogger,"t1",&tempo,1);
		gDataLogger_InsertVariable(&gDataLogger,"T1",&T,1);
		gDataLogger_InsertVariable(&gDataLogger,"y1",&y,1);
	
		// Guardar informação sobre o tempo atual
		ckprevious = ckcurrent;
	
		// Aguarda proximo instante de execução
		rt_task_wait_period();
	}

	printf("\nEncerrando a Tarefa 1...");
	rt_make_soft_real_time();
	rt_task_delete(periodicTask1);
	printf(" ok!");

	return NULL;
}

static void *periodicThread_handler2(void *arg)
{
	CLOCK_COUNTER ckcurrent,ckprevious;
	RT_TASK *periodicTask2; //Stores a handle
	int priority = 0;   // Highest
	int stack_size = 0; // Use default (512)
	int msg_size = 0;   // Use default (256)
	int period2 = 0;
	int status;
	double tempo,y,T;

	printf("\n**** Tarefa 2.");

	periodicTask2 = rt_task_init( nam2num("TASK02"), priority, stack_size, msg_size);
	rt_make_hard_real_time();
	period2 = (int) nano2count((RTIME)TASK2_PERIOD_IN_NANO);
	status = rt_task_make_periodic(periodicTask2, rt_get_time() + period2, period2);
	printf("\n**** Tarefa 2: status = %i, period = %i",status,period2);

	/* A funcao periodica eh executada dentro desse while. 	period = (int) nano2count((RTIME)MAIN_PERIOD_IN_NANO);
	   A partir de quittask, controlada a partir do main, decretamos o seu termino*/
	RDTSC_GetClock(ckprevious);
	
	while(!quittask)
	{	
		// Capturar o tempo atual
		RDTSC_GetClock(ckcurrent);	
		
		// Calculo das variaveis
		T = RDTSC_ComputeTimeInterval(&ckprevious, &ckcurrent); // Lapso de tempo desde a ultima execução.
		tempo = RDTSC_ComputeTimeInterval(&ckstart, &ckcurrent);
		y = cos(2*3.1415926*tempo);
	
		// Inserir na fila
		gDataLogger_InsertVariable(&gDataLogger,"t2",&tempo,1);
		gDataLogger_InsertVariable(&gDataLogger,"T2",&T,1);
		gDataLogger_InsertVariable(&gDataLogger,"y2",&y,1);
	
		// Guardar informação sobre o tempo atual
		ckprevious = ckcurrent;
	
		// Aguarda proximo instante de execução
		rt_task_wait_period();
	}

	printf("\nEncerrando a Tarefa 2...");
	rt_make_soft_real_time();
	rt_task_delete(periodicTask2);
	printf(" ok!");

	return NULL;
}


int main(int argc, char *argv[])
{
	RT_TASK* mainTask;      // Stores a handle
	int priority = 1;   // Highest
	int stack_size = 0; // Use default (512)
	int msg_size = 0;   // Use default (256)
	int n;
	int hperiodicThread1;
	int hperiodicThread2;

	// Para determinar o valor da frequencia da CPU, veja o seu valor com o comando: grep MHz /proc/cpuinfo
	RDTSC_SetCPUClock(3.059193e+09);
	RDTSC_GetClock(ckstart);

	// Data logger:
	if(!gDataLogger_Init(&gDataLogger,"matlabdatafiles/gmatlabdatafile.mat",NULL)){
		printf("\nErro em gDataLogger_Init\n\n");
		return EXIT_FAILURE;
	}

	gDataLogger_DeclareVariable(&gDataLogger,"t1","s",1000);
	gDataLogger_DeclareVariable(&gDataLogger,"t2","s",1000);
	gDataLogger_DeclareVariable(&gDataLogger,"T1","s",1000);
	gDataLogger_DeclareVariable(&gDataLogger,"T2","s",1000);
	gDataLogger_DeclareVariable(&gDataLogger,"y1","m",1000);
	gDataLogger_DeclareVariable(&gDataLogger,"y2","m",1000);
	gDataLogger_DeclareVariable(&gDataLogger,"Tsleep1","s",1000);

	/* Temos de garantir que o Linux don't swap us por meio deste comando */
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	/* Inicializa o scheduler do Linux como SCHED_FIFO */
	struct sched_param mysched;

	mysched.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
	if( sched_setscheduler( 0, SCHED_FIFO, &mysched ) == -1 ) {
		puts("ERROR IN SETTING THE SCHEDULER");
		perror("errno");
		exit(1);
	}

	// Transformando o task em task de tempo real (RT_TASK). Pois eh, ateh mesmo o main
	mainTask = rt_task_init( nam2num("MAIN"), priority, stack_size, msg_size);
	if (!mainTask) {
		perror("Não pode criar tarefa MAIN.\n");
		exit(1);
	}
	
	// Poderiamos trocar tudo isso por uma chamada a 
	// task = rt_task_init_schmod( nam2num("Name"), priority, stack_size, msg_size, SCHED_FIFO, processor_mask)
	if (schedmode == SCHEDMODE_ONESHOT) 
		rt_set_oneshot_mode();
	else 
		rt_set_periodic_mode();
	
	start_rt_timer((int) nano2count((RTIME)MAIN_PERIOD_IN_NANO));
	
	hperiodicThread1 = rt_thread_create(periodicThread_handler1, NULL, 10000);
	if (hperiodicThread1 == 0) {
		perror("Criacao do thread 1 falhou.\n");
		exit(1);
	}
	hperiodicThread2 = rt_thread_create(periodicThread_handler2, NULL, 10000);
	if (hperiodicThread2 == 0) {
		perror("Criacao do thread 2 falhou.\n");
		exit(1);
	}
	
	for(n=1;n<=10;++n){
		usleep(200000);	
		gDataLogger_MatfileUpdate(&gDataLogger); // esvazia os buffers no arquivo de log
		
		printf("\n [%i] Atualizado arquivo Matlab",n);
	}

	quittask=1;
	sleep(2);
	printf("\n main: Waiting for end of Thread 1");
	rt_thread_join(hperiodicThread1);
	printf("\n main: Waiting for end of Thread 2");
	rt_thread_join(hperiodicThread2);
	printf("\n main: Threads 1 and 2 finished");
	
	stop_rt_timer();
	rt_task_delete(mainTask);

	// Encerramento do data logger:
	gDataLogger_Close(&gDataLogger);
	
	return 0;
}
