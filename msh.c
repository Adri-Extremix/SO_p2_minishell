//P2-SSOO-22/23

// MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>         /* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_COMMANDS 8


// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];


void siginthandler(int param)
{
	printf("****  Saliendo del MSH **** \n");
	//signal(SIGINT, siginthandler);
	exit(0);
}


/* Timer */
pthread_t timer_thread;
unsigned long  mytime = 0;

void* timer_run ( )
{
	while (1)
	{
		usleep(1000);
		mytime++;
	}
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
	/**** Do not delete this code.****/
	int end = 0; 
	int executed_cmd_lines = -1;
	char *cmd_line = NULL;
	char *cmd_lines[10];

	if (!isatty(STDIN_FILENO)) {
		cmd_line = (char*)malloc(100);
		while (scanf(" %[^\n]", cmd_line) != EOF){
			if(strlen(cmd_line) <= 0) return 0;
			cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
			strcpy(cmd_lines[end], cmd_line);
			end++;
			fflush (stdin);
			fflush(stdout);
		}
	}

	pthread_create(&timer_thread,NULL,timer_run, NULL);

	/*********************************/

	char ***argvv = NULL;
	int num_commands;
        char *cero = "0";
        setenv("Acc",cero,1);
        
        
	while (1) 
	{
		int status = 0;
		int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

		// Get command
		//********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
		executed_cmd_lines++;
		if( end != 0 && executed_cmd_lines < end) {
			command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
		}
		else if( end != 0 && executed_cmd_lines == end){
			return 0;
		}
		else{
			command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
		}
		//************************************************************************************************


		/************************ STUDENTS CODE ********************************/
		if (command_counter > 0) {
			if (command_counter > MAX_COMMANDS){
				printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
			}
			else {
			        
			        
			        
			        if (strcmp(argvv[0][0],"mycalc") == 0)
			        {
			                
			                if((argvv[0][1] == NULL) || (argvv[0][2] == NULL) || (argvv[0][3] == NULL))
			                {
                                    printf("[ERROR] La estructura del comando es mycalc <operando_1><add/mul/div><operando_2>\n");     
			                }
			                else
			                {
			                        int operando_incorrecto = 0;
			                        int no_operador = 0;
		                            // Comprobación de digitos
		                            for (int i = 1;i <=3; i = i + 2)
		                            {       
		                                    //Si es negativo
                                            if (argvv[0][i][0] == '-')
	                                        {
	                                                for(int j = 1;j < strlen(argvv[0][i]);j++)
	                                                {
	                                                        if (isdigit(argvv[0][i][j]) == 0)
	                                                        {
		                                                            operando_incorrecto = 1;    
	                                                        }
	                                                }
	                                        }
	                                        // Si es positivo
	                                        else
	                                        {
	                                               /* for(int j = 0;j < strlen(argvv[0][i]);j++)
	                                                {   
	                                                        if (isdigit(argvv[0][i][j]) == 0)
	                                                        {
		                                                            operando_incorrecto = 1;    
	                                                        }
	                                                }
	                                                */
	                                                if (isdigit(argvv[0][i] == 0))
	                                                {
	                                                        operando_incorrecto = 1;
	                                                }
	                                        } 
	                                 
		                            }
			                        

	                                int operando1 = atoi(argvv[0][1]);
	                                char *operador = argvv[0][2];
	                                int operando2 = atoi(argvv[0][3]);
	                                int resultado;
	                                int resto;
	                                int acc_int;
	                                acc_int = atoi(getenv("Acc"));
	                                char acc[10];
	                                if (strcmp(operador,"add") == 0 && operando_incorrecto == 0)
	                                {
	                                        // SUMA
	                                        resultado = operando1 + operando2;
	                                        acc_int = acc_int + resultado;
	                                        fprintf(stderr,"[OK] %i + %i = %i; Acc %i\n", operando1, operando2, resultado,acc_int);
	                                        
	                                        sprintf(acc,"%i",acc_int);
	                                        setenv("Acc",acc,1);
	                                }
	                                else if (strcmp(operador,"mul") == 0 && operando_incorrecto == 0)
	                                {
	                                        //Mul
	                                        resultado = operando1 * operando2;
	                                        fprintf(stderr,"[OK] %i * %i = %i\n", operando1, operando2, resultado);
	                                }
	                                else if (strcmp(operador,"div") == 0 && operando_incorrecto == 0)
	                                {
	                                        if(operando2 == 0)
	                                        {
	                                                printf("[ERROR] No se puede dividir entre 0\n");
	                                        }
	                                        // div
	                                        resultado = operando1 / operando2;
	                                        resto = operando1 % operando2;
	                                        fprintf(stderr,"[OK] %i / %i = %i; Resto %i\n", operando1, operando2, resultado, resto);
	                                        
	                                }
	                                else
	                                {
	                                        no_operador = 1;
	                                }
	                                if(no_operador == 1)
	                                {
	                                        printf("[ERROR] La estructura del comando es mycalc <operando_1><add/mul/div><operando_2>\n");
	                                }
			                        
			                }
			        }
			        else if(strcmp(argvv[0][0],"mytime") == 0)
			        {
			        int mitiempo;
			        mitiempo = mytime/1000;
			        int horas;
			        int minutos;
			        int segundos;
			        horas = mitiempo / 3600;
			        mitiempo = mitiempo - horas * 3600;
			        minutos = mitiempo / 60;
			        mitiempo = mitiempo - minutos * 60;
			        segundos = mitiempo;
			        fprintf(stderr,"%.2i:%.2i:%.2i\n",horas,minutos,segundos);			        
			        }
			        
			        
			        
			        
			        // Para mandatos simples
			        else if (command_counter == 1)
			        {
			                int pid;
			                pid = fork();
			                switch(pid) 
			                {
                                                case -1:
                                                        perror("Error en la creación de procesos");
                                                        exit(0);
                                                case 0:
                                                
                                                        if (strcmp(filev[2],"0") != 0)
                                                        {
                                                                if (close(2) == -1)
                                                                {
                                                                        perror("Error en el cierre del descriptor 2");
                                                                        exit(0);
                                                                }
                                                                else
                                                                {
                                                                        if (open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666) == -1)
                                                                        {
                                                                                perror("Error en la apertura del descriptor de salida de errores");
                                                                                exit(0);
                                                                        }
                                                                }
                                                        }
                                                        
                                                        if (strcmp(filev[0],"0") != 0)
                                                        {
                                                                if (close(0) == -1)
                                                                {
                                                                        perror("Error en el cierre del descriptor 0");
                                                                        exit(0);
                                                                }
                                                                else
                                                                {
                                                                        if (open(filev[0],O_RDONLY) == -1)
                                                                        {
                                                                                perror("Error en la apertura del descriptor de entrada");
                                                                                exit(0);
                                                                        }
                                                                }
                                                        
                                                        }
                                                        if (strcmp(filev[1],"0") != 0)
                                                        {
                                                                if (close(1) == -1)
                                                                {
                                                                        perror("Error en el cierre del descriptor 1");
                                                                        exit(0);
                                                                }
                                                                else
                                                                {
                                                                        if (open(filev[1],O_WRONLY|O_CREAT|O_TRUNC,0666) == -1)
                                                                        {
                                                                                perror("Error en la apertura del descriptor de salida");
                                                                                exit(0);
                                                                        }
                                                                }
                                                        
                                                        }
                                                        
                                                
                                                        execvp(argvv[0][0],argvv[0]);
                                                        perror("Error mandato no detectado");
                                                        exit(0);
                                                        
                                                default:
                                                        if (in_background == 0)
                                                        {       
                                                                while (wait(0) != pid){}
                                                        }
                                                        else
                                                        {
                                                        printf("%i\n",pid);
                                                        }
                                                        
                                } //fin switch
			        }
			        else
			        {
			                // Array de procesos
			                int pid[command_counter];
			                // Array de descriptores de fichero para cada pipe, hay n-1 pipes
			                int fd[command_counter-1][2];
			                // for que pasa por todos los procesos
			                for(int i = 0; i< command_counter;i++)
			                {
			                        // Si es el último proceso
			                        if (i+1 == command_counter)
			                        {
			                                pid[i] = fork();
			                                
			                                switch(pid[i]) 
	                                                {
                                                                case -1:
                                                                perror("Error en la creación de procesos");
                                                                exit(-1);
                                                                case 0:
                                                                
                                                                // Redirección de salida de errores
                                                                if (strcmp(filev[2],"0") != 0)
                                                                {
                                                                        if (close(2) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor 2");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                                if (open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666) == -1)
                                                                                {
                                                                                        perror("Error en la apertura del descriptor de salida de errores");
                                                                                        exit(0);
                                                                                }
                                                                        }
                                                                }
                                                                
                                                                // Redirección de salida
                                                                if (strcmp(filev[1],"0") != 0)
                                                                {
                                                                        if (close(1) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor 1");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                                if (open(filev[1],O_WRONLY|O_CREAT|O_TRUNC,0666) == -1)
                                                                                {
                                                                                        perror("Error en la apertura del descriptor de salida");
                                                                                        exit(0);
                                                                                }
                                                                        }
                                                        
                                                                }
                                                                
                                                                // Se conecta con el anterior proceso
                                                                if(close(0) == -1)
                                                                {
                                                                        perror("Error en el cierre del descriptor 0");
                                                                        exit(0);
                                                                }
                                                                else
                                                                {
                                                                        if(dup(fd[i-1][0]) == -1)
                                                                        {
                                                                                perror("Error al duplicar la pipe");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                                if(close(fd[i-1][0]) == -1)
                                                                                {
                                                                                        perror("Error en el cierre del descriptor de lectura de la pipe");
                                                                                        exit(0);
                                                                                }
                                                                        }
                                                                        
                                                                }
                                                                
                                                                
                                                                execvp(argvv[i][0],argvv[i]);
                                                                perror("Error mandato no detectado");
                                                                exit(0);
                                                                default:
                                                                if(close(fd[i-1][0]) == -1)
                                                                {
                                                                        perror("Error en el cierre del descriptor de lectura de la pipe");
                                                                        exit(0);
                                                                }
	                                                }
			                        }
			                        // Si no es el último proceso
			                        else
			                        {
			                                pipe(fd[i]);
			                                pid[i] = fork();
			                                // Si es el primer proceso
			                                if(i == 0)
			                                {
			                                        switch(pid[i]) 
			                                        {
                                                                        case -1:
                                                                        perror("Error en la creación de procesos");
                                                                        exit(-1);
                                                                        case 0:
                                                                        // Redirección de salida de errores
                                                                        if (strcmp(filev[2],"0") != 0)
                                                                        {
                                                                                if (close(2) == -1)
                                                                                {
                                                                                        perror("Error en el cierre del descriptor 2");
                                                                                        exit(0);
                                                                                }
                                                                                else
                                                                                {
                                                                                        if (open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666) == -1)
                                                                                        {
                                                                                                perror("Error en la apertura del descriptor de salida de errores");
                                                                                                exit(0);
                                                                                        }
                                                                                }
                                                                        }
                                                                        
                                                                        // Redirección de entrada
                                                                        if (strcmp(filev[0],"0") != 0)
                                                                        {
                                                                                if (close(0) == -1)
                                                                                {
                                                                                        perror("Error en el cierre del descriptor 0");
                                                                                        exit(0);
                                                                                }
                                                                                else
                                                                                {
                                                                                        if (open(filev[0],O_RDONLY) == -1)
                                                                                        {
                                                                                                perror("Error en la apertura del descriptor de entrada");
                                                                                                exit(0);
                                                                                        }
                                                                                }
                                                        
                                                                        }
                                                                        
                                                                        // Se conecta con el siguiente proceso
                                                                        if(close(1) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor 1");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                                if(dup(fd[i][1]) == -1)
                                                                                {
                                                                                        perror("Error al duplicar la pipe");
                                                                                        exit(0);
                                                                                }
                                                                                else
                                                                                {
                                                                                        if(close(fd[i][1]) == -1)
                                                                                        {
                                                                                                perror("Error en el cierre del descriptor de escritura de la pipe");
                                                                                                exit(0);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                                if(close(fd[i][0]) == -1)
                                                                                                {
                                                                                                        perror("Error en el cierre del descriptor de lectura de la pipe");
                                                                                                        exit(0);
                                                                                                }
                                                                                        }
                                                                                        
                                                                                }
                                                                                
                                                                        
                                                                        }
                                                                        
                                                                        execvp(argvv[i][0],argvv[i]);
                                                                        perror("Error mandato no detectado");
                                                                        exit(0);
                                                                        default:
                                                                        if(close(fd[i][1]) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor de escritura de la pipe");
                                                                                exit(0);
                                                                        }
			                                        }
		                                        }
		                                        // Si es no es el primero ni el último proceso
			                                else
			                                {
			                                        switch(pid[i]) 
			                                        {
                                                                        case -1:
                                                                        perror("Error en la creación de procesos");
                                                                        exit(-1);
                                                                        case 0:
                                                                        // Redirección de salida de errores
                                                                        if (strcmp(filev[2],"0") != 0)
                                                                        {
                                                                                if (close(2) == -1)
                                                                                {
                                                                                        perror("Error en el cierre del descriptor 2");
                                                                                        exit(0);
                                                                                }
                                                                                else
                                                                                {
                                                                                        if (open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666) == -1)
                                                                                        {
                                                                                                perror("Error en la apertura del descriptor de salida de errores");
                                                                                                exit(0);
                                                                                        }
                                                                                }
                                                                        }
                                                                        
                                                                        // Se conecta con el anterior proceso
                                                                        if(close(0) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor 0");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                                if(dup(fd[i-1][0]) == -1)
                                                                                {
                                                                                        perror("Error al duplicar la pipe");
                                                                                        exit(0);
                                                                                }
                                                                                else
                                                                                {
                                                                                        if(close(fd[i-1][0]) == -1)
                                                                                        {
                                                                                                perror("Error en el cierre del descriptor de lectura de la pipe");
                                                                                                exit(0);
                                                                                        }
                                                                                }
                                                                                
                                                                        }
                                                                        
                                                                        
                                                                        // Se conecta con el siguiente proceso
                                                                        if(close(1) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor 1");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                                if(dup(fd[i][1]) == -1)
                                                                                {
                                                                                        perror("Error al duplicar la pipe");
                                                                                        exit(0);
                                                                                }
                                                                                else
                                                                                {
                                                                                        if(close(fd[i][1]) == -1)
                                                                                        {
                                                                                                perror("Error en el cierre del descriptor de escritura de la pipe");
                                                                                                exit(0);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                                if(close(fd[i][0]) == -1)
                                                                                                {
                                                                                                        perror("Error en el cierre del descriptor de lectura de la pipe");
                                                                                                        exit(0);
                                                                                                }
                                                                                        }
                                                                                        
                                                                                }
                                                                                
                                                                        }
                                                                        
                                                                        execvp(argvv[i][0],argvv[i]);
                                                                        perror("Error mandato no detectado\n");
                                                                        exit(0);
                                                     
                                                                        default:
                                                                        if(close(fd[i][1]) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor de escritura de la pipe");
                                                                                exit(0);
                                                                        }
                                                                        else
                                                                        {
                                                                        if(close(fd[i-1][0]) == -1)
                                                                        {
                                                                                perror("Error en el cierre del descriptor de lectura de la pipe");
                                                                                exit(0);
                                                                        }
                                                                        }
                                                                        
			                                        }
			                                }
			                        }
			                }
			                if (in_background == 0)
                                        {
			                        while(wait(0) != pid[command_counter-1]){}
		                        }
		                        else
		                        {
		                        printf("%i\n",pid[command_counter-1]);
		                        }
		                     
			        }
				
			}
		}
	}
	
	return 0;
}




