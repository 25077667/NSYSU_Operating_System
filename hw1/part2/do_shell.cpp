#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "linenoise.h"
#include "proc.hpp"
#define EXIT_SHELL(state) exit(state)
using namespace std;

string get_user_line();
int cmd_hook(string cmd);
vector<string> separate_command(string commands);
string print_prompt();
int help();
bool bothAreSpaces(char lhs, char rhs);

int main()
{
    cout << "YOU ARE IN MY SHELL!" << endl;
    cout << "Type \'help\' to get helps." << endl;
    while (true) {
        string singleLine = get_user_line();

        /* Check user diden't quit or something*/
        if (cmd_hook(singleLine) != EXIT_SUCCESS)
            break;

        auto commands = separate_command(singleLine);
        Cmd_q q;
        for (auto i : commands) {
            auto ele = new Proc(i);
            q.push_back(ele);
        }
        q.execute();
        delete &q;  // End of commands
    }
    EXIT_SHELL(EXIT_SUCCESS);
}

int help()
{
    cout << "This is HELP" << endl;
    cout << "Not support stderr yet" << endl;
    cout << "Will compelete single process first!" << endl;
    cout << "\'quit\' is the special command to exit(0)" << endl;
    return 0;
}

string print_prompt()
{
    auto id_fd = popen("id -u", "r");
    int id;
    auto trash = fscanf(id_fd, "%d", &id);
    pclose(id_fd);
    string promptSign = ((id) ? "$" : "#");

    return promptSign + " ";
}

string get_user_line()
{
    string singleLine;
    //while (true) {
        auto line = linenoise(print_prompt().c_str());
        if (line[0] != '\0' && line[0] != '/') {
            singleLine = string(line);
            linenoiseHistoryAdd(line);           /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
            //break;
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
    //}
    return singleLine;
}

/* This function might out of date, I'm going to be deleted */
int cmd_hook(string cmd)
{
    /* Return 0 if execution succeed */
    int exe_result = 0;
    if (cmd == "help")
        exe_result = help();
    else if (cmd == "quit")
        EXIT_SHELL(EXIT_SUCCESS);

    return exe_result;
}

vector<string> separate_command(string commands)
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

inline bool bothAreSpaces(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}