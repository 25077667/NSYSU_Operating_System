#ifndef PROC_H
#define PROC_H
#include <iostream>
#include <string>
using namespace std;

class Proc
{
public:
    Proc();
    void setSTDIN(FILE *);
    void setSTDOUT(FILE *);
    void setSTDERR(FILE *);
    void setAllIO(FILE *, FILE *, FILE *);
    void applyIO(FILE *, FILE *);
    FILE *doExecute();  // Pass file descriptors to destination
    void redirection(FILE *, FILE *);


private:
    string command, in_buf, out_buf, err_buf;
    int status;
    FILE *in_fd, *out_fd, *err_fd;
    void raiseError(int);
};

#endif
