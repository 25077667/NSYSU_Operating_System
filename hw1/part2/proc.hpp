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
    void setAllIO(FILE *, FILE *, FILE *);
    void setSIO(string, string, string);
    int doExecute(vector<FILE *> &);
    void commandParser();
    Proc *prev, *next;
    bool pass;  // If this command just a file, needn't to execute.

private:
    string command, in_s, out_s, err_s;
    int status;
    FILE *out_fd, *err_fd;
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
    /* Execute all commands in this queue*/
    int execute(vector<FILE *> &);
    // We do not need pop
    Proc *head, *tail;
};

#endif
