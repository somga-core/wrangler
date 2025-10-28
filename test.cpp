#include <iostream>

int main(int argc, char const *argv[]) {
    char ch = '1';
    char chs[] = " ";
    
    std::cout << strtol(chs, NULL, 16) << std::endl;
    return 0;
}
