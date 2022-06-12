#include <iostream>
#include "bpt.h"
#include "string.hpp"

sjtu::bpt<sjtu::pair<sjtu::string, int>, int> tree("file.dat", "file_leaves.dat", "file_delete.dat",
                                                   "file_leaves_delete.dat");
const int min_ = -2147483648;
const int max_ = 2147483647;

int main() {
    int n;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        sjtu::string s;
        std::cin >> s;
        if (s == sjtu::string("insert")) {
            int x;
            std::cin >> s;
            std::cin >> x;
            try { tree.insert(sjtu::pair<sjtu::pair<sjtu::string, int>, int>(sjtu::pair<sjtu::string, int>(s, x), x)); }
            catch (...) {}
        } else if (s == sjtu::string("find")) {
            std::cin >> s;
            try {
                sjtu::pair<sjtu::string, int> l(s, min_);
                sjtu::pair<sjtu::string, int> r(s, max_);
                sjtu::vector<int> tmp = tree.traverse(l, r);
                if (tmp.length) {
                    for (int i = 0; i < tmp.length; i++) {
                        printf("%d ", tmp[i]);
                    }
                    printf("\n");
                } else {
                    printf("null\n");
                }
            }
            catch (...) { std::cout << "null" << std::endl; }
        } else if (s == sjtu::string("delete")) {
            int x;
            std::cin >> s;
            std::cin >> x;
            try { tree.erase(sjtu::pair<sjtu::string, int>(s, x)); }
                //try { tree.erase(s); }
            catch (...) {}
        }
    }
}

