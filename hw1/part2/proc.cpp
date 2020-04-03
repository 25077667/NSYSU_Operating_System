#include "proc.hpp"

void Proc::setSTDIN(bool set){
    this->doIn = set;
}

void Proc::setSTDOUT(bool set){
    this->doOut = set;
}

void Proc::setSTDERR(bool set){
    this->doErr = set;
}
