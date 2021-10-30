#include <Windows.h>
#include <iostream>
#include "../dll/dll.h"
//#pragma comment(lib,"../dll/dll.h")

int main(){
   
    const char* strToFind = "To Find str";
    char str_test[] = "To Find str";
    char str_rep[12] = "To Replace";
    std::cout << str_test << " " << (SIZE_T)&str_test << std::endl;
    ProccessMain(strToFind, str_rep);

    std::cout << str_test << std::endl;
}