#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day7)
    {
    private:
        struct program
        {
            program() { }
            program(const string& n) : name(n) { }

            string parent;
            string name;
            int weight;
            int totalWeight;
            vector<string> children;
        };

        static program ParseProgram(const string& input)
        {
            auto stream = istringstream(input);
            auto p = program();

            stream >> skipws >> p.name;
            stream.ignore(numeric_limits<int>::max(), '(');
            stream >> p.weight;
            stream.ignore(numeric_limits<int>::max(), ')');

            while (!stream.eof())
            {
                string child;
                stream >> child;

                if (child.empty() || child == "->"s)
                {
                    continue;
                }

                if (child[child.size() - 1] == ',')
                {
                    child.pop_back();
                }

                p.children.push_back(child);
            }

            return p;
        }

        static void UpdateMap(const program& p, map<string, program>& map)
        {
            auto addResult = map.emplace(p.name, p);

            if (!addResult.second)
            {
                addResult.first->second.weight = p.weight;
                addResult.first->second.children = p.children;
            }

            for (auto iter = p.children.begin(); iter != p.children.end(); ++iter)
            {
                auto childIter = map.find(*iter);

                if (childIter == map.end())
                {
                    auto child = program();
                    child.name = *iter;
                    child.parent = p.name;
                    map[*iter] = child;
                }
                else
                {
                    childIter->second.parent = p.name;
                }
            }
        }

        static string FindRoot(const map<string, program>& map)
        {
            auto p = map.begin()->second;
            while (!p.parent.empty())
            {
                p = map.find(p.parent)->second;
            }

            return p.name;
        }

        static int Weigh(const string& root, map<string, program>& map)
        {
            program& rootNode = map.find(root)->second;
            auto total = rootNode.weight;

            for (auto child = rootNode.children.begin(); child != rootNode.children.end(); child++)
            {
                total += Weigh(*child, map);
            }

            rootNode.totalWeight = total;
            return total;
        }

        static int ExpectedWeight(const vector<string>& siblings, const map<string, program>& nodes)
        {
            auto counts = map<int, int>();

            for (auto iter = siblings.begin(); iter != siblings.end(); iter++)
            {
                auto candidateWeight = nodes.find(*iter)->second.totalWeight;
                auto count = (++counts[candidateWeight]);
                if (count > 1)
                {
                    return candidateWeight;
                }
            }

            throw 1;
        }

        static pair<string, int> Balance(map<string, program>& map)
        {
            auto root = FindRoot(map);
            auto expectedWeight = Weigh(root, map);
            return Balance(root, expectedWeight, map);
        }

        static pair<string, int> Balance(const string& root, int expectedWeight, const map<string, program>& weighedMap)
        {
            const program& rootNode = weighedMap.find(root)->second;

            if (!rootNode.children.empty())
            {
                auto expectedChild = ExpectedWeight(rootNode.children, weighedMap);

                for (auto iter = rootNode.children.begin(); iter != rootNode.children.end(); iter++)
                {
                    int childWeight = weighedMap.find(*iter)->second.totalWeight;

                    if (childWeight != expectedChild)
                    {
                        return Balance(*iter, expectedChild, weighedMap);
                    }
                }
            }

            auto difference = expectedWeight - rootNode.totalWeight;

            return make_pair(root, rootNode.weight + difference);
        }

    public:
        TEST_METHOD(Day7_1_Test1)
        {
            auto parsed = ParseProgram("pbga (66)"s);

            Assert::AreEqual("pbga"s, parsed.name);
            Assert::AreEqual(66, parsed.weight);
            Assert::AreEqual(size_t(0), parsed.children.size());
        }

        TEST_METHOD(Day7_1_Test2)
        {
            auto parsed = ParseProgram("fwft (72) -> ktlj, cntj, xhth"s);

            Assert::AreEqual("fwft"s, parsed.name);
            Assert::AreEqual(72, parsed.weight);
            Assert::AreEqual(size_t(3), parsed.children.size());
            Assert::AreEqual("ktlj"s, parsed.children[0]);
            Assert::AreEqual("cntj"s, parsed.children[1]);
            Assert::AreEqual("xhth"s, parsed.children[2]);
        }

        TEST_METHOD(Day7_1_Test3)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);
            UpdateMap(ParseProgram("ugml (68)"s), elements);
            UpdateMap(ParseProgram("padx (45)"s), elements);
            UpdateMap(ParseProgram("fwft (72)"s), elements);

            Assert::AreEqual(size_t(4), elements.size());
            Assert::AreEqual("tknk"s, elements["tknk"s].name);
            Assert::AreEqual(41, elements["tknk"s].weight);
            Assert::AreEqual(size_t(3), elements["tknk"s].children.size());
            Assert::IsTrue(elements["tknk"s].parent.empty());

            Assert::AreEqual("ugml"s, elements["ugml"s].name);
            Assert::AreEqual(68, elements["ugml"s].weight);
            Assert::AreEqual(size_t(0), elements["ugml"s].children.size());
            Assert::AreEqual("tknk"s, elements["ugml"s].parent);

            Assert::AreEqual("padx"s, elements["padx"s].name);
            Assert::AreEqual(45, elements["padx"s].weight);
            Assert::AreEqual(size_t(0), elements["padx"s].children.size());
            Assert::AreEqual("tknk"s, elements["padx"s].parent);

            Assert::AreEqual("fwft"s, elements["fwft"s].name);
            Assert::AreEqual(72, elements["fwft"s].weight);
            Assert::AreEqual(size_t(0), elements["fwft"s].children.size());
            Assert::AreEqual("tknk"s, elements["fwft"s].parent);
        }

        TEST_METHOD(Day7_1_Test4)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("ugml (68)"s), elements);
            UpdateMap(ParseProgram("padx (45)"s), elements);
            UpdateMap(ParseProgram("fwft (72)"s), elements);
            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);

            Assert::AreEqual(size_t(4), elements.size());
            Assert::AreEqual("tknk"s, elements["tknk"s].name);
            Assert::AreEqual(41, elements["tknk"s].weight);
            Assert::AreEqual(size_t(3), elements["tknk"s].children.size());
            Assert::IsTrue(elements["tknk"s].parent.empty());

            Assert::AreEqual("ugml"s, elements["ugml"s].name);
            Assert::AreEqual(68, elements["ugml"s].weight);
            Assert::AreEqual(size_t(0), elements["ugml"s].children.size());
            Assert::AreEqual("tknk"s, elements["ugml"s].parent);

            Assert::AreEqual("padx"s, elements["padx"s].name);
            Assert::AreEqual(45, elements["padx"s].weight);
            Assert::AreEqual(size_t(0), elements["padx"s].children.size());
            Assert::AreEqual("tknk"s, elements["padx"s].parent);

            Assert::AreEqual("fwft"s, elements["fwft"s].name);
            Assert::AreEqual(72, elements["fwft"s].weight);
            Assert::AreEqual(size_t(0), elements["fwft"s].children.size());
            Assert::AreEqual("tknk"s, elements["fwft"s].parent);
        }

        TEST_METHOD(Day7_1_Test5)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("ugml (68)"s), elements);
            UpdateMap(ParseProgram("padx (45)"s), elements);
            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);
            UpdateMap(ParseProgram("fwft (72)"s), elements);

            Assert::AreEqual(size_t(4), elements.size());
            Assert::AreEqual("tknk"s, elements["tknk"s].name);
            Assert::AreEqual(41, elements["tknk"s].weight);
            Assert::AreEqual(size_t(3), elements["tknk"s].children.size());
            Assert::IsTrue(elements["tknk"s].parent.empty());

            Assert::AreEqual("ugml"s, elements["ugml"s].name);
            Assert::AreEqual(68, elements["ugml"s].weight);
            Assert::AreEqual(size_t(0), elements["ugml"s].children.size());
            Assert::AreEqual("tknk"s, elements["ugml"s].parent);

            Assert::AreEqual("padx"s, elements["padx"s].name);
            Assert::AreEqual(45, elements["padx"s].weight);
            Assert::AreEqual(size_t(0), elements["padx"s].children.size());
            Assert::AreEqual("tknk"s, elements["padx"s].parent);

            Assert::AreEqual("fwft"s, elements["fwft"s].name);
            Assert::AreEqual(72, elements["fwft"s].weight);
            Assert::AreEqual(size_t(0), elements["fwft"s].children.size());
            Assert::AreEqual("tknk"s, elements["fwft"s].parent);
        }

        TEST_METHOD(Day7_1_Test6)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);
            UpdateMap(ParseProgram("ugml (68)"s), elements);
            UpdateMap(ParseProgram("padx (45)"s), elements);
            UpdateMap(ParseProgram("fwft (72)"s), elements);

            Assert::AreEqual("tknk"s, FindRoot(elements));
        }


        TEST_METHOD(Day7_1_Final)
        {
            auto elements = map<string, program>();

            auto lines = ReadAllLines("C:\\Day7.txt"s);
            for (auto iter = lines.begin(); iter != lines.end(); iter++)
            {
                UpdateMap(ParseProgram(*iter), elements);
            }

            Assert::AreEqual("cqmvs"s, FindRoot(elements));
        }

        TEST_METHOD(Day7_2_Test1)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("ugml (68)"s), elements);
            UpdateMap(ParseProgram("padx (45)"s), elements);
            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);
            UpdateMap(ParseProgram("fwft (72)"s), elements);

            auto total = Weigh("tknk"s, elements);
            Assert::AreEqual(226, total);
            Assert::AreEqual(226, elements["tknk"s].totalWeight);
            Assert::AreEqual(68, elements["ugml"s].totalWeight);
            Assert::AreEqual(45, elements["padx"s].totalWeight);
            Assert::AreEqual(72, elements["fwft"s].totalWeight);
        }

        TEST_METHOD(Day7_2_Test2)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("pbga (66)"s), elements);
            UpdateMap(ParseProgram("xhth (57)"s), elements);
            UpdateMap(ParseProgram("ebii (61)"s), elements);
            UpdateMap(ParseProgram("havc (66)"s), elements);
            UpdateMap(ParseProgram("ktlj (57)"s), elements);
            UpdateMap(ParseProgram("fwft (72) -> ktlj, cntj, xhth"s), elements);
            UpdateMap(ParseProgram("qoyq (66)"s), elements);
            UpdateMap(ParseProgram("padx (45) -> pbga, havc, qoyq"s), elements);
            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);
            UpdateMap(ParseProgram("jptl (61)"s), elements);
            UpdateMap(ParseProgram("ugml (68) -> gyxo, ebii, jptl"s), elements);
            UpdateMap(ParseProgram("gyxo (61)"s), elements);
            UpdateMap(ParseProgram("cntj (57)"s), elements);

            Weigh(FindRoot(elements), elements);

            Assert::AreEqual(243, ExpectedWeight(elements["tknk"s].children, elements));
            Assert::AreEqual(61, ExpectedWeight(elements["ugml"s].children, elements));
            Assert::AreEqual(66, ExpectedWeight(elements["padx"s].children, elements));
            Assert::AreEqual(57, ExpectedWeight(elements["fwft"s].children, elements));
        }

        TEST_METHOD(Day7_2_Test3)
        {
            auto elements = map<string, program>();

            UpdateMap(ParseProgram("pbga (66)"s), elements);
            UpdateMap(ParseProgram("xhth (57)"s), elements);
            UpdateMap(ParseProgram("ebii (61)"s), elements);
            UpdateMap(ParseProgram("havc (66)"s), elements);
            UpdateMap(ParseProgram("ktlj (57)"s), elements);
            UpdateMap(ParseProgram("fwft (72) -> ktlj, cntj, xhth"s), elements);
            UpdateMap(ParseProgram("qoyq (66)"s), elements);
            UpdateMap(ParseProgram("padx (45) -> pbga, havc, qoyq"s), elements);
            UpdateMap(ParseProgram("tknk (41) -> ugml, padx, fwft"s), elements);
            UpdateMap(ParseProgram("jptl (61)"s), elements);
            UpdateMap(ParseProgram("ugml (68) -> gyxo, ebii, jptl"s), elements);
            UpdateMap(ParseProgram("gyxo (61)"s), elements);
            UpdateMap(ParseProgram("cntj (57)"s), elements);

            auto imbalance = Balance(elements);

            Assert::AreEqual("ugml"s, imbalance.first);
            Assert::AreEqual(60, imbalance.second);
        }

        TEST_METHOD(Day7_2_Final)
        {
            auto elements = map<string, program>();

            auto lines = ReadAllLines("C:\\Day7.txt"s);
            for (auto iter = lines.begin(); iter != lines.end(); iter++)
            {
                UpdateMap(ParseProgram(*iter), elements);
            }

            auto imbalance = Balance(elements);
            Assert::AreEqual("vmttcwe"s, imbalance.first);
            Assert::AreEqual(2310, imbalance.second);
        }
    };
}