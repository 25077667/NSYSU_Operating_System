#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

bool do_cmd(string cmd){
    return system(cmd.c_str());
}

bool putPWD(){
    return do_cmd("pwd -P");
}

void print_prompt(){
    char promptSign = '$';
    putPWD();
    //string id_str = do_cmd(string("id"));
    //cout << id_str.substr(id_str.find("uid=")) << endl;
    //if(id_str.substr(4, )
    cout <<  promptSign << ' ';
}

int main()
{
    cout << "YOU ARE IN MY SHELL!" << endl;
    while(true){
        string cmd;
        print_prompt();
        cin >> cmd;

    }
    return 0;
}
