#ifndef PROC_H
#define PROC_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Proc_fd
{
public:
    Proc_fd();
    ~Proc_fd();
    FILE *get_fd(int index);
    int set_pipe(int _in);
    int set_pipe(FILE *_in);
    FILE *redirect(string filename, string mode);
    void read(string &);
    void write(string, int);

private:
    /**
     * About this file descriptor array
     * fd[0] is for input
     * fd[1] is for output
     * fd[2] is for error
     */
    FILE *fd[3];
};

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
    bool pass;

private:
    string command;
    int status;
    Proc_fd fd;
    // FILE *in_fd, *out_fd, *err_fd;
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
