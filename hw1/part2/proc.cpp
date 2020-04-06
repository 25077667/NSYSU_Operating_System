#include "proc.hpp"

#include <algorithm>
#include <cstring>
#include <vector>
using namespace std;
#define BUFFER_SIZE 1000 /* Memory page takes 4K for cache*/

static inline void copy2File(FILE *in, FILE *out)
{
    for (char tmp[BUFFER_SIZE] = {0}; !feof(in) && fgets(tmp, BUFFER_SIZE, in);
         memset(tmp, 0, BUFFER_SIZE))
        fputs(tmp, out);
}

/*remove extra <space> and ';'*/
static string removeSpace_semicolon(string str)
{
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
    return str;
}

Proc::Proc(string cmd)
{
    this->in_fd = nullptr;
    this->out_fd = stdout;
    this->err_fd = stderr;
    this->pass = false;

    this->command = cmd;
    this->prev = nullptr;
    this->next = nullptr;
}

Proc::Proc(Proc *_left, string cmd) : Proc(cmd)
{
    this->prev = _left;
    this->next = nullptr;
    if (_left)
        _left->next = this;
}

Proc::Proc(Proc *_left, Proc *_right, string cmd) : Proc(cmd)
{
    this->prev = _left;
    this->next = _right;
    if (_left)
        _left->next = this;
    if (_right)
        _right->prev = this;
}

Proc::~Proc()
{
    if (this->in_fd)
        fclose(this->in_fd);
    if (this->out_fd != stdout)
        fclose(this->out_fd);
    if (this->err_fd != stderr)
        fclose(this->err_fd);
}

const inline FILE *Proc::getOutFd() const
{
    return this->out_fd;
}
const inline FILE *Proc::getInFd() const
{
    return this->in_fd;
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
int Proc::doExecute()
{
    int errorCode = 0;
    if (this->pass) {
        /* redirect to file output */
    } else {
        /* pipe line */
        char tmp[BUFFER_SIZE] = {0};
        /* Append perverious command result to the next command*/
        while ((this->in_fd) && !feof(this->in_fd) &&
               fgets(tmp, BUFFER_SIZE, this->in_fd)) {
            this->command.append(tmp);
            memset(tmp, 0, BUFFER_SIZE);
        }
        auto result_fd = popen(this->command.c_str(), "r");
        if (result_fd != NULL) {
            /* Copy result_fd to out_fd*/
            copy2File(result_fd, this->out_fd);
            pclose(result_fd);

            if (this->next)
                this->next->in_fd = this->out_fd;
        } else {
            errorCode = 65537;  // command not found
        }
    }

    raiseError(errorCode);
    return errorCode;
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
 * rb:
 * "0000 0000 0000 0001" File error
 * 0000 0000 0000 0000 0000 0000 0000 0001: file not exists or permission denied
 * 0000 0000 0000 0001 0000 0000 0000 0001: command not found
 *
 */
void Proc::raiseError(int errorCode)
{
    switch (errorCode) {
    case 1:
        perror("File is not exist or permission denied\n");
        break;
    case 65537:
        perror("command not found");
    default:
        break;
    }
}

void Proc::commandParser()
{
    int errorCode = 0;
    /**
     * This identifier will get the last word to redirection
     *
     * TODO: support stderr redirection, multi-redirection, append
     * string identifier;
     */

    char c = this->command.back();
    auto _in_fd = (this->prev) ? this->prev->out_fd : nullptr;
    auto _out_fd = (this->next) ? this->next->in_fd : stdout;
    if (c == ';') {
        setAllIO(_in_fd, stdout, stderr);
    } else if (c == '>') {
        if (this->next == nullptr)
            errorCode = 1;
        else {
            auto nextFile = removeSpace_semicolon(this->next->command);
            this->next->out_fd = fopen(nextFile.c_str(), "w");
            this->next->pass = true;  // Remove this will double free detected
                                      // in tcache 2, but don;t know why
        }
        setAllIO(_in_fd, this->next->out_fd, stderr);
    } else if (c == '<') {
        if (this->next == nullptr)
            errorCode = 1;
        else {
            auto nextFile = removeSpace_semicolon(this->next->command);
            this->next->out_fd = fopen(nextFile.c_str(), "r");
            this->next->pass = true;
        }
        setAllIO(this->next->out_fd, stdout, stderr);
    } else if (c == '|')
        setAllIO(_in_fd, _out_fd, stderr);
    else
        setAllIO(_in_fd, _out_fd, stderr);
    raiseError(errorCode);
    this->command.back() = ' ';  // replace redirection to ' '
}

/////////////////////Cmd_q zone///////////////////////

Cmd_q::Cmd_q()
{
    this->size = 0;
    this->head = this->tail = nullptr;
}

Cmd_q::~Cmd_q()
{
    auto curr = this->head;
    while (curr) {
        auto nextOne = curr->next;
        delete curr;  // Will invoke the distructor automatically
        curr = nextOne;
    }
    this->size = 0;
}

bool Cmd_q::empty()
{
    return (bool) this->size;
}

void Cmd_q::push_back(Proc *ele)
{
    if (this->head == nullptr) {
        this->head = ele;
        this->tail = ele;
    } else {
        ele->prev = this->tail;
        this->tail->next = ele;
        this->tail = ele;
    }

    // ele->commandParser();
    this->size++;
}


/**
 * Execute all the commands and return the status code while exist
 */
int Cmd_q::execute()
{
    for (auto curr = this->head; curr; curr = curr->next)
        curr->commandParser();
    auto errorCode = 0;
    for (auto curr = this->head; curr; curr = curr->next) {
        errorCode = curr->doExecute();
    }
    return errorCode;
}