#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

int bg=0;
char prompt[5000];

void read(char *cmd)
{
	while((*cmd=getchar())!='\n')
		cmd++;
	*cmd=0;
}

void newread()
{
		int p,q,i,j;
		char person[100],system[100],drt[50];
		p=getlogin_r(person,10000);
		q=gethostname(system,1000);
		getcwd(drt,50);
		printf("%s@%s:~$%s",person,system,drt);
		printf(">> ");
}
int newparse(char *l,char **arg)
{
	int i=0;
	arg[0]=strtok(l," -");
	while(arg[i] != NULL)
	{
		i++;
		arg[i]=strtok(NULL," ");
	}
	return i;
}

int getcommand(char *cmd,char **arg)
{
	read(cmd);		//INPUT
	int len=newparse(cmd,arg);
	if(arg[0]==NULL)
		return 0;
	if(strcmp(arg[len-1],"&")==0)
	{
		bg=1;
		arg[len-1]='\0';
	}
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
	int x;
	char *arg[6400];
	while(1)
	{
		newread();
		type=getcommand(cmd,arg);
		if (type == 0)  // NEW PROCESS
		{
			if(arg[0]==NULL)
				continue;
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
				if(bg==1)
				{
					/*while((x=waitpid(-1,&status,WNOHANG))>0)
					{
						if (x!=0 & x!=-1)
						{
							if(WIFEXITED(status))
								printf("CHILD WITH PID %d HAS EXITED\n",x);
						}

					}*/
				}
				else
					waitpid(pid,&status,WUNTRACED);
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
