// 12 вариант
// трапеция, список, очередь
// М8О-206Б-19 Киреев А.К.

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

#include "list.hpp"
#include "trapezoid.hpp"

void print(Trapezoid<double> t) {
    if (t.valid) {
        std::cout << t;
    }
}

// мануал
void help() {
    std::cout << "Command -- Description\n" <<
                "1  [idx] -- Add figure \n" <<
                "2 [idx] -- Delete figure by index from vector\n" <<
                "3 -- Display vector\n" << 
                "4 -- Display count of figures with less area\n" <<
                "5 -- Display help\n" <<
                "6 -- End program" << std::endl;
}

void mainLoop() {
    int command = 0, idx = 0;
    List<Trapezoid<double>, custom_allocator<Trapezoid<double>, 100>> l;
    std::string s;
    while (std::cout << "Cmd: " && std::cin >> s) {
        if (s.length() > 1) {
            std::cout << "Invalid command." << std::endl;
            continue;
        }
        std::stringstream ss(s);
        ss >> command;
        if (command == 6)
            break;
        if (command == 1) {
            std::cin >> idx;
            List<Trapezoid<double>, custom_allocator<Trapezoid<double>, 100>>::ListIterator it(l, idx);
            Trapezoid<double> tmp(std::cin);
            try {
                l.insert(it, tmp);
            } catch (std::logic_error& e) {
                std::cerr << "ERROR: " << e.what() << "\n";
                continue;
            }
        } else if (command == 2) {
            std::cin >> idx;
            List<Trapezoid<double>, custom_allocator<Trapezoid<double>, 100>>::ListIterator it(l, idx);
            try {
                l.erase(it);
            } catch (std::logic_error& e) {
                std::cerr << "ERROR: " << e.what() << "\n";
                continue;
            }
        } else if (command == 3) {
            std::cout << "=========START OF VECTOR=========" << std::endl;
            std::for_each(l.begin(), l.end(), print);
            std::cout << "==========END OF VECTOR==========" << std::endl;
        } else if (command == 4) {
            std::cout << "Enter max val:\n";
            int max_val;
            std::cin >> max_val;
            int ans = std::count_if(l.begin(), l.end(), [max_val](Trapezoid<double> t){return t.area() < max_val;});
            std::cout << "Count = " << ans << "\n";
        } else if (command == 5) {
            help();
        } else {
            std::cout << "Invalid command!" << std::endl; 
        }
    }
}

int main() {
    help();
    mainLoop();
    return 0;
}