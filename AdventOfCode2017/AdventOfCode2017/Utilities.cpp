#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <fstream>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{
    std::vector<std::string> ReadAllLines(const std::string& fileName)
    {
        std::vector<std::string> result;

        std::ifstream input(fileName);

        if (input.fail())
        {
            throw 1;
        }

        while (!input.eof())
        {
            char buffer[1024];
            input.getline(buffer, sizeof(buffer) / sizeof(buffer[0]));
            result.push_back(buffer);
        }

        input.close();

        return result;
    }
}