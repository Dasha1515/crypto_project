#include "../../include/gaml.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <string>
#include <limits>
#include <sstream>

using namespace std;

namespace ElGamal {

int g, y, x;
const int p = 30803;

//обработка ошибок ввода
int safeInputInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (!(cin >> value)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Ошибка: введите число!\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

string safeInputString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (cin.fail()) {
            cin.clear();
            cerr << "Ошибка ввода!\n";
        } else if (value.empty()) {
            cerr << "Ошибка: ввод не может быть пустым!\n";
        } else {
            return value;
        }
    }
}

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

long long modularExponent(long long a, long long b, long long m) {
    long long result = 1;
    a = a % m;
    while (b > 0) {
        if (b % 2 == 1) result = (result * a) % m;
        a = (a * a) % m;
        b = b / 2;
    }
    return result;
}
//поиск первообразного корян (g)
int findPrimitiveRoot(int p) {
    if (!isPrime(p)) return -1;
    int phi = p - 1;
    vector<int> factors;

    int temp = phi;
    for (int i = 2; i <= temp; ++i) {
        if (temp % i == 0) {
            factors.push_back(i);
            while (temp % i == 0) temp /= i;
        }
    }

    for (int r = 2; r <= p; ++r) {
        bool isRoot = true;
        for (int q : factors) {
            if (modularExponent(r, phi / q, p) == 1) {
                isRoot = false;
                break;
            }
        }
        if (isRoot) return r;
    }
    return -1;
}

vector<unsigned char> readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл '" << filename << "'\n";
        return {};
    }
    
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    
    vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        cerr << "Ошибка чтения файла\n";
        return {};
    }
    return buffer;
}

bool writeFile(const string& filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось создать файл '" << filename << "'\n";
        return false;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

bool writeCiphertext(const string& filename, const vector<pair<int, int>>& ciphertext) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Ошибка создания файла для зашифрованных данных\n";
        return false;
    }
    
    for (const auto& pair : ciphertext) {
        file.write(reinterpret_cast<const char*>(&pair.first), sizeof(int));
        file.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
    return true;
}

vector<pair<int, int>> readCiphertext(const string& filename) {
    vector<pair<int, int>> ciphertext;
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Ошибка: не удалось открыть файл с зашифрованными данными\n";
        return {};
    }
    
    pair<int, int> pair;
    while (file.read(reinterpret_cast<char*>(&pair.first), sizeof(int)) &&
           file.read(reinterpret_cast<char*>(&pair.second), sizeof(int))) {
        ciphertext.push_back(pair);
    }
    return ciphertext;
}

void generateKeys() {
    g = findPrimitiveRoot(p);
    srand(time(0));
    x = 2 + rand() % (p - 3);
    y = modularExponent(g, x, p);
}

//шифрование
vector<pair<int, int>> encryptBinary(const vector<unsigned char>& data) {
    vector<pair<int, int>> ciphertext;
    for (unsigned char byte : data) {
        int m = static_cast<int>(byte);
        int k = 2 + rand() % (p - 3);
        int a = modularExponent(g, k, p);
        int b = (modularExponent(y, k, p) * m) % p;
        ciphertext.emplace_back(a, b);
    }
    return ciphertext;
}
//дешифрование
vector<unsigned char> decryptBinary(const vector<pair<int, int>>& ciphertext, int secretKey) {
    vector<unsigned char> data;
    for (const auto& pair : ciphertext) {
        int a = pair.first;
        int b = pair.second;
        int s = modularExponent(a, secretKey, p);
        int s_inv = modularExponent(s, p - 2, p);
        int m = (b * s_inv) % p;
        data.push_back(static_cast<unsigned char>(m));
    }
    return data;
}

//ввод данных
vector<unsigned char> getDataFromConsole() {
    string input;
    getline(cin, input);
    return vector<unsigned char>(input.begin(), input.end());
}

void run() {
    while (true) {
        cout << "\n=== Шифр Эль-Гамаля ===" << endl;
        cout << "1. Зашифровать сообщение" << endl;
        cout << "2. Расшифровать сообщение" << endl;
        cout << "3. Выход" << endl;
        
        int action = safeInputInt("Выберите действие: ");
        
        if (action == 3) break;
        
        cout << "\nВыберите источник данных:" << endl;
        cout << "1. Работа с файлом" << endl;
        cout << "2. Работа с консолью" << endl;
        int source = safeInputInt("Ваш выбор: ");
        
        if (action == 1) {
            // Шифрование
            vector<unsigned char> data;
            string filename;
            
            if (source == 1) {
                filename = safeInputString("Введите имя файла: ");
                data = readFile(filename);
                if (data.empty()) continue;
            } else {
                cout << "Введите текст для шифрования: ";
                data = getDataFromConsole();
            }
            
            generateKeys();
            auto ciphertext = encryptBinary(data);
            
            if (source == 1) {
                string outputFilename = safeInputString("Введите имя файла для сохранения: ");
                if (writeCiphertext(outputFilename, ciphertext)) {
                    cout << "Файл успешно зашифрован. Секретный ключ: " << x << endl;
                }
            } else {
                cout << "\nРезультат шифрования:\n";
                cout << "Ключ: " << x << endl;
                cout << "Шифротекст (пары a b): ";
                for (const auto& pair : ciphertext) {
                    cout << pair.first << " " << pair.second << " ";
                }
                cout << endl;
            }
        } 
        else if (action == 2) {
            if (source == 2) {
                int key = safeInputInt("Введите секретный ключ: ");
                cout << "Введите зашифрованные данные (пары a b через пробел): ";
                
                vector<pair<int, int>> ciphertext;
                string input;
                getline(cin, input);
                stringstream ss(input);
                int a, b;
                
                while (ss >> a >> b) {
                    ciphertext.emplace_back(a, b);
                }
                
                if (!ciphertext.empty()) {
                    auto decrypted = decryptBinary(ciphertext, key);
                    cout << "\nРасшифрованный текст: " << string(decrypted.begin(), decrypted.end()) << endl;
                } else {
                    cerr << "Ошибка: неверный формат ввода\n";
                }
            } 
            else {

                string filename = safeInputString("Введите имя файла с зашифрованными данными: ");
                int key = safeInputInt("Введите секретный ключ: ");
                
                auto ciphertext = readCiphertext(filename);
                if (!ciphertext.empty()) {
                    auto decrypted = decryptBinary(ciphertext, key);
                    string outputFilename = safeInputString("Введите имя файла для сохранения: ");
                    if (writeFile(outputFilename, decrypted)) {
                        cout << "Файл успешно расшифрован" << endl;
                    }
                }
            }
        }
    }
}

}
