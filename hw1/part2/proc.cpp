#include "proc.hpp"
#include <cstring>
#include <vector>
using namespace std;

Proc::Proc()
{
    this->in_fd = this->out_fd = this->err_fd = NULL;
}

void Proc::setSTDIN(FILE *set)
{
    this->in_fd = set;
}

void Proc::setSTDOUT(FILE *set)
{
    this->out_fd = set;
}

void Proc::setSTDERR(FILE *set)
{
    this->err_fd = set;
}

void Proc::setAllIO(FILE *_in, FILE *_out, FILE *_err)
{
    this->in_fd = _in;
    this->out_fd = _out;
    this->err_fd = _err;
}

/**
 * Will use the input file descriptor straightly
 * And create new output file descriptot to next process
 *
 * TODO: stderr not work now!!
 *
 * @return: the output file descriptor(read only)
 */
FILE *Proc::doExecute()
{
    char tmp[1000] = {0};
    /* Append perverious command result to the next command*/
    /* Just like pipe line*/
    while (fscanf(this->in_fd, tmp) != EOF)
        this->command.append(tmp);

    return popen(this->command.c_str(), "r");  // The out_fd (read only)
}

/**
 * This function actually do the I/O stream,
 * send and receive streams.
 */
void Proc::applyIO(FILE *source, FILE *distiantion)
{
    int errorCode = 0;
    if (source) {
        this->in_fd = source;
        char buf[10000];
        while (fscanf(source, buf) != EOF) {
            this->in_buf.append(buf);
        }
        errorCode = 0;
    }
    if (distiantion) {
        fprintf(distiantion, this->out_buf.c_str());
        errorCode = 0;
    }
    raiseError(errorCode);
}

/**
 * Error code type info:
 *
 * 0000 0000 0000 0000 0000 0000 0000 0000: No error
 * split this int to left 2 byte and right 2 byte
 * left 2 bytes are error detial, using `lb` to description
 * right 2 btyes are error class, using `rb` to description
 *
 * From the right to the left speaking
 *
 */
void Proc::raiseError(int errorCode)
{
    if (errorCode) {
    }
}

inline void redirection(FILE *in_fd, FILE *out_fd)
{
    fprintf(out_fd, in_fd);
}