#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv, char **envp){
    printf("YOU ARE IN MY SHELL!\n\n");
    return execve("/bin/dash", argv, envp);
}
