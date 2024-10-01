#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h>
#include<sys/wait.h> 
#include<sys/socket.h>
#include<fcntl.h>
#include<readline/readline.h> 
#include<readline/history.h>

#define MAX_SIZE 1024

FILE * fout;
pid_t bg_pid;
int bg_cnt=0;
pid_t pid;

int main(){
	char user[100];
    	char hostname[100];
    	char cwd[100];
	char prompt[350];
	
	
	while(1){
		fflush(stdout);
		
		//get prompt
		if (getlogin_r(user, sizeof(user)) != 0) {
			perror("getlogin");
			exit(1);
   		}
    		if (gethostname(hostname, sizeof(hostname)) != 0) {
        		perror("gethostname");
        		exit(1);
		}
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			perror("getcwd");
			exit(1);
		}
		//sprintf(prompt, "%s%s%s$ ", user, hostname, cwd);
		//strcpy(prompt, user);
		//strcat(prompt, hostname);
		//strcat(prompt, cwd);
		//printf("prompt: %s\n",prompt);
		char test[150];
		sprintf(test, "%s%s", "/home/", user);
		char str[100];
		strcpy(str,cwd);
		//路徑初始值為 ~ ，也就是 /home/<當前使用者>/
		char init[100]="~";
		char *spilt[2]={"\0","\0"};
		
		if(strcmp(cwd,"/home") == 0){
			sprintf(prompt,"\e[0;32m%s\e[0;0m@\e[0;33m%s\e[0;0m:\e[0;36m%s\e[0;0m$ ",user,hostname,cwd);
		}
		else if(strcmp(cwd,"/") == 0){
			sprintf(prompt,"\e[0;32m%s\e[0;0m@\e[0;33m%s\e[0;0m:\e[0;36m%s\e[0;0m$ ",user,hostname,cwd);
		}
		else{
			spilt[0] = strtok(str, "/");
			//printf("%s",spilt[0]);
			if(strcmp(spilt[0],"home") == 0){
				spilt[1] = strtok(NULL, "/");
				if(strcmp(spilt[1],user) == 0){
					strcat(init,cwd+strlen(spilt[0])+strlen(spilt[1])+2);
					sprintf(prompt,"\e[0;32m%s\e[0;0m@\e[0;33m%s\e[0;0m:\e[0;36m%s\e[0;0m$ ",user,hostname,init); //\033 是 ESC 字元的八進制表示法，也可以用 \e 
				}
				else{
					sprintf(prompt,"\e[0;32m%s\e[0;0m@\e[0;33m%s\e[0;0m:\e[0;36m%s\e[0;0m$ ",user,hostname,cwd);
				}
			}
			else	sprintf(prompt,"\e[0;32m%s\e[0;0m@\e[0;33m%s\e[0;0m:\e[0;36m%s\e[0;0m$ ",user,hostname,cwd);
		}
		int bg_mode=0 ,output_mode=0;
		
		char line[100], copy[100], cut[100];
		char* buffer = &line[0];
		char* fout_file= "\0";
		
		buffer = readline(prompt);
		add_history(buffer);
		
		strcpy(cut,buffer);
		int re, fn;
		int j;
		for(j=0;j<strlen(buffer)-1;j++){
			if(buffer[j]== '>'&& buffer[j+1]!='>'){
				output_mode =1;
				//printf("1");
				fout_file = strtok(cut, "> ");
				fout_file = strtok(NULL, "> ");
				//fn = open(fout_file, O_CREAT| O_WRONLY);
				fout = fopen(fout_file, "w");
				break;
				
			}
			else if(buffer[j]== '>'&& buffer[j+1]=='>'){
				output_mode=1;
				fout_file = strtok(cut, ">> ");
				fout_file = strtok(NULL, ">> ");
				//fn = open(fout_file,O_CREAT |O_APPEND);
				fout = fopen(fout_file, "a");
				break;
			}
		}
		if(output_mode == 1){
			fn = fileno(fout);
			re = dup(1);
			dup2(fn, 1);
		}
		//printf("%s",buffer);
		//buffer[strcspn(buffer, "\n")] = '\0';  //'\n'->'\0'
		strcpy(copy,buffer);
		
		if(buffer[strlen(buffer)-1] == '&')	bg_mode=1; //background execution 
		if(strcmp(buffer, "exit") == 0){
			fflush(stdout);
			exit(0);
		}
		
		//spilt
		char *args[100];
		char *token = strtok(buffer, " ");
		int args_num = 0;
		args[args_num++] = token;//////////////
		
		//Build-in commands
		if(strcmp(args[0], "cd") == 0){
			while (token != NULL) {
				token = strtok(NULL, " ");
				args[args_num++] = token;
				
				//args_num++;
			}
            		args[args_num] = NULL;
			if(args[2] !=NULL){
				printf("cd: too many arguments\n");
				continue;
			}
			if(args[1] == NULL || strcmp(args[1], "~") == 0){
				char temp[100]="/home/";
				strcat(temp,user);
				chdir(temp);
				continue;
			}
			//printf("cwd %s\n",cwd);
			//printf("test %s\n",test);
			//printf("%d\n",strcmp(cwd, test));
			//printf("%d\n",strcmp(args[1], ".."));
			
			if(strcmp(cwd, test) == 0 && strcmp(args[1], "..") == 0){// cd ..
				//printf("test");
				chdir("/home");
				continue;
			}
			//printf("flag");
			if(cwd == "/home" && args[1]==".."){
				chdir("/");
				continue;
			}
			if(args[1][0] == '~'){// cd ~/Desktop
				char temp2[100];
				strcpy(temp2,args[1]);
				token = strtok(temp2, "/");
				token = strtok(NULL, "  ");
				//printf("token: %s",token);
				sprintf(args[1], "%s%s%s%s", "/home/", user, "/", token);
				chdir(args[1]);
				continue;
			}
			else{
				if(chdir(args[1])!=0)	perror("cd");
			}
		}
		else if(strcmp(args[0], "pwd") == 0){
			getcwd(cwd, sizeof(cwd));
			printf("%s\n", cwd);
		}
		else if(strcmp(args[0], "echo") == 0){
			//printf("copy %s\n",copy);
			char arg_cat[MAX_SIZE]="\0";
			strcat(arg_cat,copy+strlen(args[0])+1);
			if(arg_cat[0]=='$') printf("%s\n", getenv("PATH"));
			else printf("%s\n", arg_cat);
		}
		else if(strcmp(args[0], "export") == 0){
			char arg_cat[MAX_SIZE]="\0";
			strcat(arg_cat,copy+strlen(args[0])+1);
			token = strtok(NULL, ":");
			args[args_num++] = token;
			//printf("args[1]: %s\n",args[1]);
			token = strtok(NULL, ":");
			args[args_num++] = token;
			//printf("args[2]: %s\n",args[2]);
			//strcat(arg_cat,arg_cat+strlen(args[1])+1);
			char ori_path[500]="\0";
			char ori_path2[MAX_SIZE]="\0";
			//ori_path=getenv("PATH");
			strcpy(ori_path, getenv("PATH"));
			sprintf(ori_path2,"%s:%s",ori_path,args[2]);
			//strcat(ori_path,args[2]);
			setenv("PATH",ori_path2,1);
			//printf("PATH:%s\n", getenv("PATH"));
		}
		else if(strcmp(args[0], "bg") == 0){ //bg
			pid_t result = waitpid(bg_pid, NULL, WNOHANG);
			if(result == -1){
				perror("waitpid");
			}
			else if(result == 0){
				printf("%d is still running\n",bg_pid);
			}
			else{
				printf("%d is Done\n",bg_pid);
			}
		}
		else{
			args[1]=NULL;
			for(args_num=1;args_num<5;args_num++){
				token = strtok(NULL, " ");
				args[args_num] = token;
			}
			
			pid = fork();
			if(pid <0){
				printf("error Fork failed.\n");
				exit(-1);
			}
			else if(pid == 0){
				execlp(args[0], args[0], args[1], args[2], args[3], args[4], NULL);
			}
			else{
				if(bg_mode == 0)	wait(NULL);
				else{
					printf("background pid:  %d\n",pid);
					bg_pid=pid;
				}	
			}
		}
		
		fflush(stdout);
		if(output_mode ==1){
			dup2(re,1);
			fclose(fout);
		}
	}

}
