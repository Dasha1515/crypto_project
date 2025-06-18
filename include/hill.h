#ifndef HILL_H
#define HILL_H

#include <vector>
#include <string>

namespace HillCipher {
    std::vector<std::vector<int>> generateInvertibleMatrix();
    bool isInvertibleMod256(const std::vector<std::vector<int>>& matrix);
    std::vector<unsigned char> processHill(const std::vector<unsigned char>& input, 
                                         const std::vector<std::vector<int>>& matrix);
    std::vector<std::vector<int>> inverseMatrix(const std::vector<std::vector<int>>& matrix);
    void run();
}

#endif // HILL_H
