#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{
    TEST_CLASS(Day3_1)
    {
    private:
        std::pair<int, int> FindCoordinates(int cell)
        {
            auto x = 0;
            auto y = 0;
            auto side = 0;
            auto inc = true;

            auto current = 1;
            while (current < cell)
            {
                auto len = 0;
                while (len <= side && current < cell)
                {
                    x = inc ? x + 1 : x - 1;
                    len++;
                    current++;
                }

                len = 0;
                while (len <= side && current < cell)
                {
                    y = inc ? y + 1 : y - 1;
                    len++;
                    current++;
                }

                side++;
                inc = !inc;
            }

            return std::make_pair(x, y);
        }

        int TaxiDistance(const std::pair<int, int>& coordinates)
        {
            return abs(coordinates.first) + abs(coordinates.second);
        }

    public:
        TEST_METHOD(Day3_1_Test1)
        {
            Assert::AreEqual(0, TaxiDistance(FindCoordinates(1)));
        }

        TEST_METHOD(Day3_1_Test2)
        {
            Assert::AreEqual(3, TaxiDistance(FindCoordinates(12)));
        }

        TEST_METHOD(Day3_1_Test3)
        {
            Assert::AreEqual(2, TaxiDistance(FindCoordinates(23)));
        }

        TEST_METHOD(Day3_1_Test4)
        {
            Assert::AreEqual(31, TaxiDistance(FindCoordinates(1024)));
        }

        TEST_METHOD(Day3_1_Final)
        {
            Assert::AreEqual(438, TaxiDistance(FindCoordinates(265149)));
        }
    };
}