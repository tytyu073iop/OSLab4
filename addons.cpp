#include "addons.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void removeFirstLine(const std::string& filename) {
    std::vector<std::string> lines;
    std::string line;
    
    // Read all lines from file
    std::ifstream inFile(filename);
    while (std::getline(inFile, line)) {
        lines.push_back(line);
    }
    inFile.close();
    
    // Write back all lines except first
    std::ofstream outFile(filename);
    for (size_t i = 1; i < lines.size(); ++i) {
        outFile << lines[i] << '\n';
    }
    outFile.close();
}