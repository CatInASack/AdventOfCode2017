#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include <algorithm>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{
    TEST_CLASS(Day6)
    {
    private:
        static int FindMax(const std::vector<int>& input)
        {
            auto max = std::max_element(input.begin(), input.end());
            return max - input.begin();
        }

        static void ReAlloc(std::vector<int>& input)
        {
            auto bank = FindMax(input);
            auto blocks = 0;
            std::swap(blocks, input[bank]);

            while (blocks > 0)
            {
                bank = (bank + 1) % input.size();
                input[bank]++;
                blocks--;
            }
        }

        static std::pair<int, int> FindLoop(std::vector<int>& input)
        {
            auto iteration = 0;
            std::map<std::vector<int>, int> pastStates;

            auto result = pastStates.emplace(input, iteration);

            while (result.second)
            {
                iteration++;
                ReAlloc(input);
                result = pastStates.emplace(input, iteration);
            }

            return std::make_pair(pastStates.size(), iteration - result.first->second);
        }

        static int FindLoopThreshold(std::vector<int>& input)
        {
            return FindLoop(input).first;
        }

        static int FindLoopLength(std::vector<int>& input)
        {
            return FindLoop(input).second;
        }

    public:
        TEST_METHOD(Day6_1_Test1)
        {
            std::vector<int> input = { 0, 2, 7, 0, };
            Assert::AreEqual(2, FindMax(input));
        }

        TEST_METHOD(Day6_1_Test2)
        {
            std::vector<int> input = { 3, 1, 2, 3, };
            Assert::AreEqual(0, FindMax(input));
        }

        TEST_METHOD(Day6_1_Test3)
        {
            std::vector<int> input = { 0, 2, 7, 0, };
            ReAlloc(input);
            Assert::AreEqual(size_t(4), input.size());
            Assert::AreEqual(2, input[0]);
            Assert::AreEqual(4, input[1]);
            Assert::AreEqual(1, input[2]);
            Assert::AreEqual(2, input[3]);
        }

        TEST_METHOD(Day6_1_Test4)
        {
            std::vector<int> input = { 3, 1, 2, 3, };
            ReAlloc(input);
            Assert::AreEqual(size_t(4), input.size());
            Assert::AreEqual(0, input[0]);
            Assert::AreEqual(2, input[1]);
            Assert::AreEqual(3, input[2]);
            Assert::AreEqual(4, input[3]);
        }

        TEST_METHOD(Day6_1_Test5)
        {
            std::vector<int> input = { 0, 2, 7, 0, };
            Assert::AreEqual(5, FindLoopThreshold(input));
        }

        TEST_METHOD(Day6_1_Final)
        {
            std::vector<int> input = { 2, 8, 8, 5, 4, 2, 3, 1, 5, 5, 1, 2, 15, 13, 5, 14, };
            Assert::AreEqual(3156, FindLoopThreshold(input));
        }

        TEST_METHOD(Day6_2_Test1)
        {
            std::vector<int> input = { 0, 2, 7, 0, };
            Assert::AreEqual(4, FindLoopLength(input));
        }

        TEST_METHOD(Day6_2_Final)
        {
            std::vector<int> input = { 2, 8, 8, 5, 4, 2, 3, 1, 5, 5, 1, 2, 15, 13, 5, 14, };
            Assert::AreEqual(1610, FindLoopLength(input));
        }
    };
}