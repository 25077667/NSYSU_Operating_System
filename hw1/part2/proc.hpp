#ifndef PROC_H
#define PROC_H
#include <iostream>
#include <string>
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
    Proc(Proc *, string);
    Proc(Proc *, Proc *, string);
    ~Proc();
    void setSTDIN(FILE *);
    void setSTDOUT(FILE *);
    void setSTDERR(FILE *);
    void setAllIO(FILE *, FILE *, FILE *);
    int doExecute();
    void commandParser();
    Proc *prev, *next;

private:
    string command;
    int status;
    FILE *in_fd, *out_fd, *err_fd;
    void raiseError(int);
};


/**
 * A queue of Proc
 * Can inspire all the cammands and execute
 */
class Cmd_q
{
public:
    Cmd_q();
    ~Cmd_q();

    void push_back(Proc *);
    bool empty();
    /* Execute all commands in this queue*/
    int execute();
    // We do not need pop
    int size;
    Proc *head, *tail;
};

#endif
