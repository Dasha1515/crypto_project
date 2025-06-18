#include "../../include/shamir.h"
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace ShamirProtocol {

int p, c1, d1, c2, d2;

int mod_pow(int a, int b, int m) {
    int result = 1;
    a = a % m;
    while (b > 0) {
        if (b % 2 == 1) result = (result * a) % m;
        a = (a * a) % m;
        b = b / 2;
    }
    return result;
}

bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i*i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int generate_prime(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(min, max);
    int num;
    do { num = dist(gen); } while (!is_prime(num));
    return num;
}

int mod_inverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1)
            return x;
    return -1;
}

void generate_keys() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(2, p - 2);
    
    do { c1 = dist(gen); d1 = mod_inverse(c1, p - 1); } while (d1 == -1);
    do { c2 = dist(gen); d2 = mod_inverse(c2, p - 1); } while (d2 == -1);
}

vector<int> encrypt_message(const string& message) {
    vector<int> result;
    for (char ch : message) {
        int m = static_cast<unsigned char>(ch);
        int step1 = mod_pow(m, c1, p);
        int step2 = mod_pow(step1, c2, p);
        int step3 = mod_pow(step2, d1, p);
        result.push_back(step3);
    }
    return result;
}

string decrypt_message(const vector<int>& encrypted) {
    string result;
    for (int num : encrypted) {
        int m = mod_pow(num, d2, p);
        result += static_cast<char>(m);
    }
    return result;
}

bool file_exists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

vector<int> encrypt_file(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Не удалось открыть файл");
    }
    
    vector<int> result;
    char ch;
    while (file.get(ch)) {
        int m = static_cast<unsigned char>(ch);
        int step1 = mod_pow(m, c1, p);
        int step2 = mod_pow(step1, c2, p);
        int step3 = mod_pow(step2, d1, p);
        result.push_back(step3);
    }
    file.close();
    return result;
}

void save_encrypted_file(const string& filename, const vector<int>& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Не удалось создать файл для сохранения");
    }
    
    for (int num : data) {
        file.write(reinterpret_cast<const char*>(&num), sizeof(num));
    }
    file.close();
}

vector<int> load_encrypted_file(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Не удалось открыть файл с зашифрованными данными");
    }
    
    vector<int> result;
    int num;
    while (file.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        result.push_back(num);
    }
    file.close();
    return result;
}

void save_decrypted_file(const string& filename, const string& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Не удалось создать файл для сохранения");
    }
    
    file.write(data.c_str(), data.size());
    file.close();
}

void create_file_with_content(const string& filename) {
    cout << "Файл не существует, создаем новый" << endl;
    cout << "Введите текст для записи в файл: ";
    string content;
    getline(cin, content);
    
    ofstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Не удалось создать файл");
    }
    
    file.write(content.c_str(), content.size());
    file.close();
    cout << "Файл успешно создан с введенным содержимым." << endl;
}

void encrypt_console() {
    string input;
    cout << "\nВведите сообщение для шифрования: ";
    getline(cin, input);
    
    p = generate_prime(1000, 10000);
    generate_keys();
    
    vector<int> encrypted = encrypt_message(input);
    
    cout << "\n=== Результат шифрования ===" << endl;
    cout << "p: " << p << endl;
    cout << "Ключ Пользователя №1: " << c1 << endl;
    cout << "Ключ Пользователя №2: " << c2 << endl;
    cout << "Зашифрованные данные: ";
    for (int num : encrypted) cout << num << " ";
    cout << endl;
}

void encrypt_file_mode() {
    string filename;
    cout << "\nВведите имя файла для шифрования: ";
    getline(cin, filename);
    
    if (!file_exists(filename)) {
        try {
            create_file_with_content(filename);
        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
            return;
        }
    }
    
    p = generate_prime(1000, 10000);
    generate_keys();
    
    try {
        vector<int> encrypted = encrypt_file(filename);
        
        string output_filename;
        cout << "Введите имя файла для сохранения зашифрованных данных: ";
        getline(cin, output_filename);
        save_encrypted_file(output_filename, encrypted);
        
        cout << "\n=== Результат шифрования ===" << endl;
        cout << "p: " << p << endl;
        cout << "Ключ Пользователя №1: " << c1 << endl;
        cout << "Ключ Пользователя №2: " << c2 << endl;
        cout << "Зашифрованные данные сохранены в файл: " << output_filename << endl;
        
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void decrypt_console() {
    cout << "Введите зашифрованные числа: ";
    string input;
    getline(cin, input);
    
    vector<int> encrypted;
    stringstream ss(input);
    int num;
    while (ss >> num) encrypted.push_back(num);
    
    cout << "Введите p: ";
    cin >> p;
    cout << "Введите ключ Пользователя №1: ";
    cin >> c1;
    cout << "Введите ключ Пользователя №2: ";
    cin >> c2;
    cin.ignore();
    
    d1 = mod_inverse(c1, p - 1);
    d2 = mod_inverse(c2, p - 1);
    
    if (d1 == -1 || d2 == -1) {
        cout << "Некорректные ключи, дешифровка не возможна" << endl;
        return;
    }
    
    string decrypted = decrypt_message(encrypted);
    cout << "\n=== Результат дешифрования ===" << endl;
    cout << "Расшифрованный текст: " << decrypted << endl;
}

void decrypt_file_mode() {
    string filename;
    cout << "Введите имя файла с зашифрованными данными: ";
    getline(cin, filename);
    
    try {
        vector<int> encrypted = load_encrypted_file(filename);
        
        cout << "Введите p: ";
        cin >> p;
        cout << "Введите ключ Пользователя №1: ";
        cin >> c1;
        cout << "Введите ключ Пользователя №2: ";
        cin >> c2;
        cin.ignore();
        
        d1 = mod_inverse(c1, p - 1);
        d2 = mod_inverse(c2, p - 1);
        
        if (d1 == -1 || d2 == -1) {
            cout << "Некорректные ключи, дешифровка не возможна" << endl;
            return;
        }
        
        string decrypted = decrypt_message(encrypted);
        
        string output_filename;
        cout << "Введите имя файла для сохранения расшифрованных данных: ";
        getline(cin, output_filename);
        save_decrypted_file(output_filename, decrypted);
        
        cout << "\n=== Результат дешифрования ===" << endl;
        cout << "Расшифрованные данные сохранены в файл: " << output_filename << endl;
        
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void run() {
    cout << "Трехэтапный протокол Шамира" << endl;
    while (true) {
        cout << "\n=== Протокол Шамира ===" << endl;
        cout << "1. Зашифровать сообщение" << endl;
        cout << "2. Расшифровать сообщение" << endl;
        cout << "3. Выход" << endl;
        cout << "Выберите действие: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice == 3) break;
        
        cout << "\n1. Работа с консолью" << endl;
        cout << "2. Работа с файлом" << endl;
        cout << "Выберите вариант: ";
        int sub_choice;
        cin >> sub_choice;
        cin.ignore();
        
        if (choice == 1) {
            if (sub_choice == 1) encrypt_console();
            else if (sub_choice == 2) encrypt_file_mode();
            else cout << "Неверный выбор!" << endl;
        }
        else if (choice == 2) {
            cout << "\n1. Дешифровать из консоли" << endl;
            cout << "2. Дешифровать из файла" << endl;
            cout << "Выберите вариант: ";
            cin >> sub_choice;
            cin.ignore();
            
            if (sub_choice == 1) decrypt_console();
            else if (sub_choice == 2) decrypt_file_mode();
            else cout << "Неверный выбор!" << endl;
        }
        else {
            cout << "Неверный выбор!" << endl;
        }
    }
}

}
