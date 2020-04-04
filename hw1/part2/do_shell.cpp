#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#define EXIT_SHELL(state) exit(state)

using namespace std;
int do_cmd(string cmd);
string get_user_inputs();
vector<string> format_command(string commands);
inline bool bothAreSpaces(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}
void print_prompt();
int help();

int main()
{
    cout << "YOU ARE IN MY SHELL!" << endl;
    cout << "Type \'help\' to get helps." << endl;
    while (true) {
        print_prompt();

        string singleLine = get_user_inputs();
        auto commands = format_command(singleLine);
    }
    EXIT_SHELL(EXIT_SUCCESS);
}

int help()
{
    cout << "This is HELP" << endl;
    cout << "Not support stderr yet" << endl;
    cout << "Will compelete single process first!" << endl;
    return 0;
}

void print_prompt()
{
    char promptSign = '$';
    /*
     * TODO: Add user identify to change the sign
     *       add user can self configurilze the prompt symbol
     */

    cout << promptSign << ' ';
}

string get_user_inputs()
{
    string single_cmd;
    string prev_line_cmd;
    bool keepTyping = true;

    while (keepTyping && getline(cin, single_cmd)) {
        keepTyping = false;
        single_cmd = prev_line_cmd + single_cmd;

        /* Only identify the last char is a '\' */
        if (single_cmd.back() == '\\') {
            single_cmd.pop_back();
            keepTyping = true;
        }

        if (keepTyping)
            prev_line_cmd = single_cmd;
    }
    return single_cmd;
}

int do_cmd(string cmd)
{
    /* Return 0 if execution succeed */
    int exe_result = 0;
    if (cmd == "help")
        exe_result = help();
    else if (cmd == "exit")
        EXIT_SHELL(EXIT_SUCCESS);
    else {
        // https://burweisnote.blogspot.com/2017/08/execlpexecvp.html
    }
    return exe_result;
}

vector<string> format_command(string commands)
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