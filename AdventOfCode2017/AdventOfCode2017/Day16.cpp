#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>

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
            char programOrder[NumPrograms];
            int rawPosition[NumPrograms];
            int startIndex;
        public:
            line() : startIndex(0)
            {
                char current = 'a';

                for (int i = 0; i < NumPrograms; i++)
                {
                    programOrder[i] = 'a' + i;
                    rawPosition[i] = i;
                }
            }

            char& operator[](int program)
            {
                return programOrder[(program + startIndex) % NumPrograms];
            }

            void spin(int position) { startIndex = (startIndex + NumPrograms - position) % NumPrograms; }

            void exchange(int positionA, int positionB)
            {
                char& progA = (*this)[positionA];
                char& progB = (*this)[positionB];
                rawPosition[progA - 'a'] = (startIndex + positionB) % NumPrograms;
                rawPosition[progB - 'a'] = (startIndex + positionA) % NumPrograms;
                swap(progA, progB);
            }

            void partner(char programA, char programB)
            {
                int& posA = rawPosition[programA - 'a'];
                int& posB = rawPosition[programB - 'a'];
                char& progA = programOrder[posA];
                char& progB = programOrder[posB];
                swap(posA, posB);
                swap(progA, progB);
            }

            std::string to_string()
            {
                string result;
                result.resize(NumPrograms);
                for (int i = 0; i < NumPrograms; i++)
                {
                    result[i] = (*this)[i];
                }

                return result;
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

            template<int NumPrograms>
            void dance(line<NumPrograms>& programs) const
            {
                switch (move)
                {
                case danceMove::Spin:
                    programs.spin(operandA.position);
                    break;
                case danceMove::Exchange:
                    programs.exchange(operandA.position, operandB.position);
                    break;
                case danceMove::Partner:
                    programs.partner(operandA.program, operandB.program);
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

        template<int NumPrograms>
        static tuple<int, int, map<string, int>> findLoop(std::vector<step>& steps)
        {
            auto iteration = 0;
            std::map<string, int> pastStates;
            line<NumPrograms> line;

            auto result = pastStates.emplace(line.to_string(), iteration);

            while (result.second)
            {
                iteration++;
                for_each(steps.begin(), steps.end(), [&line](const step& aStep) { aStep.dance(line); });
                result = pastStates.emplace(line.to_string(), iteration);
            }

            return make_tuple(pastStates.size(), iteration - result.first->second, move(pastStates));
        }

        static string memoizeLoop(int iteration, const tuple<int, int, map<string, int>>& loopInfo)
        {
            auto loopPeriod = get<1>(loopInfo);
            auto loopOffset = get<0>(loopInfo) - loopPeriod;
            const map<string, int>& iterations = get<2>(loopInfo);

            if (iteration > loopOffset)
            {
                iteration = ((iteration - loopOffset) % loopPeriod) + loopOffset;
            }

            auto matchedIteration = find_if(
                iterations.begin(),
                iterations.end(),
                [iteration](const pair<string, int>& elem) { return elem.second == iteration; }
                );

            return matchedIteration->first;
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
            line<5> line;
            Assert::AreEqual("abcde"s, line.to_string());
            Assert::AreEqual('a', line[0]);
            Assert::AreEqual('b', line[1]);
            Assert::AreEqual('c', line[2]);
            Assert::AreEqual('d', line[3]);
            Assert::AreEqual('e', line[4]);
        }

        TEST_METHOD(Day16_1_Test3)
        {
            line<5> line;

            step(1).dance(line);

            Assert::AreEqual("eabcd"s, line.to_string());
            Assert::AreEqual('e', line[0], L"A0");
            Assert::AreEqual('a', line[1], L"A1");
            Assert::AreEqual('b', line[2], L"A2");
            Assert::AreEqual('c', line[3], L"A3");
            Assert::AreEqual('d', line[4], L"A4");

            step(3, 4).dance(line);

            Assert::AreEqual("eabdc"s, line.to_string());
            Assert::AreEqual('e', line[0], L"B0");
            Assert::AreEqual('a', line[1], L"B1");
            Assert::AreEqual('b', line[2], L"B2");
            Assert::AreEqual('d', line[3], L"B3");
            Assert::AreEqual('c', line[4], L"B4");

            step('e', 'b').dance(line);

            Assert::AreEqual("baedc"s, line.to_string());
            Assert::AreEqual('b', line[0], L"C0");
            Assert::AreEqual('a', line[1], L"C1");
            Assert::AreEqual('e', line[2], L"C2");
            Assert::AreEqual('d', line[3], L"C3");
            Assert::AreEqual('c', line[4], L"C4");
        }

        TEST_METHOD(Day16_1_Final)
        {
            line<16> line;
            auto steps = parseSteps(ReadFile("C:\\Day16.txt"s));

            for_each(steps.begin(), steps.end(), [&line](const step& aStep) { aStep.dance(line); });

            Assert::AreEqual("lbdiomkhgcjanefp"s, line.to_string());
        }

        TEST_METHOD(Day_16_2_Test1)
        {
            auto steps = parseSteps("s1,x3/4,pe/b"s);
            auto loopInfo = findLoop<5>(steps);
            Assert::AreEqual("ceadb"s, memoizeLoop(2, loopInfo));
        }

        TEST_METHOD(Day16_2_Final)
        {
            auto steps = parseSteps(ReadFile("C:\\Day16.txt"s));
            auto loopInfo = findLoop<16>(steps);
            Assert::AreEqual("ejkflpgnamhdcboi"s, memoizeLoop(1000000000, loopInfo));
        }
    };
}