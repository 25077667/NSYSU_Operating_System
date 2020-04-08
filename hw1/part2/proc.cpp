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

static inline void string2out(string s, FILE *out)
{
    if (out && out != stdin) {
        fputs(s.c_str(), out);
    }
}

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
    this->out_fd = stdout;
    this->err_fd = stderr;
    this->pass = false;

    this->command = cmd;
    this->prev = nullptr;
    this->next = nullptr;
}

Proc::~Proc()
{
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
 * @return: the output file descriptor(read only)
 */
int Proc::doExecute()
{
    int errorCode = 0;
    /*
     * Redirect to file output that is not a command
     * Hence this code needn't to execute.
     */

    if (!this->pass) {
        // this->command += ("\'" + this->in_s + "\'");
        this->command += this->in_s;
        // Well, ls might input a '\n' at the begin of the line
        replace(this->command.begin(), this->command.end(), '\n', ' ');

        cout << ">>>>>>>>>>>>>>>>>>>" << endl
             << this->command << endl
             << "<<<<<<<<<<<<<<<<<<<" << endl;

        auto result_fd = popen(this->command.c_str(), "r");
        if (result_fd == NULL) {
            errorCode = 65537;  // command not cfound
        } else {
            this->out_s = file2String(result_fd);
            pclose(result_fd);

            // pass this output to next input
            if (this->next)
                this->next->in_s = this->out_s;
            else if (this->out_fd)
                fputs(this->out_s.c_str(), this->out_fd);
            else
                errorCode = 2;
        }
    } else {
        if (this->next)
            this->next->in_s = this->out_s;
        // other conditions are "redirection and in the end of command"
        if (this->out_fd != stdout && this->out_fd) {
            fputs(this->in_s.c_str(), this->out_fd);
            fclose(this->out_fd);
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
     * TODO: support stderr redirection, multi-redirection, append
     * string identifier;
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
        if (!this->next)
            errorCode = 65537;
    } else {
        errorCode = 131073;
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