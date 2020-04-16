#include "proc.hpp"

#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;
#define BUFFER_SIZE 1000 /* Memory page takes 4K for cache*/
#define ERR_FILE_NOT_EXIST 1
#define ERR_CMD_NOT_FOUND 65537
#define ERR_AMBIGUOUS_CMD 131073
#define ERR_WRONG_FD 196609
#define ERR_WRONG_FILE_MODE 262145
#define ERR_PIPE 2

extern "C" {
#include "popen.h"
}


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

/**
 * Will close the file to sent the end of line
 */
static inline void string2out(string s, FILE *out)
{
    fprintf(out, "%s", s.c_str());
    fprintf(out, "%c", 3);
    fflush(out);
}

/**
 * This function will not close your file
 */
static inline string file2String(FILE *f)
{
    string s;
    char tmp[BUFFER_SIZE] = {0};
    while (f && !feof(f) && fread(tmp, sizeof(char), BUFFER_SIZE, f)) {
        s.append(tmp);
        memset(tmp, 0, BUFFER_SIZE);
    }
    return s;
}

Proc_fd::Proc_fd()
{
    this->fd[0] = stdin;
    this->fd[1] = stdout;
    this->fd[2] = stderr;
}

Proc_fd::~Proc_fd()
{
    if (this->fd[0] != stdin)
        fclose(this->fd[0]);
    if (this->fd[1] != stdout)
        fclose(this->fd[1]);
    if (this->fd[2] != stderr)
        fclose(this->fd[2]);
}

FILE *Proc_fd::get_fd(int index)
{
    if (index < 0 || index > 2)
        return NULL;
    return this->fd[index];
}

// TODO: here might change to "pipe sender" or somthing
int Proc_fd::set_pipe(int _sender_i, int _receiver_i)
{
    int errorCode = 0;
    int fd[2];
    if (pipe(fd) < 0) {
        errorCode = ERR_PIPE;
    } else {
        dup2(fd[0], STDIN_FILENO);
        dup2(fd[0], _sender_i);
    }
    return errorCode;
}

int Proc_fd::set_pipe(FILE *_sender_fd, FILE *_receiver_fd)
{
    return set_pipe(fileno(_sender_fd), fileno(_receiver_fd));
}

/**
 * Give R/W method for current "Proc"
 * If you want '>' to a file, set mode to be "w"
 * If you want '<' from a file, set mode to be "r"
 *
 * Return: File ptr
 */
FILE *Proc_fd::redirect(string filename, string mode)
{
    auto result_fd = fopen(filename.c_str(), mode.c_str());
    if (mode.at(0) == 'r') {
        dup2(fileno(result_fd), STDIN_FILENO);
    } else if (mode.at(0) == 'w')
        this->fd[1] = result_fd;
    return result_fd;
}

/**
 * Read from in_fd
 * Must send before you read
 * Or you will get an infinite loop.
 * Be careful for creadting the pipe first.
 */
void Proc_fd::read(string &_restrict)
{
    _restrict = file2String(this->fd[0]);
}

/**
 * This index is 1 or 2 only
 * 1 for stdout, 2 for stderr
 * Be careful for creadting the pipe first
 */
void Proc_fd::write(string str, int index)
{
    string2out(str, this->fd[index]);
}

/////////////////////Proc zone/////////////////////

Proc::Proc(string cmd)
{
    this->command = cmd;
    this->pass = false;
    this->prev = nullptr;
    this->next = nullptr;
}

Proc::~Proc()
{
    // Will auto invoke Proc_fd's distructor
}

/**
 * Will use the input file descriptor straightly
 * And create new output file descriptot to next process
 *
 * TODO: stderr not work now!!
 *
 * @return: the error code
 */
int Proc::doExecute(vector<FILE *> &bgPool)
{
    int errorCode = 0;

    if (!this->pass) {
        pid_t get_pid = 0;
        auto result_fd = mypopen(this->command.c_str(), "r", &get_pid);
        if (result_fd == NULL) {
            errorCode = ERR_CMD_NOT_FOUND;  // command not cfound
        } else {
            if (this->command.back() == '&') {
                cout << '[' << get_pid << ']' << endl;
                bgPool.push_back(result_fd);
            } else {
                auto result_s = file2String(result_fd);
                this->fd.write(result_s, 1);
                mypclose(result_fd);
            }
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
 * 0000 0000 0000 0000 0000 0000 0000 0001: file not exists or permission
 * denied
 * 0000 0000 0000 0001 0000 0000 0000 0001: command not found
 * 0000 0000 0000 0010 0000 0000 0000 0001: ambiguous command
 * 0000 0000 0000 0011 0000 0000 0000 0001: wrong file descriptor
 * 0000 0000 0000 0100 0000 0000 0000 0001: wrong file mode
 *
 * rb:
 * "0000 0000 0000 0010" pipe error
 *  0000 0000 0000 0000 0000 0000 0000 0010: pipe error
 */
void Proc::raiseError(int errorCode)
{
    switch (errorCode) {
    case ERR_FILE_NOT_EXIST:
        perror("File is not exist or permission denied\n");
        break;
    case ERR_CMD_NOT_FOUND:
        perror("Command not found");
        break;
    case ERR_AMBIGUOUS_CMD:
        perror("Ambiguous command");
        break;
    case ERR_WRONG_FD:
        perror("Wrong file descriptor");
        break;
    case ERR_WRONG_FILE_MODE:
        perror("Wrong file mode");
        break;
    case ERR_PIPE:
        perror("Pipe error");
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
     * TODO: support stderr redirection, append string identifier;
     */
    char c = this->command.back();
    if (c == ';')
        ;
    else if (c == '>' || c == '<') {
        // Next command is a file
        if (!this->next) {  // File not exist
            errorCode = ERR_FILE_NOT_EXIST;
        } else {
            auto fileName = removeSpace_semicolon(this->next->command);
            auto fd = this->fd.redirect(fileName, (c == '>') ? "w" : "r");
            this->next->pass = true;
            errorCode = (fd) ? errorCode : ERR_WRONG_FD;
        }
    } else if (c == '|') {
        if (!this->next)
            errorCode = ERR_CMD_NOT_FOUND;
        else {
            this->fd.set_pipe(this->fd.get_fd(1));
        }
    } else {
        errorCode = ERR_AMBIGUOUS_CMD;
    }

    raiseError(errorCode);
    this->command.back() = ' ';
}

/////////////////////Cmd_q zone/////////////////////

Cmd_q::Cmd_q()
{
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
}

void Cmd_q::push_back(Proc *ele)
{
    ele->prev = this->tail;
    if (this->head == nullptr)
        this->head = ele;
    else
        this->tail->next = ele;
    this->tail = ele;
}

/**
 * Execute all the commands and return the status code while exist
 */
int Cmd_q::execute(vector<FILE *> &bgPool)
{
    int std_save[3] = {dup(STDIN_FILENO), dup(STDOUT_FILENO),
                       dup(STDERR_FILENO)};
    for (auto curr = this->head; curr; curr = curr->next)
        curr->commandParser();
    auto errorCode = 0;
    for (auto curr = this->head; curr; curr = curr->next)
        errorCode = curr->doExecute(bgPool);

    dup2(std_save[0], STDIN_FILENO);
    dup2(std_save[1], STDOUT_FILENO);
    dup2(std_save[2], STDERR_FILENO);
    return errorCode;
}