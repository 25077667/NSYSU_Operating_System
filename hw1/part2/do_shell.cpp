#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "proc.hpp"

#define EXIT_SHELL(state) exit(state)
#define EXIT_PASS 4
#define BUFFER_SIZE 1000
using namespace std;

extern "C" {
#include "linenoise.h"
#include "popen.h"
}

static string get_user_line();
static int cmd_hook(string cmd);
static vector<string> separate_command(string commands);
static string print_prompt();
static int help();
static bool bothAreSpaces(char lhs, char rhs);
static void clean_bgPool(vector<FILE *> &);

int main()
{
    cout << "YOU ARE IN MY SHELL!" << endl;
    cout << "Type \'help\' to get helps." << endl;
    int errorCode = EXIT_SUCCESS;
    vector<FILE *> bgPool;
    while (true) {
        string singleLine = get_user_line();

        /* Check user diden't quit or something*/
        errorCode = cmd_hook(singleLine);
        if (errorCode == EXIT_SUCCESS)
            break;
        else if (errorCode == EXIT_PASS)
            continue;
        else if (errorCode == EXIT_FAILURE) {
            auto commands = separate_command(singleLine);
            Cmd_q q;
            for (auto i : commands) {
                auto ele = new Proc(i);
                q.push_back(ele);
            }
            errorCode = q.execute(bgPool);
        }
    }
    clean_bgPool(bgPool);
    EXIT_SHELL(errorCode);
}

static int help()
{
    cout << "This is HELP" << endl
         << "Not support stderr yet" << endl
         << "Will compelete single process first!" << endl
         << "\'quit\' is the special command to exit(0)" << endl;
    return EXIT_PASS;
}

static string print_prompt()
{
    char buffer[BUFFER_SIZE] = {0};

    /* Get id */
    auto tmp_fd = popen("id -u", "r");
    int id = 1;
    if (fscanf(tmp_fd, "%d", &id))
        pclose(tmp_fd);
    string promptSign = ((id) ? "$ " : "# ");

    /* Get pwd */
    tmp_fd = popen("pwd -P", "r");
    if (fscanf(tmp_fd, "%s", buffer))
        pclose(tmp_fd);
    string pwd(buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    /* Get user name*/
    tmp_fd = popen("whoami", "r");
    if (fscanf(tmp_fd, "%s", buffer))
        pclose(tmp_fd);
    string userName(buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    /* Get host name */
    tmp_fd = popen("cat /proc/sys/kernel/hostname", "r");
    if (fscanf(tmp_fd, "%s", buffer))
        pclose(tmp_fd);
    string hostName(buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    return userName + "@" + hostName + ":" + pwd + promptSign;
}

static string get_user_line()
{
    string singleLine;
    // while (true) {
    auto line = linenoise(print_prompt().c_str());
    if (line == NULL)
        ;
    else if (line[0] != '\0' && line[0] != '/') {
        singleLine = string(line);
        linenoiseHistoryAdd(line);           /* Add to the history. */
        linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        // break;
    } else if (!strncmp(line, "/historylen", 11)) {
        /* The "/historylen" command will change the history len. */
        int len = atoi(line + 11);
        linenoiseHistorySetMaxLen(len);
    } else if (!strncmp(line, "/mask", 5)) {
        linenoiseMaskModeEnable();
    } else if (!strncmp(line, "/unmask", 7)) {
        linenoiseMaskModeDisable();
    } else if (line[0] == '/') {
        printf("Unreconized command: %s\n", line);
    }
    free(line);
    return singleLine;
}

/* This function might out of date, I'm going to be deleted */
static int cmd_hook(string cmd)
{
    /* Return 0 if execution succeed */
    int exe_result = EXIT_FAILURE;
    if (cmd == "help")
        exe_result = help();
    else if (cmd == "quit")
        exe_result = EXIT_SUCCESS;
    else if (cmd.substr(0, cmd.find(" ")) == "cd") {
        cmd.erase(0, cmd.find(" ") + 1);
        /* chdir: 0 is success, -1 is failed */
        if (chdir(cmd.c_str())) {
            cerr << "myshell: cd: " << cmd << ": No such file or directory"
                 << endl;
        }
    } else if (cmd == "exit") {
        cout << "Trying to exit?" << endl
             << "If YES, please type \'quit\'" << endl;
    }
    return exe_result;
}

static vector<string> separate_command(string commands)
{
    /* Remove duplicate <space> */
    commands += ";";
    auto new_end = unique(commands.begin(), commands.end(), bothAreSpaces);
    commands.erase(new_end, commands.end());

    /* semicolon is end of command */
    vector<string> command_set;

    /*
     *  Priority table:
     *  pipe > redirection > semicolon
     *
     *  TODO: Maybe support "append" "stderr" "stdout" clearly redirection.
     */
    while (commands.length()) {
        auto found = commands.find("|");
        if (found != string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
        }

        found = commands.find(">");
        if (found != string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
        }

        found = commands.find("<");
        if (found != string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
        }

        found = commands.find(";");
        if (found != string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
        }
    }

    /* Redirect to */

    return command_set;
}

static inline bool bothAreSpaces(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}

static void clean_bgPool(vector<FILE *> &v)
{
    for (auto i : v)
        mypclose(i);
}