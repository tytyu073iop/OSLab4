#include <gtest/gtest.h>
#include "addons.h"

TEST(AddonsTest, generalTest) {
    const std::string fileName = "Googletest.bin";
    std::ofstream MyFile(fileName);
	if (!MyFile.is_open()) {
		std::cerr << "Error opening file!\n";
		FAIL();
	}

    MyFile << "testline\n" << "secondline\n";
    MyFile.close();
    removeFirstLine(fileName);
    std::ifstream MyFileIn(fileName);
	if (!MyFileIn.is_open()) {
		std::cerr << "Error opening file!\n";
		FAIL();
	}

    std::string result = "";
    std::getline(MyFileIn, result);
    EXPECT_EQ("secondline", result);
    MyFileIn.close();
    
}