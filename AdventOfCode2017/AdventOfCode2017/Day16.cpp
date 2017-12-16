#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <sstream>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day16)
    {
    private:
        enum danceMove
        {
            Spin = 0,
            Exchange = 1,
            Partner = 2,
        };

        union operand
        {
            int position;
            char program;
        };

        template<int NumPrograms>
        class line
        {
        private:
            char programs[NumPrograms];
            int startIndex;
        public:
            line() : startIndex(0)
            {
                char current = 'a';

                generate(
                    &programs[0],
                    &programs[NumPrograms  -1],
                    [&current]() { return current++; }
                    );
            }

            char& operator[](int program)
            {
                return programs[(program + startIndex) % NumPrograms];
            }
        };

        class step
        {
        private:
            danceMove move;
            operand operandA;
            operand operandB;

        public:
            step(int pos) : move(danceMove::Spin)
            {
                operandA.position = pos;
            }

            step(int posA, int posB) : move(danceMove::Exchange)
            {
                operandA.position = posA;
                operandB.position = posB;
            }

            step(char progA, char progB) : move(danceMove::Partner)
            {
                operandA.program = progA;
                operandB.program = progB;
            }

            danceMove getMove() const { return move; }

            const operand& getOperandA() const { return operandA; }

            const operand& getOperandB() const { return operandB; }

            void dance(vector<char>& programs) const
            {
                switch (move)
                {
                case danceMove::Spin:
                    for (auto n = 0; n < operandA.position; n++)
                    {
                        programs.insert(programs.begin(), programs.back());
                        programs.pop_back();
                    }
                    break;
                case danceMove::Exchange:
                    swap(programs[operandA.position], programs[operandB.position]);
                    break;
                case danceMove::Partner:
                    auto p1 = find(programs.begin(), programs.end(), operandA.program);
                    auto p2 = find(programs.begin(), programs.end(), operandB.program);
                    swap(*p1, *p2);
                    break;
                }
            }
        };

        static vector<step> parseSteps(const string& input)
        {
            vector<step> result;

            istringstream stream(input);

            while (!stream.eof())
            {
                char m;
                int posA;
                int posB;
                char progA;
                char progB;

                stream >> m;

                switch (m)
                {
                case 's':
                    stream >> posA;
                    result.emplace_back(posA);
                    break;
                case 'x':
                    stream >> posA >> m >> posB;
                    result.emplace_back(posA, posB);
                    break;
                case 'p':
                    stream >> progA >> m >> progB;
                    result.emplace_back(progA, progB);
                    break;
                }
            }

            return result;
        }

        static vector<char> initialLine(size_t size)
        {
            vector<char> result;
            result.resize(size);
            char current = 'a';

            generate(
                result.begin(),
                result.end(),
                [&current]() { return current++; }
            );

            return result;
        }

    public:
        TEST_METHOD(Day16_1_Test1)
        {
            auto result = parseSteps("s1,x3/4,pe/b"s);
            Assert::AreEqual(size_t(3), result.size());
            Assert::AreEqual((int)danceMove::Spin, (int)result[0].getMove(), L"M1");
            Assert::AreEqual(1, result[0].getOperandA().position, L"A1");
            Assert::AreEqual((int)danceMove::Exchange, (int)result[1].getMove(), L"M2");
            Assert::AreEqual(3, result[1].getOperandA().position, L"A2");
            Assert::AreEqual(4, result[1].getOperandB().position, L"B2");
            Assert::AreEqual((int)danceMove::Partner, (int)result[2].getMove(), L"M3");
            Assert::AreEqual('e', result[2].getOperandA().program, L"A3");
            Assert::AreEqual('b', result[2].getOperandB().program, L"B3");
        }

        TEST_METHOD(Day16_1_Test2)
        {
            auto line = initialLine(5);
            Assert::AreEqual(size_t(5), line.size());
            Assert::AreEqual('a', line[0]);
            Assert::AreEqual('b', line[1]);
            Assert::AreEqual('c', line[2]);
            Assert::AreEqual('d', line[3]);
            Assert::AreEqual('e', line[4]);
        }

        TEST_METHOD(Day16_1_Test3)
        {
            auto line = initialLine(5);

            step(1).dance(line);

            Assert::AreEqual(size_t(5), line.size());
            Assert::AreEqual('e', line[0], L"A0");
            Assert::AreEqual('a', line[1], L"A1");
            Assert::AreEqual('b', line[2], L"A2");
            Assert::AreEqual('c', line[3], L"A3");
            Assert::AreEqual('d', line[4], L"A4");

            step(3, 4).dance(line);

            Assert::AreEqual(size_t(5), line.size());
            Assert::AreEqual('e', line[0], L"B0");
            Assert::AreEqual('a', line[1], L"B1");
            Assert::AreEqual('b', line[2], L"B2");
            Assert::AreEqual('d', line[3], L"B3");
            Assert::AreEqual('c', line[4], L"B4");

            step('e', 'b').dance(line);

            Assert::AreEqual(size_t(5), line.size());
            Assert::AreEqual('b', line[0], L"C0");
            Assert::AreEqual('a', line[1], L"C1");
            Assert::AreEqual('e', line[2], L"C2");
            Assert::AreEqual('d', line[3], L"C3");
            Assert::AreEqual('c', line[4], L"C4");
        }

        TEST_METHOD(Day16_1_2_Final)
        {
            auto line = initialLine(16);
            auto steps = parseSteps(ReadFile("C:\\Day16.txt"s));

            for_each(steps.begin(), steps.end(), [&line](const step& aStep) { aStep.dance(line); });

            Assert::AreEqual("lbdiomkhgcjanefp"s, string(line.begin(), line.end()), L"Stage 1");

            //for (auto i = 0; i < 1000000000 - 1; i++)
            //{
            //    for_each(steps.begin(), steps.end(), [&line](const step& aStep) { aStep.dance(line); });
            //}

            //Assert::AreEqual("lbdiomkhgcjanefp"s, string(line.begin(), line.end()), L"Stage 2");
        }
    };
}