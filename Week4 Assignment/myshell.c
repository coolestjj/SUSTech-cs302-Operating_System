#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include<stdlib.h> 
#include <pwd.h>

char line[256];
 
char args[3][20];
 
// 命令参数个数
int arg_number;
 
void exec() {
    int pid = fork();
    if (pid == 0) {
    	if (arg_number == 2) {
            char* argument_list[] = {args[0], args[1], NULL};
            execvp(args[0], argument_list);
        }
        else {
            char* argument_list[] = {args[0], NULL, NULL};
            execvp(args[0], argument_list);
        }
    }
    else {
        wait(NULL);
    }  
}
 
// cd
void exec_cd() {
    chdir(args[1]); 
}

int main() {    
    
    char hostname[64];
    gethostname(hostname, sizeof(hostname));
    
    char path[128];
    getcwd(path, 128);
    
    struct  passwd *pwd;
    pwd = getpwuid(getuid());

    printf("\n%s@%s:%s $", pwd->pw_name, hostname, path);
    
    while (fgets(line, 256, stdin)) {
        arg_number = sscanf(line, "%s%s%s", args[0], args[1], args[2]);
        if (strcmp(args[0], "exit") == 0) {
            break;
        }
        else if (strcmp(args[0], "cd") == 0) {
            exec_cd();
        }
        else {
            exec();
        }
        
        char hostname[64];
    	gethostname(hostname, sizeof(hostname));
    
    	char path[128];
    	getcwd(path, 128);
    	
    	struct  passwd *pwd;
        pwd = getpwuid(getuid());

    	printf("\n%s@%s:%s $", pwd->pw_name, hostname, path);
    }
 
    return 0;
}