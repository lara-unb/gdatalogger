#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/timer.h>

#include "../../gmatrix/gmatrix.h"
#include "../gqueue.h"
#include "../gmatlabdatafile.h"
#include "../gdatalogger.h"

// periodo de amostragem
#define TASK1_PERIOD_IN_NANO 30000000
#define TASK2_PERIOD_IN_NANO 30000000

static int quittask = 0;
GDATALOGGER gDataLogger;
GMATRIX_DECLARE(MatA,3,1);
GMATRIX_DECLARE(MatB,4,2);

//Funcao periodica
void periodicThread_handler1(void *arg)
{
	double t0, tprevious, tempo, tic, toc;
	double Tsleep, T, y;

	printf("\n**** Tarefa 1.");

	rt_task_set_mode(0,T_RRB ,NULL);
	rt_task_set_periodic(NULL, TM_NOW, TASK1_PERIOD_IN_NANO);

	t0 = ((double)(rt_timer_read()))/1e9;
	tempo = 0.0;

	while(!quittask)
	{	
		rt_task_wait_period(NULL);

		// Calculo das variaveis
		tprevious = tempo;
		tempo = ((double)(rt_timer_read()))/1e9 - t0;
		T = tempo - tprevious;
		y = sin(2*3.1415926*tempo);
		GMATRIX_DATA(MatA,1,1) = y;
		GMATRIX_DATA(MatA,2,1) = 2.0*y;
		GMATRIX_DATA(MatA,3,1) = -y;
	
		// usleep
		tic = ((double)(rt_timer_read()))/1e9;
		rt_task_sleep(10000000);
		toc = ((double)(rt_timer_read()))/1e9;
		Tsleep = toc-tic;

		// Inserir na fila
		gDataLogger_InsertVariable(&gDataLogger,"Tsleep1",&Tsleep);
		gDataLogger_InsertVariable(&gDataLogger,"t1",&tempo);
		gDataLogger_InsertVariable(&gDataLogger,"T1",&T);
		gDataLogger_InsertVariable(&gDataLogger,"y1",&y);
		gDataLogger_InsertVariable(&gDataLogger,"MatA",MatA.Data);

	}

	printf("\nEncerrando a Tarefa 1.");
}

void periodicThread_handler2(void *arg)
{
	double t0, tprevious, tempo, tic, toc;
	double Tsleep, T, y;

	printf("\n**** Tarefa 2.");

	rt_task_set_mode(0,T_RRB ,NULL);
	rt_task_set_periodic(NULL, TM_NOW, TASK2_PERIOD_IN_NANO);

	t0 = ((double)(rt_timer_read()))/1e9;
	tempo = 0.0;

	while(!quittask)
	{	
		rt_task_wait_period(NULL);

		// Calculo das variaveis
		tprevious = tempo;
		tempo = ((double)(rt_timer_read()))/1e9 - t0;
		T = tempo - tprevious;
		y = sin(2*3.1415926*tempo);
		GMATRIX_RANDN(MatB);

		//printf("\n tempo  = %f, y = %f",tempo,y);
	
		// usleep
		tic = ((double)(rt_timer_read()))/1e9;
		rt_task_sleep(10000);
		toc = ((double)(rt_timer_read()))/1e9;
		Tsleep = toc-tic;

		// Inserir na fila
		gDataLogger_InsertVariable(&gDataLogger,"Tsleep2",&Tsleep);
		gDataLogger_InsertVariable(&gDataLogger,"t2",&tempo);
		gDataLogger_InsertVariable(&gDataLogger,"T2",&T);
		gDataLogger_InsertVariable(&gDataLogger,"y2",&y);
		gDataLogger_InsertVariable(&gDataLogger,"MatB",MatB.Data);
	}

	printf("\nEncerrando a Tarefa 2.");
}

void catch_signal(int sig)
{
}

int main(int argc, char *argv[])
{
	RT_TASK task1;
	RT_TASK task2;
	int status,n;
	double t0,tempo;
		
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);
	
	// Data logger:
	if(!gDataLogger_Init(&gDataLogger,"matlabdatafiles/gmatlabdatafile.mat",NULL)){
		printf("\nErro em gDataLogger_Init\n\n");
		return EXIT_FAILURE;
	}

	gDataLogger_DeclareVariable(&gDataLogger,"t1","s",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"t2","s",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"T1","s",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"T2","s",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"y1","m",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"y2","m",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"Tsleep1","s",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"Tsleep2","s",1,1,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"MatA","s",MatA.Nr,MatA.Nc,1000);
	gDataLogger_DeclareVariable(&gDataLogger,"MatB","m",MatB.Nr,MatB.Nc,1000);

	// Tarefas
	status = rt_task_create(&task1, "task1", 0, 99, T_JOINABLE|T_FPU);
	if (status != 0) {
		perror("Criacao do thread 1 falhou.\n");
		exit(1);
	}
	status = rt_task_start(&task1, &periodicThread_handler1, NULL);
	if (status != 0) {
		perror("Inicializacao do thread 1 falhou.\n");
		exit(1);
	}
	rt_task_slice(&task1,100000);

	status = rt_task_create(&task2, "task2", 0, 99, T_JOINABLE|T_FPU);
	if (status != 0) {
		perror("Criacao do thread 2 falhou.\n");
		exit(1);
	}
	status = rt_task_start(&task2, &periodicThread_handler2, NULL);
	if (status != 0) {
		perror("Inicializacao do thread 2 falhou.\n");
		exit(1);
	}
	rt_task_slice(&task2,100000);
	
	t0 = ((double)(rt_timer_read()))/1000000.0;
	for(n=1;n<=10;++n){
		usleep(200000);	
		gDataLogger_MatfileUpdate(&gDataLogger); // esvazia os buffers no arquivo de log
		
		tempo = ((double)(rt_timer_read()))/1000000.0 - t0;
		printf("\n [%i] Atualizado arquivo Matlab em t = %f s",n,tempo);
	}

	quittask = 1;

	perror("\nEncerrando thread 1...");
	rt_task_join(&task1); 
	perror(" ok\n");
	perror("\nEncerrando thread 2...");
	rt_task_join(&task2); 
	perror(" ok\n");
	
	rt_task_delete(&task1);
	rt_task_delete(&task2);

	// Encerramento do data logger:
	gDataLogger_Close(&gDataLogger);
	
	return 0;
}
