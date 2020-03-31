#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

string do_cmd(string cmd){
    system(cmd.c_str());
}

string putPWD(){
    do_cmd("pwd -P");
}

void print_prompt(){
    char promptSign = '$';
    //string id_str = do_cmd(string("id"));
    //cout << id_str.substr(id_str.find("uid=")) << endl;
    //if(id_str.substr(4, )
    //cout << getPWD() <<  promptSign << ' ';
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
