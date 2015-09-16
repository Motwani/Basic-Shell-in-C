#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
void parse(char *l, char **arg)		//PARSEING THE STRING
{
	while (*l != '\0') 
	{       			
		while (*l == ' ' || *l == '\t' || *l == '\n')
			*l++ = '\0';     
		*arg++ = l; 
		while (*l != '\0' && *l != ' ' && *l != '\t' && *l != '\n') 
			l++;            
	}
	*arg = '\0';              
}


int getcommand(char *cmd,char **arg)
{
	//scanf("%[^\n]s",cmd);
	gets(cmd);		//INPUT TAKEN AS STRING
	parse(cmd,arg);
	if (strcmp(arg[0],"cd") == 0 || strcmp(arg[0],"exit") == 0)
		return 1;
	else
		return 0;
}

int main(int argc)
{
	pid_t pid,cpid;
	char cmd[100000];
	int len=10000;
	int type,status,p,q;
	char person[10000],system[10000],drt[500];
	char *arg[6400];
	while(1)
	{
		p=getlogin_r(person,10000);
		q=gethostname(system,1000);
		getcwd(drt,50);
		printf("%s@%s:~$%s",person,system,drt);
		printf(">> ");
		type=getcommand(cmd,arg);
		if (type == 0)  // NEW PROCESS
		{
			pid=fork();
			if(pid == 0) 		//CHILD PROCESS
			{
				if (execvp(*arg,arg) < 0 )
				{
					printf("NO SUCH COMMAND\n");
					exit(-1);
				}
			}
			else
			{
				do {
					cpid = waitpid(pid,&status,WUNTRACED);
				}while(!WIFEXITED(status) && !WIFSIGNALED(status));
			}
		}
		else if(type == 1) 		// SYSTEM PROCESS
		{
			if (strcmp(arg[0],"exit") == 0)
				return 0;
			else if(arg[1] == 0)
				chdir("/home");
			else
				chdir(arg[1]);
		}
	}
	return 0;
}
