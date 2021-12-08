#include <iostream>
#include <string>
#include "LR_algorithm.h"

//Прописать исключения, подключить тесты
int main() {
    Grammar gram;
    int n;
    std::cin >> n;
    std::string str;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < n; ++i) {
        std::cin >> gram;
    }
    Algo a;
    a.Fit(gram);
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cin >> str;
        a.Predict(str);
    }
    return 0;
}