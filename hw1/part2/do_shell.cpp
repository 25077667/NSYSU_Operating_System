#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "colorprint.hpp"
#include "proc.hpp"
#define EXIT_SHELL(state) exit(state)
#define EXIT_PASS 4
#define BUFFER_SIZE 1000

extern "C" {
#include "linenoise.h"
#include "popen.h"
}

static std::string get_user_line();
static int cmd_hook(std::string cmd);
static std::vector<std::string> separate_command(std::string commands);
static std::string print_prompt();
static int help();
static bool bothAreSpaces(char lhs, char rhs);
static void clean_bgPool(std::vector<FILE *> &);
struct PS {
    bool name = 1;
    bool hostname = 1;
    bool pwd = 0;
    bool prompt = 1;
    bool rainbow = 1;
} ps;


int main()
{
    std::cout << "YOU ARE IN MY SHELL!" << std::endl;
    std::cout << "Type \'help\' to get helps." << std::endl;
    int errorCode = EXIT_SUCCESS;
    std::vector<FILE *> bgPool;
    while (true) {
        std::string singleLine = get_user_line();

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
    std::cout
        << "This is HELP" << std::endl
        << "Not support stderr yet" << std::endl
        << "Will compelete single process first!" << std::endl
        << "\'quit\' is the special command to exit(0)" << std::endl
        << std::endl
        << "SET promt:" << std::endl
        << "\tYou can say some thing turn on and off!" << std::endl
        << "\tYes, only on and off" << std::endl
        << std::endl
        << "\tFor example:" << std::endl
        << "\t\tPS=pwd:off" << std::endl
        << "\t\tPS=name:on" << std::endl
        << "\tThe supporting attributes are: pwd, name, hostname, prompt, "
           "rainbow"
        << std::endl;
    return EXIT_PASS;
}

static std::string print_prompt()
{
    char buffer[BUFFER_SIZE] = {0};
    std::string promptSign, pwd, userName, hostName;
    FILE *tmp_fd;

    /* Get id */
    if (ps.prompt) {
        tmp_fd = popen("id -u", "r");
        int id = 1;
        if (fscanf(tmp_fd, "%d", &id))
            pclose(tmp_fd);
        promptSign = ((id) ? "$ " : "# ");
    }

    /* Get pwd */
    if (ps.pwd) {
        tmp_fd = popen("pwd -P", "r");
        if (fscanf(tmp_fd, "%s", buffer))
            pclose(tmp_fd);
        pwd = ":" + std::string(buffer, strlen(buffer));
        memset(buffer, 0, BUFFER_SIZE);
    }

    /* Get user name*/
    if (ps.name) {
        tmp_fd = popen("whoami", "r");
        if (fscanf(tmp_fd, "%s", buffer))
            pclose(tmp_fd);
        userName = std::string(buffer, strlen(buffer));
        memset(buffer, 0, BUFFER_SIZE);
    }

    /* Get host name */
    if (ps.hostname) {
        tmp_fd = popen("cat /proc/sys/kernel/hostname", "r");
        if (fscanf(tmp_fd, "%s", buffer))
            pclose(tmp_fd);
        hostName = "@" + std::string(buffer, strlen(buffer));
        memset(buffer, 0, BUFFER_SIZE);
    }

    return userName + hostName + pwd + promptSign;
}

static std::string get_user_line()
{
    std::string singleLine;

    print_color(print_prompt(), ps.rainbow);
    auto line = linenoise("");

    if (line == NULL)
        ;
    else if (line[0] != '\0' && line[0] != '/') {
        singleLine = std::string(line);
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
static int cmd_hook(std::string cmd)
{
    /* Return 0 if execution succeed */
    int exe_result = EXIT_FAILURE;
    if (cmd == "help")
        exe_result = help();

    else if (cmd == "quit" || cmd == "q")
        exe_result = EXIT_SUCCESS;

    else if (cmd.substr(0, cmd.find(" ")) == "cd") {
        cmd.erase(0, cmd.find(" ") + 1);
        /* chdir: 0 is success, -1 is failed */
        if (chdir(cmd.c_str())) {
            cerr << "myshell: cd: " << cmd << ": No such file or directory"
                 << std::endl;
        }
    }

    else if (cmd.substr(0, cmd.find("=")) == "PS") {
        cmd.erase(0, cmd.find("=") + 1);
        auto attribute = cmd.substr(0, cmd.find(":"));
        cmd.erase(0, cmd.find(":") + 1);
        auto action = cmd;

        if (attribute == "name")
            ps.name = (action == "on");
        else if (attribute == "hostname")
            ps.hostname = (action == "on");
        else if (attribute == "pwd")
            ps.pwd = (action == "on");
        else if (attribute == "prompt")
            ps.prompt = (action == "on");
        else if (attribute == "rainbow")
            ps.rainbow = (action == "on");
        else
            cerr << "Wrong format for setting PS!!" << std::endl;

    } else if (cmd == "exit") {
        std::cout << "Trying to exit?" << std::endl
                  << "If YES, please type \'quit\'" << std::endl;
    }
    return exe_result;
}

static std::vector<std::string> separate_command(std::string commands)
{
    /* Remove duplicate <space> */
    commands += ";";
    auto new_end = unique(commands.begin(), commands.end(), bothAreSpaces);
    commands.erase(new_end, commands.end());

    /* semicolon is end of command */
    std::vector<std::string> command_set;

    /*
     *  Priority table:
     *  pipe > redirection > semicolon
     *
     *  TODO: Maybe support "append" "stderr" "stdout" clearly redirection.
     */
    while (commands.length()) {
        auto found = commands.find("|");
        if (found != std::string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
            continue;
        }

        found = commands.find(">");
        if (found != std::string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
            continue;
        }

        found = commands.find("<");
        if (found != std::string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
            continue;
        }

        found = commands.find(";");
        if (found != std::string::npos) {
            command_set.push_back(commands.substr(0, ++found));
            commands.erase(0, found);
            continue;
        }
    }

    /* Redirect to */
    return command_set;
}

static inline bool bothAreSpaces(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}

static void clean_bgPool(std::vector<FILE *> &v)
{
    for (auto i : v)
        mypclose(i);
}