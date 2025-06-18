#include "../../include/hill.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

namespace HillCipher {

//вычисление определителя матрицы
int determinant(const vector<vector<int>>& matrix) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

bool isInvertibleMod256(const vector<vector<int>>& matrix) {
    int det = determinant(matrix) % 256;
    if (det < 0) det += 256;
    return (det % 2) != 0;
}

int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    throw runtime_error("Обратный элемент не существует");
}

vector<vector<int>> inverseMatrix(const vector<vector<int>>& matrix) {
    int det = determinant(matrix);
    det = det % 256;
    if (det < 0) det += 256;
    
    int invDet = modInverse(det, 256);
    
    vector<vector<int>> invMatrix(2, vector<int>(2));
    invMatrix[0][0] = (matrix[1][1] * invDet) % 256;
    invMatrix[0][1] = (-matrix[0][1] * invDet) % 256;
    invMatrix[1][0] = (-matrix[1][0] * invDet) % 256;
    invMatrix[1][1] = (matrix[0][0] * invDet) % 256;
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (invMatrix[i][j] < 0) {
                invMatrix[i][j] += 256;
            }
        }
    }
    
    return invMatrix;
}

//процесс шфирования и дешифрования
vector<unsigned char> processHill(const vector<unsigned char>& input, 
                                const vector<vector<int>>& matrix) {
    vector<unsigned char> output;
    
    for (size_t i = 0; i < input.size(); i += 2) {
        if (i + 1 >= input.size()) break;
        
        int b1 = input[i];
        int b2 = input[i+1];
        
        int c1 = (matrix[0][0] * b1 + matrix[0][1] * b2) % 256;
        int c2 = (matrix[1][0] * b1 + matrix[1][1] * b2) % 256;
        
        if (c1 < 0) c1 += 256;
        if (c2 < 0) c2 += 256;
        
        output.push_back(static_cast<unsigned char>(c1));
        output.push_back(static_cast<unsigned char>(c2));
    }
    
    return output;
}

vector<vector<int>> generateInvertibleMatrix() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    
    vector<vector<int>> matrix(2, vector<int>(2));
    
    while (true) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                matrix[i][j] = dis(gen);
            }
        }
        
        if (isInvertibleMod256(matrix)) {
            return matrix;
        }
    }
}

vector<unsigned char> readFile(const string& filename) {
    ifstream inFile(filename, ios::binary);
    
    if (!inFile) {
        cout << "Файл " << filename << " не существует, создаем новый" << endl;
        cin.ignore();
        
        string userInput;
        cout << "Введите текст для записи в файл: ";
        getline(cin, userInput);
        
        ofstream outFile(filename, ios::binary);
        if (!outFile) {
            throw runtime_error("Не удалось создать файл: " + filename);
        }
        outFile.write(userInput.data(), userInput.size());
        outFile.close();
        
        inFile.open(filename, ios::binary);
        if (!inFile) {
            throw runtime_error("Не удалось открыть созданный файл: " + filename);
        }
    }
    
    vector<unsigned char> content((istreambuf_iterator<char>(inFile)), 
                         istreambuf_iterator<char>());
    return content;
}

void writeFile(const string& filename, const vector<unsigned char>& data) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("Не удалось создать файл: " + filename);
    }
    
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    outFile.close();
}

int getInput(const string& prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << prompt;
        if (!(cin >> choice)) {
            cout << "Ошибка: введите число от " << minVal << " до " << maxVal << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (choice >= minVal && choice <= maxVal) {
            return choice;
        }
        cout << "Введите число от " << minVal << " до " << maxVal << endl;
    }
}

void encryptConsole() {
    cin.ignore();
    string plaintext;
    cout << "Введите текст для шифрования: ";
    getline(cin, plaintext);
    
    vector<unsigned char> bytes(plaintext.begin(), plaintext.end());
    if (bytes.size() % 2 != 0) {
        bytes.push_back(' ');
    }

    vector<vector<int>> keyMatrix = generateInvertibleMatrix();
    vector<unsigned char> ciphertext = processHill(bytes, keyMatrix);
    
    cout << "\n=== Результат шифрования ===\n";
    cout << "Зашифрованное сообщение";
    for (unsigned char c : ciphertext) {
        cout << static_cast<int>(c) << " ";
    }
    cout << "\nКлючевая матрица:\n";
    cout << "[" << keyMatrix[0][0] << " " << keyMatrix[0][1] << "]\n";
    cout << "[" << keyMatrix[1][0] << " " << keyMatrix[1][1] << "]\n";
}

void encryptFile() {
    string inputFile, outputFile;
    cout << "Введите имя файла для шифрования: ";
    cin >> inputFile;
    cout << "Введите имя файла для сохранения зашифрованных данных: ";
    cin >> outputFile;

    try {
        vector<unsigned char> fileContent = readFile(inputFile);
        if (fileContent.size() % 2 != 0) {
            fileContent.push_back(' ');
        }

        vector<vector<int>> keyMatrix = generateInvertibleMatrix();
        vector<unsigned char> ciphertext = processHill(fileContent, keyMatrix);
        writeFile(outputFile, ciphertext);

        cout << "\n=== Результат шифрования ===\n";
        cout << "Зашифрованные данные сохранены в файл: " << outputFile << endl;
        cout << "Ключевая матрица:\n";
        cout << "[" << keyMatrix[0][0] << " " << keyMatrix[0][1] << "]\n";
        cout << "[" << keyMatrix[1][0] << " " << keyMatrix[1][1] << "]\n";

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void decryptConsole() {
    cin.ignore();
    string ciphertextStr;
    cout << "Введите зашифрованное сообщение: ";
    getline(cin, ciphertextStr);
    
    vector<unsigned char> ciphertext;
    istringstream iss(ciphertextStr);
    int num;
    while (iss >> num) {
        ciphertext.push_back(static_cast<unsigned char>(num));
    }

    vector<vector<int>> keyMatrix(2, vector<int>(2));
    cout << "Введите элементы ключевой матрицы:\n";
    cout << "a11: "; keyMatrix[0][0] = getInput("", 0, 255);
    cout << "a12: "; keyMatrix[0][1] = getInput("", 0, 255);
    cout << "a21: "; keyMatrix[1][0] = getInput("", 0, 255);
    cout << "a22: "; keyMatrix[1][1] = getInput("", 0, 255);

    try {
        if (!isInvertibleMod256(keyMatrix)) {
            throw runtime_error("Матрица не обратима по модулю 256");
        }

        vector<vector<int>> invKeyMatrix = inverseMatrix(keyMatrix);
        vector<unsigned char> plaintext = processHill(ciphertext, invKeyMatrix);
        string plaintextStr(plaintext.begin(), plaintext.end());
        
        cout << "\n=== Результат дешифрования ===\n";
        cout << "Расшифрованный текст: " << plaintextStr << endl;
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void decryptFile() {
    string inputFile, outputFile;
    cout << "Введите имя файла с зашифрованными данными: ";
    cin >> inputFile;
    cout << "Введите имя файла для сохранения расшифрованных данных: ";
    cin >> outputFile;

    try {
        vector<vector<int>> keyMatrix(2, vector<int>(2));
        cout << "Введите элементы ключевой матрицы:\n";
        cout << "a11: "; keyMatrix[0][0] = getInput("", 0, 255);
        cout << "a12: "; keyMatrix[0][1] = getInput("", 0, 255);
        cout << "a21: "; keyMatrix[1][0] = getInput("", 0, 255);
        cout << "a22: "; keyMatrix[1][1] = getInput("", 0, 255);

        if (!isInvertibleMod256(keyMatrix)) {
            throw runtime_error("Матрица не обратима по модулю 256");
        }

        vector<unsigned char> ciphertext = readFile(inputFile);
        vector<vector<int>> invKeyMatrix = inverseMatrix(keyMatrix);
        vector<unsigned char> plaintext = processHill(ciphertext, invKeyMatrix);
        writeFile(outputFile, plaintext);

        cout << "\n=== Результат дешифрования ===\n";
        cout << "Расшифрованные данные сохранены в файл: " << outputFile << endl;

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void run() {
    while (true) {
        cout << "\n=== Шифр Хилла ===\n";
        cout << "1. Зашифровать сообщение\n";
        cout << "2. Расшифровать сообщение\n";
        cout << "3. Выход\n";
        
        int choice = getInput("Выберите действие: ", 1, 3);
        
        if (choice == 3) break;
        
        cout << "\n1. Работа с консолью\n";
        cout << "2. Работа с файлом\n";
        int subChoice = getInput("Выберите вариант: ", 1, 2);
        
        if (choice == 1) {
            if (subChoice == 1) encryptConsole();
            else encryptFile();
        } 
        else if (choice == 2) {
            if (subChoice == 1) decryptConsole();
            else decryptFile();
        }
    }
}

} 
