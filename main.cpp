#include <iostream>
#include <string>
#include "LR_algorithm.h"

//Прописать исключения, подключить тесты, сделать функцию чтобы печатала прям ответ, наладить ввод строки
int main() {
    Grammar gram;
    int n;
    std::cin >> n;
    std::string str;
    std::cin >> str;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < n; ++i) {
        std::cin >> gram;
    }
    Algo a;
    a.Fit(gram);
    if (a.Predict(str)) {
        std::cout << 1;
    } else {
        std::cout << 0;
    }
    return 0;
}