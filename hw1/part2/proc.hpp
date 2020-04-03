#ifndef PROC_H
#define PROC_H
#include <string>
#include <iostream>

class Proc{

    public:
        void setSTDIN(bool);
        void setSTDOUT(bool);
        void setSTDERR(bool);
    private:
        string command, in_buf, out_buf;
        int status;
        bool doIn, doOut, doErr;
};

#endif
