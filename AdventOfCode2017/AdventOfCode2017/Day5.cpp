#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{
    TEST_CLASS(Day5)
    {
    private:
        std::vector<int> ConvertToInt(const std::vector<std::string>& input)
        {
            std::vector<int> output;
            output.resize(input.size());

            auto toInt = [](const std::string& str)
            {
                return std::stoi(str);
            };

            std::transform(input.begin(), input.end(), output.begin(), toInt);

            return output;
        }

        static void Increment(int& i) { i++; }

        static void Part2Update(int& i)
        {
            if (i >= 3)
            {
                i--;
            }
            else
            {
                i++;
            }
        }

        template<void(*Update)(int&)=Increment>
        class state
        {
        private:
            std::vector<int> jumps;
            size_t pc;
            int moves;
        public:
            state(const std::vector<int>& initJumps) : jumps(initJumps), pc(0), moves(0)
            {
            }

            void ComputeNextMove()
            {
                auto adjustment = jumps[pc];
                Update(jumps[pc]);
                pc += adjustment;
                moves++;
            }

            bool IsEscaped() const { return pc >= jumps.size(); }

            int Moves() const { return moves; }

            int Solve()
            {
                while (!IsEscaped())
                {
                    ComputeNextMove();
                }

                return Moves();
            }
        };

    public:

        TEST_METHOD(Day5_1_Test1)
        {
            std::vector<std::string> data = { "1", "2", "3", "5", };
            auto result = ConvertToInt(data);
            Assert::AreEqual(size_t(4), result.size());
            Assert::AreEqual(1, result[0]);
            Assert::AreEqual(2, result[1]);
            Assert::AreEqual(3, result[2]);
            Assert::AreEqual(5, result[3]);
        }

        TEST_METHOD(Day5_1_Test2)
        {
            std::vector<int> input = { 0, 3, 0, 1, -3, };
            Assert::AreEqual(5, state<>(input).Solve());
        }

        TEST_METHOD(Day5_1_Final)
        {
            auto raw = ReadAllLines("C:\\Day5.txt");
            auto input = ConvertToInt(raw);
            Assert::AreEqual(351282, state<>(input).Solve());
        }

        TEST_METHOD(Day5_2_Test1)
        {
            std::vector<int> input = { 0, 3, 0, 1, -3, };
            Assert::AreEqual(10, state<Part2Update>(input).Solve());
        }

        TEST_METHOD(Day5_2_Final)
        {
            auto raw = ReadAllLines("C:\\Day5.txt");
            auto input = ConvertToInt(raw);
            Assert::AreEqual(24568703, state<Part2Update>(input).Solve());
        }
    };
}