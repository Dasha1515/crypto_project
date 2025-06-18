#ifndef GAML_H
#define GAML_H

#include <vector>
#include <string>
#include <utility>
using namespace std;
namespace ElGamal {
    extern int g, y, x;
    extern const int p;
    
    int safeInputInt(const string& prompt);
    string safeInputString(const string& prompt);
    bool isPrime(int n);
    long long modularExponent(long long a, long long b, long long m);
    int findPrimitiveRoot(int p);
    vector<unsigned char> readFile(const string& filename);
    bool writeFile(const string& filename, const vector<unsigned char>& data);
    bool writeCiphertext(const string& filename, const vector<pair<int, int>>& ciphertext);
    vector<std::pair<int, int>> readCiphertext(const string& filename);
    void generateKeys();
    vector<pair<int, int>> encryptBinary(const vector<unsigned char>& data);
    vector<unsigned char> decryptBinary(const vector<pair<int, int>>& ciphertext, int secretKey);
    vector<unsigned char> getDataFromConsole();
    void run();
}

#endif
