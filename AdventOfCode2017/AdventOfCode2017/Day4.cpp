#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{
    TEST_CLASS(Day4)
    {
    private:
        static std::vector<std::string> TokenizeString(const std::string& string)
        {
            std::vector<std::string> result;

            std::string buffer;
            for (auto iter = string.begin(); iter != string.end(); iter++)
            {
                switch (*iter)
                {
                case ' ':
                case '\n':
                case '\r':
                    if (!buffer.empty())
                    {
                        result.push_back(buffer);
                        buffer.clear();
                    }
                    break;
                default:
                    buffer.push_back(*iter);
                    break;
                }
            }

            if (!buffer.empty())
            {
                result.push_back(buffer);
            }

            return result;
        }

        class CompareAnagrams
        {
        public:
            bool operator()(const std::string& left, const std::string& right)
            {
                auto leftSorted(left);
                std::sort(leftSorted.begin(), leftSorted.end(), std::less<char>());
                auto rightSorted(right);
                std::sort(rightSorted.begin(), rightSorted.end(), std::less<char>());

                return std::less<std::string>()(leftSorted, rightSorted);
            }
        };

        template<class Comp=std::less<std::string>>
        static bool IsValidPassphrase(const std::string& phrase)
        {
            auto words = TokenizeString(phrase);
            std::set<std::string, Comp> uniqueWords(words.begin(), words.end());
            return words.size() == uniqueWords.size();
        }

        static std::vector<std::string> ReadAllLines(const std::string& fileName)
        {
            std::vector<std::string> result;

            std::ifstream input(fileName);

            if (input.fail())
            {
                throw 1;
            }

            while (!input.eof())
            {
                char buffer[1024];
                input.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
                result.push_back(buffer);
            }

            input.close();

            return result;
        }

    public:

        TEST_METHOD(Day4_1_Test1)
        {
            auto result = TokenizeString("elem1   e2  word3 four\n");
            Assert::AreEqual(size_t(4), result.size());
            Assert::AreEqual("elem1", result[0].data());
            Assert::AreEqual("e2", result[1].data());
            Assert::AreEqual("word3", result[2].data());
            Assert::AreEqual("four", result[3].data());
        }

        TEST_METHOD(Day4_1_Test2)
        {
            Assert::IsTrue(IsValidPassphrase("aa bb cc dd ee"));
        }

        TEST_METHOD(Day4_1_Test3)
        {
            Assert::IsFalse(IsValidPassphrase("aa bb cc dd aa"));
        }

        TEST_METHOD(Day4_1_Test4)
        {
            Assert::IsTrue(IsValidPassphrase("aa bb cc dd aaa"));
        }

        TEST_METHOD(Day4_1_Test5)
        {
            Assert::IsFalse(IsValidPassphrase("aa aa bb cc dd"));
        }

        TEST_METHOD(Day4_1_Final)
        {
            auto lines = ReadAllLines("C:\\Day4.txt");
            Assert::AreEqual(size_t(512), lines.size());

            auto count = 0;
            for (auto iter = lines.begin(); iter != lines.end(); iter++)
            {
                if (IsValidPassphrase(*iter))
                {
                    count++;
                }
            }
            Assert::AreEqual(383, count);
        }

        TEST_METHOD(Day4_2_Test1)
        {
            Assert::IsTrue(IsValidPassphrase<CompareAnagrams>("abcde fghij"));
        }

        TEST_METHOD(Day4_2_Test2)
        {
            Assert::IsFalse(IsValidPassphrase<CompareAnagrams>("abcde xyz ecdab"));
        }

        TEST_METHOD(Day4_2_Test3)
        {
            Assert::IsTrue(IsValidPassphrase<CompareAnagrams>("a ab abc abd abf abj"));
        }

        TEST_METHOD(Day4_2_Test4)
        {
            Assert::IsTrue(IsValidPassphrase<CompareAnagrams>("iiii oiii ooii oooi oooo"));
        }

        TEST_METHOD(Day4_2_Test5)
        {
            Assert::IsFalse(IsValidPassphrase<CompareAnagrams>("oiii ioii iioi iiio"));
        }

        TEST_METHOD(Day4_2_Final)
        {
            auto lines = ReadAllLines("C:\\Day4.txt");
            Assert::AreEqual(size_t(512), lines.size());

            auto count = 0;
            for (auto iter = lines.begin(); iter != lines.end(); iter++)
            {
                if (IsValidPassphrase<CompareAnagrams>(*iter))
                {
                    count++;
                }
            }
            Assert::AreEqual(265, count);
        }
    };
}