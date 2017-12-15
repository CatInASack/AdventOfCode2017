#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day15)
    {
    private:
        static uint32_t generate(uint32_t seed, uint32_t factor)
        {
            
            return (uint64_t(seed) * uint64_t(factor)) % 2147483647;
        }

        static const uint32_t mask = 0xFFFF;

        static bool judge(uint32_t a, uint32_t b)
        {
            return (a & mask) == (b & mask);
        }

        template<class _TAA, class _TAB>
        static int dredd(uint32_t a, _TAA acceptA, uint32_t b, _TAB acceptB, uint32_t iterations)
        {
            auto count = 0;
            for (auto i = 0u; i < iterations; i++)
            {
                do { a = generate(a, 16807); } while (!acceptA(a));
                do { b = generate(b, 48271); } while (!acceptB(b));
                if (judge(a, b))
                {
                    count++;
                }
            }

            return count;
        }

        static int judy(uint32_t a, uint32_t b, uint32_t iterations)
        {
            return dredd(
                a,
                [](uint32_t) { return true; },
                b,
                [](uint32_t) { return true; },
                iterations
                );
        }

    public:
        TEST_METHOD(Day15_1_Test1)
        {
            Assert::AreEqual(0x10AB67u, generate(65, 16807), L"A1");
            Assert::AreEqual(0x19AAD337u, generate(8921, 48271), L"B1");

            Assert::AreEqual(0x4664F739u, generate(0x10AB67u, 16807), L"A2");
            Assert::AreEqual(0x49888588u, generate(0x19AAD337u, 48271), L"B2");

            Assert::AreEqual(0xEA2E34Au, generate(0x4664F739u, 16807), L"A3");
            Assert::AreEqual(0x5552E34Au, generate(0x49888588u, 48271), L"B3");

            Assert::AreEqual(0x67F816C7u, generate(0xEA2E34Au, 16807), L"A4");
            Assert::AreEqual(0x837CC07u, generate(0x5552E34Au, 48271), L"B4");

            Assert::AreEqual(0x509F9824u, generate(0x67F816C7u, 16807), L"A5");
            Assert::AreEqual(0x11002804u, generate(0x837CC07u, 48271), L"B5");
        }

        TEST_METHOD(Day15_1_Test2)
        {
            Assert::IsFalse(judge(0x10AB67u, 0x19AAD337u), L"1");
            Assert::IsFalse(judge(0x4664F739u, 0x49888588u), L"2");
            Assert::IsTrue(judge(0xEA2E34Au, 0x5552E34Au), L"3");
            Assert::IsFalse(judge(0x67F816C7u, 0x837CC07u), L"4");
            Assert::IsFalse(judge(0x509F9824u, 0x11002804u), L"5");
        }

        TEST_METHOD(Day15_1_Test3)
        {
            Assert::AreEqual(1, judy(65, 8921, 5));
            Assert::AreEqual(588, judy(65, 8921, 40000000));
        }

        TEST_METHOD(Day15_1_Final)
        {
            Assert::AreEqual(609, judy(883, 879, 40000000));
        }

        TEST_METHOD(Day15_2_Test1)
        {
            auto result = dredd(
                65,
                [](uint32_t value) { return value % 4 == 0; },
                8921,
                [](uint32_t value) { return value % 8 == 0; },
                1056
                );
            Assert::AreEqual(1, result);
        }

        TEST_METHOD(Day15_2_Test2)
        {
            auto result = dredd(
                65,
                [](uint32_t value) { return value % 4 == 0; },
                8921,
                [](uint32_t value) { return value % 8 == 0; },
                5000000
                );
            Assert::AreEqual(309, result);
        }

        TEST_METHOD(Day15_2_Final)
        {
            auto result = dredd(
                883,
                [](uint32_t value) { return value % 4 == 0; },
                879,
                [](uint32_t value) { return value % 8 == 0; },
                5000000
            );
            Assert::AreEqual(253, result);
        }
    };
}