#include "stdafx.h"
#include "CppUnitTest.h"
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day10)
    {
    private:
        static int digestBlock(vector<int>::const_iterator iter)
        {
            auto digest = 0;
            for (auto ii = 0; ii < 16; ii++, iter++)
            {
                digest ^= *iter;
            }

            return digest;
        }

        static string denseHash(const vector<int>& input)
        {
            Assert::AreEqual(size_t(256), input.size());

            ostringstream out;

            auto iter = input.begin();
            for (auto block = 0; block < 16; block++, iter += 16)
            {
                auto blockDigest = digestBlock(iter);
                out << hex << setfill('0') << setw(2) << blockDigest;
            }

            return out.str();
        }

        static vector<int> stringBytes(const string& input)
        {
            vector<int> result;
            result.resize(input.size());
            transform(input.begin(), input.end(), result.begin(), [](auto c) { return toascii(c); });
            return result;
        }

        static vector<int> makeKey(const string& input)
        {
            auto key = stringBytes(input);
            key.push_back(17);
            key.push_back(31);
            key.push_back(73);
            key.push_back(47);
            key.push_back(23);

            return key;
        }

        template<int NumMarks>
        class knots
        {
        private:
            int marks[NumMarks];
            size_t skip;
            size_t pos;
        public:
            knots() : skip(0), pos(0)
            {
                int count = 0;
                generate_n(&marks[0], NumMarks, [&count]() { return count++; });
            }

            vector<int> getMarks() const
            {
                return vector<int>(&marks[0], &marks[0] + NumMarks);
            }

            size_t getPos() const { return pos; }

            size_t getSkip() const { return skip; }

            int getProof() const { return marks[0] * marks[1]; }

            knots& twist(int n)
            {
                for (auto count = 0; count < n / 2; ++count)
                {
                    auto l = &marks[(pos + count) % NumMarks];
                    auto r = &marks[(pos + (n - 1) - count) % NumMarks];
                    swap(*l, *r);
                }

                pos = (pos + n + skip) % NumMarks;
                skip++;

                return *this;
            }
        };

        static string digestString(const string& input)
        {
            knots<256> knots;
            auto key = makeKey(input);

            for (auto round = 0; round < 64; round++)
            {
                for (auto twist = key.begin(); twist != key.end(); twist++)
                {
                    knots.twist(*twist);
                }
            }

            auto marks = knots.getMarks();

            return denseHash(marks);
        }

    public:
        TEST_METHOD(Day10_1_Test0)
        {
            knots<5> k;
            auto marks = k.getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(0, marks[0]);
            Assert::AreEqual(1, marks[1]);
            Assert::AreEqual(2, marks[2]);
            Assert::AreEqual(3, marks[3]);
            Assert::AreEqual(4, marks[4]);
            Assert::AreEqual(size_t(0), k.getPos());
            Assert::AreEqual(size_t(0), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test1)
        {
            knots<5> k;
            auto marks = k.twist(1).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(0, marks[0]);
            Assert::AreEqual(1, marks[1]);
            Assert::AreEqual(2, marks[2]);
            Assert::AreEqual(3, marks[3]);
            Assert::AreEqual(4, marks[4]);
            Assert::AreEqual(size_t(1), k.getPos());
            Assert::AreEqual(size_t(1), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test2)
        {
            knots<5> k;
            auto marks = k.twist(2).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(1, marks[0]);
            Assert::AreEqual(0, marks[1]);
            Assert::AreEqual(2, marks[2]);
            Assert::AreEqual(3, marks[3]);
            Assert::AreEqual(4, marks[4]);
            Assert::AreEqual(size_t(2), k.getPos());
            Assert::AreEqual(size_t(1), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test3)
        {
            knots<5> k;
            auto marks = k.twist(3).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(2, marks[0]);
            Assert::AreEqual(1, marks[1]);
            Assert::AreEqual(0, marks[2]);
            Assert::AreEqual(3, marks[3]);
            Assert::AreEqual(4, marks[4]);
            Assert::AreEqual(size_t(3), k.getPos());
            Assert::AreEqual(size_t(1), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test4)
        {
            knots<5> k;
            auto marks = k.twist(4).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(3, marks[0]);
            Assert::AreEqual(2, marks[1]);
            Assert::AreEqual(1, marks[2]);
            Assert::AreEqual(0, marks[3]);
            Assert::AreEqual(4, marks[4]);
            Assert::AreEqual(size_t(4), k.getPos());
            Assert::AreEqual(size_t(1), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test5)
        {
            knots<5> k;
            auto marks = k.twist(5).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(4, marks[0]);
            Assert::AreEqual(3, marks[1]);
            Assert::AreEqual(2, marks[2]);
            Assert::AreEqual(1, marks[3]);
            Assert::AreEqual(0, marks[4]);
            Assert::AreEqual(size_t(0), k.getPos());
            Assert::AreEqual(size_t(1), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test6)
        {
            knots<5> k;
            auto marks = k.twist(3).twist(4).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(4, marks[0]);
            Assert::AreEqual(3, marks[1]);
            Assert::AreEqual(0, marks[2]);
            Assert::AreEqual(1, marks[3]);
            Assert::AreEqual(2, marks[4]);
            Assert::AreEqual(size_t(3), k.getPos());
            Assert::AreEqual(size_t(2), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test7)
        {
            knots<5> k;
            auto marks = k.twist(3).twist(4).twist(1).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(4, marks[0]);
            Assert::AreEqual(3, marks[1]);
            Assert::AreEqual(0, marks[2]);
            Assert::AreEqual(1, marks[3]);
            Assert::AreEqual(2, marks[4]);
            Assert::AreEqual(size_t(1), k.getPos());
            Assert::AreEqual(size_t(3), k.getSkip());
        }

        TEST_METHOD(Day10_1_Test8)
        {
            knots<5> k;
            auto marks = k.twist(3).twist(4).twist(1).twist(5).getMarks();

            Assert::AreEqual(size_t(5), marks.size());
            Assert::AreEqual(3, marks[0]);
            Assert::AreEqual(4, marks[1]);
            Assert::AreEqual(2, marks[2]);
            Assert::AreEqual(1, marks[3]);
            Assert::AreEqual(0, marks[4]);
            Assert::AreEqual(size_t(4), k.getPos());
            Assert::AreEqual(size_t(4), k.getSkip());

            Assert::AreEqual(12, k.getProof());
        }

        TEST_METHOD(Day10_1_Final)
        {
            knots<256> k;
            auto twists = { 183, 0, 31, 146, 254, 240, 223, 150, 2, 206, 161, 1, 255, 232, 199, 88 };

            for_each(twists.begin(), twists.end(), [&k](auto t) { k.twist(t); });

            Assert::AreEqual(15990, k.getProof());
        }

        TEST_METHOD(Day10_2_Test1)
        {
            vector<int> bytes = { 65, 27, 9, 1, 4, 3, 40, 50, 91, 7, 6, 0, 2, 5, 68, 22 };
            Assert::AreEqual(64, digestBlock(bytes.begin()));
        }

        TEST_METHOD(Day10_2_Test2)
        {
            auto input = "1,2,3"s;
            auto result = stringBytes(input);

            Assert::AreEqual(size_t(5), result.size());
            Assert::AreEqual(49, result[0]);
            Assert::AreEqual(44, result[1]);
            Assert::AreEqual(50, result[2]);
            Assert::AreEqual(44, result[3]);
            Assert::AreEqual(51, result[4]);
        }

        TEST_METHOD(Day10_2_Test3)
        {
            Assert::AreEqual("a2582a3a0e66e6e86e3812dcb672a272"s, digestString(""s));
        }

        TEST_METHOD(Day10_2_Test4)
        {
            Assert::AreEqual("33efeb34ea91902bb2f59c9920caa6cd"s, digestString("AoC 2017"s));
        }

        TEST_METHOD(Day10_2_Test5)
        {
            Assert::AreEqual("3efbe78a8d82f29979031a4aa0b16a9d"s, digestString("1,2,3"s));
        }

        TEST_METHOD(Day10_2_Test6)
        {
            Assert::AreEqual("63960835bcdc130f0b66d7ff4f6a5a8e"s, digestString("1,2,4"s));
        }

        TEST_METHOD(Day10_2_Final)
        {
            Assert::AreEqual("90adb097dd55dea8305c900372258ac6"s, digestString("183,0,31,146,254,240,223,150,2,206,161,1,255,232,199,88"s));
        }
    };
}