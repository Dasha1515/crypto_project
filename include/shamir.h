#ifndef SHAMIR_H
#define SHAMIR_H

#include <vector>
#include <string>

namespace ShamirProtocol {
    extern int p, c1, d1, c2, d2;
    
    int mod_pow(int a, int b, int m);
    bool is_prime(int n);
    int generate_prime(int min, int max);
    int mod_inverse(int a, int m);
    void generate_keys();
    std::vector<int> encrypt_message(const std::string& message);
    std::string decrypt_message(const std::vector<int>& encrypted);
    bool file_exists(const std::string& filename);
    std::vector<int> encrypt_file(const std::string& filename);
    void save_encrypted_file(const std::string& filename, const std::vector<int>& data);
    std::vector<int> load_encrypted_file(const std::string& filename);
    void save_decrypted_file(const std::string& filename, const std::string& data);
    void create_file_with_content(const std::string& filename);
    void run();
}

#endif // SHAMIR_H
