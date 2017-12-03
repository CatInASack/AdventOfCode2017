#include "stdafx.h"
#include "CppUnitTest.h"
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{
    TEST_CLASS(Day3_2)
    {
    private:
        static int FindTestValue(int x, int y, std::map<std::pair<int, int>, int>& values)
        {
            auto total = 0;

            for (auto ii = x - 1; ii <= x + 1; ii++)
            {
                for (auto jj = y - 1; jj <= y + 1; jj++)
                {
                    auto iter = values.find(std::make_pair(ii, jj));

                    if (iter != values.end())
                    {
                        total += iter->second;
                    }
                }
            }

            values[std::make_pair(x, y)] = total;

            return total;
        }

        static int NullValue(int, int, std::map<std::pair<int, int>, int>&)
        {
            return 0;
        }

        static int TaxiDistance(const std::pair<int, int>& coordinates)
        {
            return abs(coordinates.first) + abs(coordinates.second);
        }

        class state
        {
        private:
            int x;
            int y;
            int len;
            int side;
            bool inc;
            bool countingX;
            std::map<std::pair<int, int>, int> values;
            int(*UpdateValueSpace)(int, int, std::map<std::pair<int, int>, int>&);

        public:
            state(int(*updateValueSpace)(int, int, std::map<std::pair<int, int>, int>&), int initialValue)
            {
                x = 0;
                y = 0;
                len = 0;
                side = 1;
                inc = true;
                countingX = true;

                values[std::make_pair(0, 0)] = initialValue;
                UpdateValueSpace = updateValueSpace;
            }

            int ComputeNextValue()
            {
                if (len >= side)
                {
                    len = 0;
                    countingX = !countingX;
                    if (countingX)
                    {
                        side++;
                        inc = !inc;
                    }
                }

                int& coordinate = countingX ? x : y;
                coordinate += inc ? 1 : -1;
                len++;

                auto newValue = UpdateValueSpace(x, y, values);

                return newValue;
            }

            std::pair<int, int> currentCoordinates() const
            {
                return std::make_pair(x, y);
            }
        };

        std::pair<int, int> FindCoordinates(int cell)
        {
            state s(NullValue, 0);
            for (auto current = 1; current < cell; current++)
            {
                s.ComputeNextValue();
            }
            return s.currentCoordinates();
        }

        int FindMemoryTestPattern(int cell)
        {
            state s(FindTestValue, 1);
            int value = 1;
            for (auto current = 1; current < cell; current++)
            {
                value = s.ComputeNextValue();
            }
            return value;
        }

        int FindTestValueThreshold(int threshold)
        {
            state s(FindTestValue, 1);
            while (true)
            {
                auto value = s.ComputeNextValue();

                if (value > threshold)
                {
                    return value;
                }
            }
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

        TEST_METHOD(Day3_2_Test1)
        {
            Assert::AreEqual(1, FindMemoryTestPattern(1));
        }

        TEST_METHOD(Day3_2_Test2)
        {
            Assert::AreEqual(1, FindMemoryTestPattern(2));
        }

        TEST_METHOD(Day3_2_Test3)
        {
            Assert::AreEqual(2, FindMemoryTestPattern(3));
        }

        TEST_METHOD(Day3_2_Test4)
        {
            Assert::AreEqual(4, FindMemoryTestPattern(4));
        }

        TEST_METHOD(Day3_2_Test5)
        {
            Assert::AreEqual(5, FindMemoryTestPattern(5));
        }

        TEST_METHOD(Day3_2_Test6)
        {
            Assert::AreEqual(351, FindMemoryTestPattern(20));
        }

        TEST_METHOD(Day3_2_Test7)
        {
            Assert::AreEqual(122, FindTestValueThreshold(100));
        }

        TEST_METHOD(Day3_2_Final)
        {
            Assert::AreEqual(266330, FindTestValueThreshold(265149));
        }
    };
}