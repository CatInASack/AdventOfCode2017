#include "stdafx.h"
#include "CppUnitTest.h"
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <numeric>

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

        static string denseHash(const vector<int>& rawHash)
        {
            Assert::AreEqual(size_t(16), rawHash.size());

            ostringstream out;

            for (auto iter = rawHash.begin(); iter != rawHash.end(); iter++)
            {
                out << hex << setfill('0') << setw(2) << *iter;
            }

            return out.str();
        }

        static vector<int> denseHashRaw(const vector<int>& input)
        {
            Assert::AreEqual(size_t(256), input.size());

            vector<int> result;
            auto iter = input.begin();

            for (auto block = 0; block < 16; block++, iter += 16)
            {
                result.push_back(digestBlock(iter));
            }

            return result;
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

        static vector<int> digestStringRaw(const string& input)
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

            auto rawHash = denseHashRaw(knots.getMarks());
            return rawHash;
        }

        static string digestString(const string& input)
        {
            return denseHash(digestStringRaw(input));
        }

        static int countBlocks(const string& input)
        {
            auto count = 0;
            for (auto row = 0; row < 128; row++)
            {
                auto rowKey = input + "-" + to_string(row);
                auto blocks = digestStringRaw(rowKey);
                count = accumulate(
                    blocks.begin(),
                    blocks.end(),
                    count,
                    [](int rowTotal, int block)
                    {
                        auto innerCount = 0;
                        while (block)
                        {
                            innerCount += block % 2;
                            block >>= 1;
                        }
                        return rowTotal + innerCount;
                    });
            }
            return count;
        }

        template<typename ValueType>
        static ValueType valueAt(const vector<vector<ValueType>>& cells, int x, int y, ValueType defaultValue)
        {
            if (x >= 0 &&
                y >= 0 && 
                size_t(y) < cells.size() &&
                size_t(x) < cells.at(y).size()
                )
            {
                return cells.at(y).at(x);
            }

            return defaultValue;
        }

        static vector<bool> unpack(const vector<int>& blocks)
        {
            Assert::AreEqual(size_t(16), blocks.size());

            vector<bool> result(128, false);
            auto column = 0;

            for (auto iter = blocks.begin(); iter < blocks.end(); iter++)
            {
                auto block = *iter;
                column += 8;

                for (auto bit = 0; bit < 8; bit++)
                {
                    result[column - bit - 1] = block % 2;
                    block >>= 1;
                }
            }

            Assert::AreEqual(size_t(128), result.size());

            return result;
        }

        static bool flood(vector<vector<int>>& colors, const vector<vector<bool>>& cells, int x, int y, int color)
        {
            if (valueAt(cells, x, y, false) && valueAt(colors, x, y, -1) == -1)
            {
                colors[y][x] = color;
                flood(colors, cells, x - 1, y,      color);
                flood(colors, cells, x,     y - 1,  color);
                flood(colors, cells, x + 1, y,      color);
                flood(colors, cells, x,     y + 1,  color);
                return true;
            }

            return false;
        }

        static pair<int, vector<vector<int>>> colorBlocks(const string& input)
        {
            auto count = 0;
            vector<vector<bool>> blockMap;
            blockMap.reserve(128);

            for (auto row = 0; row < 128; row++)
            {
                auto rowKey = input + "-" + to_string(row);
                auto blocks = digestStringRaw(rowKey);
                blockMap.push_back(unpack(blocks));
            }

            vector<vector<int>> colors(128, vector<int>(128, -1));

            for (auto row = 0; row < 128; row++)
            {
                for (auto column = 0; column < 128; column++)
                {
                    if (flood(colors, blockMap, column, row, count + 1))
                    {
                        count++;
                    }
                }
            }

            return make_pair(count, colors);
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

        TEST_METHOD(Day14_1_Test1)
        {
            Assert::AreEqual(8108, countBlocks("flqrgnkx"s));
        }

        TEST_METHOD(Day14_1_Final)
        {
            Assert::AreEqual(8194, countBlocks("uugsqrei"s));
        }

        TEST_METHOD(Day14_2_Test1)
        {
            auto result = colorBlocks("flqrgnkx"s);
            Assert::AreEqual(1242, result.first);
        }

        TEST_METHOD(Day14_2_Final)
        {
            auto result = colorBlocks("uugsqrei"s);
            Assert::AreEqual(1141, result.first);
        }
    };
}