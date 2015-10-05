#include<stdio.h>
#include<stdlib.h>
#include<curses.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

int bg=0;

void prmpt(char *prompt)
{
	int p,q,i,j;
	char person[100],system[100],drt[50];
	p=getlogin_r(person,10000);
	q=gethostname(system,1000);
	getcwd(drt,50);
	for(i=0,j=0;person[j]!='\0';i++,j++)
		prompt[i]=person[j];
	prompt[i++]='@';
	for(j=0;system[j]!='\0';i++,j++)
		prompt[i]=system[j];
	prompt[i++]=':';
	prompt[i++]='~';
	prompt[i++]='$';
	for(j=0;drt[j]!='\0';i++,j++)
		prompt[i]=drt[j];
	prompt[i++]='>';
	prompt[i++]='>';
	prompt[i++]=' ';
	prompt[i]='\0';
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

int reparse(char *l,char **arg)
{
	int i=0;
	arg[0]=strtok(l,"<>");
	while(arg[i] != NULL)
	{
		i++;
		arg[i]=strtok(NULL,"<>");
	}
	return i;
}
void piping(char **list,int len)
{
	int i,j,status;
	int inp=0,out=0;
	int inpind=0,outind=0;
	int infile,outfile;
	int fd[1000][2];
	for(i=0;i<len;i++)
	{
		char cmd[100];
		strcpy(cmd,*list);
		int x=0;
		while(cmd[x]!='\0')
		{
			if(cmd[x] == '<')
			{
				inp=1;
				inpind=x;
			}
			else if(cmd[x] == '>')
			{
				out=1;
				outind=x;
			}
			x++;
		}
		char *rearg[6400];
		int r=0;
		rearg[0]=strtok(cmd,"<>");
		while(rearg[r]!=NULL)
		{
			//printf("%s\n",rearg[r]);
			r++;
			rearg[r]=strtok(NULL,"<>");
		}
		//printf("%d\n",r);
		//sleep(2);
		//printf("%d\n",r);
		if(r==2)
		{
			rearg[1]=strtok(rearg[1]," ");
		}
		if(r==3)
		{
			rearg[2]=strtok(rearg[2]," ");
		}
		if(inpind < outind)
		{
			if(inp==1)
			{
				infile=open(rearg[1],O_RDONLY);
				outfile=open(rearg[2],O_CREAT| O_WRONLY, 0777);
			}
			else if(inp==0)
					outfile=open(rearg[1],O_CREAT| O_WRONLY, 0777);
		}
		else if(inpind > outind)
				infile=open(rearg[1],O_RDONLY);
		char *arg[6400];
		char *newcmd=cmd;
		j=0;
		//printf("%d\n",r);
		if(rearg[1]!=NULL)
			newcmd=rearg[0];
		//printf("$$%s$$\n",newcmd);
		arg[0]=strtok(newcmd," -");
		while(arg[j] != NULL)
		{
			//printf("$%s$\n",arg[j] );
			j++;
			arg[j]=strtok(NULL," ");
		}/*
		while(*arg!=NULL)
		{
			printf("%s\n",*arg);
			*arg++;
		}*/
		if(pipe(fd[i])<0)
		{
			perror("Pipe was not implemented\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			pid_t pid,pgid;
			pid=fork();
			if(pid==0)
			{
				if(i==0 && len == 1)
				{
					if(inp==1)
					{
						//printf("HERE\n");
						//infile=open(rearg[1],O_RDONLY);
						dup2(infile,0);
						close(infile);
						//close(fd[i][0]);
						//close(fd[i][0]);
						inp=0;
					}
					if(out==1)
					{
						//printf("IN HERE\n");
						//outfile=open(rearg[2],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
						dup2(outfile,1);
						close(outfile);
						//close(fd[i][1]);
						out=0;
					}
				}
				else
				{
					
					if(i!=0)
					{
						//printf("THIS\n");
						if(inp==1)
						{
							dup2(infile,0);
							close(infile);
							inp=0;
						}
						else if(dup2(fd[i-1][0],0)<0)
						{
							perror("Dup2 Error\n");
							exit(EXIT_FAILURE);
						}
						if(out==1)
						{
							perror("OUT HERE");
							dup2(outfile,1);
							close(outfile);
							out=0;
						}
						//close(fd[i-1][0]);
						//close(fd[i-1][1]);
						//close(fd[i][1]);
						//close(fd[i][0]);
						//printf("THAT\n");
					}
					if(i!=len-1)
					{
						perror("IN HERE");
						if(inp==1)
						{
							printf("INP\n");
							dup2(infile,0);
							close(infile);
							inp=0;
						}
						if(out==1)
						{
							printf("OUT\n");
							dup2(outfile,1);
							close(outfile);
							out=0;
						}
						if(dup2(fd[i][1],1)<0)
						{
							perror("DUP2 Error\n");
							exit(EXIT_FAILURE);
						}
						else
						{
							perror("HERE");
							//	close(fd[i][0]);
						}
					}
				}
				//printf("reached Here\n");
				if (strcmp(arg[0],"cd") == 0)
					chdir("/home");
				if (strcmp(arg[0],"exit")==0)
					exit(0);
				//printf("IN HERE\n");
				perror("BEFORE EXEC");
				int a=execvp(arg[0],arg);
				//perror("Execvp");
				if(a< 0)
				{
					printf("NO SUCH COMMAND\n");
					exit(-1);
				}
			}	
		}

	*list++;
	}

}
int getcommand(char *cmd,char **arg,char *prompt)
{
	cmd=readline(prompt);	//INPUT
	add_history(cmd);				/* Split for pipe*/
	int i=0,j,len;
	char *list[1000];
	list[0]=strtok(cmd,"|");
	while(list[i]!=NULL)
	{
		i++;
		list[i]=strtok(NULL,"|");
	}
	/*END of PIPE SPLIT CODE*/
	if(i==1)
	{
		/*char *check[100];
		char try[100];
		strcpy(try,*list);
		int ss;
		if(ss=reparse(try,check) > 1)
		{
			piping(list,1);
		}
		*/
		char try[100];
		strcpy(try,*list);
		len=newparse(try,arg);
		if(arg[0]==NULL)
			return 0;
		if(arg[len-1][0] == '&')
		{
			bg=1;
			arg[len-1]='\0';
		}
		if (strcmp(arg[0],"cd") == 0 || strcmp(arg[0],"exit") == 0)
				return 1;
		else
		{
				piping(list,i);
				return 2;
		}
	}
	else
	{
		piping(list,i);
		return 2;
	}
}

int main(int argc)
{
	while(1)
	{
		pid_t pid,cpid;
		char cmd[100000];
		char prompt[5000];
		int len=10000;
		int type,status,p,q;
		int x;
		char *arg[6400];
		prmpt(prompt);
		type=getcommand(cmd,arg,prompt);
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
				else
					printf("safds\n");
			}
			else
			{
				if(bg==1)
				{
					x=waitpid(-1,&status,WNOHANG);
					if (x>0)
					{
						printf("CHILD WITH PID %d HAS EXITED\n",x);
					}
				}
				else
					waitpid(pid,&status,WUNTRACED||WNOHANG);
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
		if(type==2)
			waitpid(pid,&status,WUNTRACED);
	}
	return 0;
}
