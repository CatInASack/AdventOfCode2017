#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day17)
    {
    private:
        pair<size_t, vector<int>> insert(int iterations, int step)
        {
            vector<int> data;
            data.reserve(iterations);
            data.push_back(0);

            size_t pos = 0;

            for (auto currentIteration = 0; currentIteration < iterations; currentIteration++)
            {
                pos = (pos + step) % data.size();
                pos = (pos + 1) % (data.size() + 1);

                auto insertionPoint = pos >= data.size() ? data.end() : data.begin() + pos;

                data.insert(insertionPoint, currentIteration + 1);
            }

            return make_pair(pos, data);
        }

    public:
        TEST_METHOD(Day17_1_Test1)
        {
            auto result = insert(1, 3);
            Assert::AreEqual(1u, result.first);
            Assert::AreEqual(2u, result.second.size());
            Assert::AreEqual(0, result.second[0], L"1");
            Assert::AreEqual(1, result.second[1], L"2");
        }

        TEST_METHOD(Day17_1_Test2)
        {
            auto result = insert(2, 3);
            Assert::AreEqual(1u, result.first);
            Assert::AreEqual(3u, result.second.size());
            Assert::AreEqual(0, result.second[0], L"1");
            Assert::AreEqual(2, result.second[1], L"2");
            Assert::AreEqual(1, result.second[2], L"3");
        }

        TEST_METHOD(Day17_1_Test3)
        {
            auto result = insert(3, 3);
            Assert::AreEqual(2u, result.first);
            Assert::AreEqual(4u, result.second.size());
            Assert::AreEqual(0, result.second[0], L"1");
            Assert::AreEqual(2, result.second[1], L"2");
            Assert::AreEqual(3, result.second[2], L"3");
            Assert::AreEqual(1, result.second[3], L"4");
        }

        TEST_METHOD(Day17_1_Test4)
        {
            auto result = insert(2017, 3);
            Assert::AreEqual(1530u, result.first);
            Assert::AreEqual(2018u, result.second.size());
            Assert::AreEqual(638, result.second[(result.first + 1) % result.second.size()]);
        }

        TEST_METHOD(Day17_1_Final)
        {
            auto result = insert(2017, 356);
            Assert::AreEqual(1024u, result.first);
            Assert::AreEqual(2018u, result.second.size());
            Assert::AreEqual(808, result.second[(result.first + 1) % result.second.size()]);
        }

        //TEST_METHOD(Day17_2_Final)
        //{
        //    auto result = insert(50000000, 356);
        //    Assert::AreEqual(1024u, result.first);
        //    Assert::AreEqual(2018u, result.second.size());
        //    Assert::AreEqual(808, result.second[(result.first + 1) % result.second.size()]);
        //}
    };
}