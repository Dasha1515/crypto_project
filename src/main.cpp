#include <iostream>
#include <string>
#include <limits>
#include "../include/gaml.h"
#include "../include/hill.h"
#include "../include/shamir.h"

using namespace std;

int getMenuChoice(const string& prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << prompt;
        if (!(cin >> choice)) {
            cout << "Ошибка: введите число от " << minVal << " до " << maxVal << "!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (choice >= minVal && choice <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
        cout << "Неверный ввод. Пожалуйста, выберите вариант от " << minVal << " до " << maxVal << ".\n";
    }
}

void showMainMenu() {
    while (true) {
        cout << "\n=== Криптографическая система ===" << endl;
        cout << "1. шифр Эль-Гамаля" << endl;
        cout << "2. Шифр Хилла" << endl;
        cout << "3. Протокол Шамира" << endl;
        cout << "4. Выход" << endl;
        
        int choice = getMenuChoice("Выберите алгоритм: ", 1, 4);
        
        switch (choice) {
            case 1:
                cout << "\n=== Шифр Эль-Гамаля ===" << endl;
                ElGamal::run();
                break;
            case 2:
                cout << "\n=== Шифр Хилла ===" << endl;
                HillCipher::run();
                break;
            case 3:
                cout << "\n=== Протокол Шамира ===" << endl;
                ShamirProtocol::run();
                break;
            case 4:
                cout << "Выход из программы." << endl;
                return;
        }
    }
}

int main() {
    try {
        showMainMenu();
    } catch (const exception& e) {
        cerr << "Произошла ошибка: " << e.what() << endl;
        return 1;
    }
    return 0;
}
