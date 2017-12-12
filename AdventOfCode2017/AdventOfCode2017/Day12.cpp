#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <map>
#include <sstream>
#include <set>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day12)
    {
    private:
        static pair<int, vector<int>> parsePipes(const string& description)
        {
            istringstream input(description);

            int rootVertex;
            int otherVertex;
            vector<int> pipes;
            string dummy;

            input >> skipws >> rootVertex >> dummy >> otherVertex;

            pipes.push_back(otherVertex);

            while (!input.eof())
            {
                input >> skipws >> dummy >> otherVertex;
                if (!input.fail())
                {
                    pipes.push_back(otherVertex);
                }
            }

            return make_pair(rootVertex, pipes);
        }

        static set<int> brokeAssDjikstra(int source, const map<int, vector<int>>& pipes)
        {
            set<int> group;
            vector<int> unprocessed;

            group.emplace(source);
            unprocessed.insert(unprocessed.end(), pipes.at(source).begin(), pipes.at(source).end());

            while (!unprocessed.empty())
            {
                auto next = unprocessed.back();
                unprocessed.pop_back();
                group.emplace(next);
                for (auto candidate = pipes.at(next).begin(); candidate != pipes.at(next).end(); candidate++)
                {
                    if (group.find(*candidate) == group.end())
                    {
                        unprocessed.push_back(*candidate);
                    }
                }
            }

            return group;
        }

        static int divide(const map<int, vector<int>>& pipes)
        {
            set<int> covered;
            auto count = 0;

            for (auto node = pipes.begin(); node != pipes.end(); node++)
            {
                if (covered.find(node->first) == covered.end())
                {
                    count++;
                    auto group = brokeAssDjikstra(node->first, pipes);
                    covered.insert(group.begin(), group.end());
                }
            }

            return count;
        }

    public:
        TEST_METHOD(Day12_1_Test1)
        {
            auto description = parsePipes("0 <-> 2"s);
            Assert::AreEqual(0, description.first);
            Assert::AreEqual(size_t(1), description.second.size());
            Assert::AreEqual(2, description.second[0]);
        }

        TEST_METHOD(Day12_1_Test2)
        {
            auto description = parsePipes("2 <-> 0, 3, 4"s);
            Assert::AreEqual(2, description.first);
            Assert::AreEqual(size_t(3), description.second.size());
            Assert::AreEqual(0, description.second[0]);
            Assert::AreEqual(3, description.second[1]);
            Assert::AreEqual(4, description.second[2]);
        }

        TEST_METHOD(Day12_1_Test3)
        {
            auto description = parsePipes("2\t<->    0, 3,\t4  \t\n"s);
            Assert::AreEqual(2, description.first);
            Assert::AreEqual(size_t(3), description.second.size());
            Assert::AreEqual(0, description.second[0]);
            Assert::AreEqual(3, description.second[1]);
            Assert::AreEqual(4, description.second[2]);
        }

        TEST_METHOD(Day12_1_Test4)
        {
            map<int, vector<int>> graph;
            graph.insert(parsePipes("0 <-> 2"s));
            graph.insert(parsePipes("1 <-> 1"s));
            graph.insert(parsePipes("2 <-> 0, 3, 4"s));
            graph.insert(parsePipes("3 <-> 2, 4"s));
            graph.insert(parsePipes("4 <-> 2, 3, 6"s));
            graph.insert(parsePipes("5 <-> 6"s));
            graph.insert(parsePipes("6 <-> 4, 5"s));

            auto group = brokeAssDjikstra(0, graph);

            Assert::AreEqual(size_t(6), group.size());
            Assert::IsTrue(group.find(0) != group.end(), L"0");
            Assert::IsTrue(group.find(2) != group.end(), L"2");
            Assert::IsTrue(group.find(3) != group.end(), L"3");
            Assert::IsTrue(group.find(4) != group.end(), L"4");
            Assert::IsTrue(group.find(5) != group.end(), L"5");
            Assert::IsTrue(group.find(6) != group.end(), L"6");
        }

        TEST_METHOD(Day12_1_Final)
        {
            auto lines = ReadAllLines("C:\\Day12.txt"s);
            map<int, vector<int>> graph;

            for_each(lines.begin(), lines.end(), [&graph](auto line) { graph.insert(parsePipes(line)); });

            auto group = brokeAssDjikstra(0, graph);

            Assert::AreEqual(size_t(141), group.size());
        }

        TEST_METHOD(Day12_2_Test1)
        {
            map<int, vector<int>> graph;
            graph.insert(parsePipes("0 <-> 2"s));
            graph.insert(parsePipes("1 <-> 1"s));
            graph.insert(parsePipes("2 <-> 0, 3, 4"s));
            graph.insert(parsePipes("3 <-> 2, 4"s));
            graph.insert(parsePipes("4 <-> 2, 3, 6"s));
            graph.insert(parsePipes("5 <-> 6"s));
            graph.insert(parsePipes("6 <-> 4, 5"s));

            Assert::AreEqual(2, divide(graph));
        }

        TEST_METHOD(Day12_2_Final)
        {
            auto lines = ReadAllLines("C:\\Day12.txt"s);
            map<int, vector<int>> graph;

            for_each(lines.begin(), lines.end(), [&graph](auto line) { graph.insert(parsePipes(line)); });

            Assert::AreEqual(171, divide(graph));
        }
    };
}