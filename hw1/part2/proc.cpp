#include "proc.hpp"

#include <cstring>
#include <vector>
using namespace std;
#define BUFFER_SIZE 1000 /* Memory page takes 4K for cache*/
#define CHECK_FILE_EXIST(x) (x)

Proc::Proc(string cmd)
{
    this->in_fd = stdin;
    this->out_fd = stdout;
    this->err_fd = stderr;

    if (cmd.back() == ';')
        cmd.pop_back();
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
    if (this->in_fd != stdin)
        fclose(this->in_fd);
    if (this->out_fd != stdout)
        fclose(this->out_fd);
    if (this->err_fd != stderr)
        fclose(this->err_fd);
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
int Proc::doExecute()
{
    int errorCode = 0;
    char tmp[BUFFER_SIZE] = {0};
    /* Append perverious command result to the next command*/
    /* Just like pipe line*/
    while (CHECK_FILE_EXIST(this->in_fd) && !feof(this->in_fd) &&
           fgets(tmp, BUFFER_SIZE, this->in_fd)) {
        this->command.append(tmp);
        memset(tmp, 0, BUFFER_SIZE);
    }
    auto result_fd = popen(this->command.c_str(), "r");

    /*Get File size*/
    fseek(result_fd, 0, SEEK_END);
    auto size = ftell(result_fd);
    fseek(result_fd, 0, SEEK_SET);

    /* Copy result_fd to out_fd*/
    memcpy(this->out_fd, result_fd, size);
    pclose(result_fd);

    if (this->next)
        this->next->in_fd = this->out_fd;
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
    auto _in_fd = (this->prev) ? this->prev->out_fd : stdin;
    auto _out_fd =
        (this->next) ? this->next->in_fd : stdout;  // Here might always stdout
    if (c == ';') {
        setAllIO(_in_fd, stdout, stderr);

    } else if (c == '>') {
        if (this->next == nullptr)
            errorCode = 1;
        this->next->out_fd = fopen(this->next->command.c_str(), "w");
        setAllIO(_in_fd, _out_fd, stderr);

    } else if (c == '<') {
        if (this->next == nullptr)
            errorCode = 1;
        this->next->out_fd = fopen(this->next->command.c_str(), "r");
        setAllIO(this->next->out_fd, stdout, stderr);

    } else if (c == '|') {
        setAllIO(_in_fd, _out_fd, stderr);
    }
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

    ele->commandParser();
    this->size++;
}


/**
 * Execute all the commands and return the status code while exist
 */
int Cmd_q::execute()
{
    auto errorCode = 0;
    for (auto curr = this->head; curr; curr = curr->next) {
        errorCode = curr->doExecute();
    }
    return errorCode;
}