#include "proc.hpp"

#include <algorithm>
#include <cstring>
#include <functional>
#include <random>
#include <vector>

using namespace std;
#define BUFFER_SIZE 1000 /* Memory page takes 4K for cache*/

extern "C" {
#include "popen.h"
}

static unsigned int rand_func()
{
    random_device rd;
    auto gen = mt19937_64(rd());
    uniform_int_distribution<int> dis(0, UINT32_MAX);
    auto f = bind(dis, gen);
    return f();
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

static inline void string2out(string s, FILE *out)
{
    if (out && out != stdin) {
        fputs(s.c_str(), out);
    }
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

Proc::Proc(string cmd)
{
    this->in_fd = stdin;
    this->out_fd = stdout;
    this->err_fd = stderr;
    this->pass = false;
    this->doPipe = false;

    this->command = cmd;
    this->prev = nullptr;
    this->next = nullptr;
}

Proc::~Proc()
{
    if (this->in_fd != stdin)
        pclose(in_fd);
    if (this->out_fd != stdout)
        pclose(this->out_fd);
    if (this->err_fd != stderr)
        fclose(this->err_fd);
}

void Proc::setSIO(string _in, string _out, string _err)
{
    this->in_s = _in;
    this->out_s = _out;
    this->err_s = _err;
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

    if (this->prev && this->prev->doPipe) {
        auto filename = string("/tmp/myShell_") + to_string(rand_func());
        auto tmp = fopen(filename.c_str(), "w");
        string2out(this->in_s, tmp);
        fflush(tmp);
        fclose(tmp);
        this->command += filename;
        this->in_s.clear();
    }
    if (!this->pass) {
        this->command += this->in_s;

        pid_t get_pid = 0;
        auto result_fd = mypopen(this->command.c_str(), "r", &get_pid);
        if (result_fd == NULL) {
            errorCode = 65537;  // command not cfound
        } else {
			this->command.pop_back();
            if (this->command.back() == '&') {
                cout << '[' << get_pid << ']' << endl;
                bgPool.push_back(result_fd);
            } else {
                this->out_s = file2String(result_fd);
                mypclose(result_fd);
            }
            if (this->out_fd && !this->next)
                fputs(this->out_s.c_str(), this->out_fd);
        }
    } else {
        /*
         * Other conditions are "redirection and in the end of command"
         * Need to judge if the out_fd is exist, if not exist that is create
         * file error.
         */
        if (this->out_fd != stdout && this->out_fd && !this->next)
            fputs(this->in_s.c_str(), this->out_fd);
    }
    // pass this output to next input
    if (this->next)
        this->next->in_s = this->out_s;
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
 *
 * rb:
 * "0000 0000 0000 0010" unknown error
 *  0000 0000 0000 0000 0000 0000 0000 0010: unknown error
 */
void Proc::raiseError(int errorCode)
{
    switch (errorCode) {
    case 1:
        perror("File is not exist or permission denied\n");
        break;
    case 65537:
        perror("Command not found");
        break;
    case 131073:
        perror("Ambiguous command");
        break;
    case 2:
        perror("unknown error");
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
    auto _in_s = (this->prev) ? this->prev->out_s : string();
    auto _out_s = (this->next) ? this->next->in_s : string();
    auto _err_s = this->err_s;
    setSIO(_in_s, _out_s, err_s);
    if (c == ';')
        ;
    else if (c == '>') {
        // Next command is a file
        if (!this->next) {  // File not exist
            errorCode = 1;
        } else {
            auto fileName = removeSpace_semicolon(this->next->command);
            this->next->out_fd = fopen(fileName.c_str(), "w");
            this->next->pass = true;
        }
    } else if (c == '<') {
        if (!this->next)
            errorCode = 1;
        else {
            auto fileName = removeSpace_semicolon(this->next->command);
            auto _in_df = fopen(fileName.c_str(), "r");
            if (_in_df == NULL)
                errorCode = 1;
            else {
                this->in_s = file2String(_in_df);
                fclose(_in_df);
            }
            this->next->pass = true;
        }
    } else if (c == '|') {
        this->doPipe = true;
        if (!this->next)
            errorCode = 65537;
    } else {
        errorCode = 131073;
    }

    raiseError(errorCode);
    this->command.back() = ' ';
}

/////////////////////Cmd_q zone///////////////////////

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
    if (this->head == nullptr) {
        this->head = ele;
    } else {
        ele->prev = this->tail;
        this->tail->next = ele;
    }
    this->tail = ele;
}


/**
 * Execute all the commands and return the status code while exist
 */
int Cmd_q::execute(vector<FILE *> &bgPool)
{
    for (auto curr = this->head; curr; curr = curr->next)
        curr->commandParser();
    auto errorCode = 0;
    for (auto curr = this->head; curr; curr = curr->next) {
        errorCode = curr->doExecute(bgPool);
    }
    return errorCode;
}
