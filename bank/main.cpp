#include <iostream>
#include <string>
#include <list>
#include <cstring>
#include "user.h"
#include "admin.h"
using namespace std;


int main() {
    admin mybank;
    mybank.readinfile();
    mybank.showmain();
    mybank.writeinfile();
    //std::cout << "Hello, World!" << std::endl;
    return 0;
}
