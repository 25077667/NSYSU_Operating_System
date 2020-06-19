#ifndef PROC_H
#define PROC_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/**
 * A single command handle a process.
 * Using linked-list to connect the source and drain
 * Using the FILE* to pass the file descriptor
 */
class Proc
{
public:
    Proc(string);
    ~Proc();
    void setSIO(string, string, string);
    int doExecute(vector<FILE *> &);
    void commandParser();
    Proc *prev, *next;

    // If this command just a file, needn't to execute (pass)
    // If this command have a pipe, will doPipe
    bool pass, doPipe;

private:
    string command, in_s, out_s, err_s;
    int status;
    FILE *in_fd, *out_fd, *err_fd;
    void raiseError(int);
};


/**
 * A queue of Proc
 * Can ripple executing commands
 */
class Cmd_q
{
public:
    Cmd_q();
    ~Cmd_q();

    void push_back(Proc *);
    /* Execute all commands in this queue*/
    int execute(vector<FILE *> &);
    // We do not need pop, will remove all while calling distructor
    Proc *head, *tail;
};

#endif
