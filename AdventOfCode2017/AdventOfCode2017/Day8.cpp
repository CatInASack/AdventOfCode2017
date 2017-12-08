#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017::Day8
{
    enum operation
    {
        inc = 0,
        dec = 1,
    };

    map<string, operation> Operations =
    {
        { "inc", operation::inc },
        { "dec", operation::dec },
    };

    enum comparison
    {
        gt = 0,
        gte = 1,
        lt = 2,
        lte = 3,
        eq = 4,
        neq = 5,
    };

    map<string, comparison> Comparisons =
    {
        { ">"s, comparison::gt },
        { ">="s, comparison::gte },
        { "<"s, comparison::lt },
        { "<="s, comparison::lte },
        { "=="s, comparison::eq },
        { "!="s, comparison::neq },
    };

    struct guard
    {
        string reg;
        comparison comp;
        int value;

        bool Evaluate(const map<string, int> regs)
        {
            auto regState = regs.find(reg);
            auto actualValue = regState != regs.end() ? regState->second : 0;

            switch (comp)
            {
            case comparison::gt:
                return actualValue > value;
            case comparison::gte:
                return actualValue >= value;
            case comparison::lt:
                return actualValue < value;
            case comparison::lte:
                return actualValue <= value;
            case comparison::eq:
                return actualValue == value;
            case comparison::neq:
                return actualValue != value;
            default:
                throw 1;
            }
        }
    };

    struct program
    {
        string reg;
        operation op;
        int amount;
        guard guard;

        int Evaluate(map<string, int>& regs)
        {
            if (guard.Evaluate(regs))
            {
                switch (op)
                {
                case inc:
                    regs[reg] += amount;
                    break;
                case dec:
                    regs[reg] -= amount;
                    break;
                }
            }

            return regs[reg];
        }
    };

    TEST_CLASS(Day8)
    {
    private:
        static program ParseProgram(const string& input)
        {
            auto stream = istringstream(input);
            auto p = program();
            string op;
            string dummy;
            string comp;

            stream >> skipws >> p.reg >> op >> p.amount >> dummy >> p.guard.reg >> comp >> p.guard.value;

            p.op = Operations[op];
            p.guard.comp = Comparisons[comp];

            return p;
        }

    public:
        TEST_METHOD(Day8_1_Test1)
        {
            auto parsed = ParseProgram("b inc 5 if a > 1"s);

            Assert::AreEqual("b"s, parsed.reg);
            Assert::AreEqual((int)operation::inc, (int)parsed.op);
            Assert::AreEqual(5, parsed.amount);
            Assert::AreEqual("a"s, parsed.guard.reg);
            Assert::AreEqual((int)comparison::gt, (int)parsed.guard.comp);
            Assert::AreEqual(1, parsed.guard.value);
        }

        TEST_METHOD(Day8_1_Test2)
        {
            auto parsed = ParseProgram("a inc 1 if b < 5"s);

            Assert::AreEqual("a"s, parsed.reg);
            Assert::AreEqual((int)operation::inc, (int)parsed.op);
            Assert::AreEqual(1, parsed.amount);
            Assert::AreEqual("b"s, parsed.guard.reg);
            Assert::AreEqual((int)comparison::lt, (int)parsed.guard.comp);
            Assert::AreEqual(5, parsed.guard.value);
        }

        TEST_METHOD(Day8_1_Test3)
        {
            map<string, int> regs;
            Assert::AreEqual(0, ParseProgram("b inc 5 if a > 1"s).Evaluate(regs));
            Assert::IsFalse(regs.find("a"s) != regs.end());
            Assert::IsTrue(regs.find("b"s) != regs.end());
            Assert::AreEqual(0, regs["b"s]);
        }

        TEST_METHOD(Day8_1_Test4)
        {
            map<string, int> regs;
            Assert::AreEqual(1, ParseProgram("a inc 1 if b < 5"s).Evaluate(regs));
            Assert::IsFalse(regs.find("b"s) != regs.end());
            Assert::IsTrue(regs.find("a"s) != regs.end());
            Assert::AreEqual(1, regs["a"s]);
        }

        TEST_METHOD(Day8_1_2_Final)
        {
            auto regs = map<string, int>();

            auto lines = ReadAllLines("C:\\Day8.txt"s);

            auto maxEver = 0;
            for (auto iter = lines.begin(); iter != lines.end(); iter++)
            {
                maxEver = max(maxEver, ParseProgram(*iter).Evaluate(regs));
            }

            auto maxValue = max_element(
                regs.begin(),
                regs.end(),
                [](const pair<string, int>& left, const pair<string, int>& right)
                {
                    return left.second < right.second;
                })->second;

            Assert::AreEqual(5946, maxValue);
            Assert::AreEqual(6026, maxEver);
        }
    };
}