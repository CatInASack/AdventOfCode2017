#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day13)
    {
    private:
        static pair<int, int> parseLayer(const string& description)
        {
            istringstream input(description);

            int layer;
            int depth;
            string dummy;

            input >> skipws >> layer >> dummy >> depth;

            return make_pair(layer, depth);
        }

        static bool isHit(int time, int depth)
        {
            return (depth == 1) || (time % (2 * (depth - 1)) == 0);
        }

        static vector<pair<int, int>> cross(const map<int, int>& layers, int delay = 0)
        {
            vector<pair<int, int>> result;

            for_each(
                layers.begin(),
                layers.end(),
                [&result, delay](const pair<int, int>& layer)
                {
                    if (isHit(layer.first + delay, layer.second))
                    {
                        result.push_back(layer);
                    }
                });

            return result;
        }

        static int severity(const map<int, int>& layers, int delay = 0)
        {
            auto hits = cross(layers, delay);
            auto sev = 0;

            for_each(
                hits.begin(),
                hits.end(),
                [&sev](const pair<int, int>& hit)
                {
                    sev += hit.first * hit.second;
                });

            return sev;
        }

        // Works, but takes 6x longer because it completes all hits for each iteration instead of bailing out after the first one.
        static int delay2(const map<int, int>& layers)
        {
            auto delay = 0;

            while (cross(layers, delay).size() != 0)
            {
                delay++;
            }

            return delay;
        }

        static int delay(const map<int, int>& layers)
        {
            auto delay = 0;

            while (layers.end() != find_if(layers.begin(), layers.end(), [delay](const pair<int, int>& layer) { return isHit(layer.first + delay, layer.second); }))
            {
                delay++;
            }

            return delay;
        }

    public:
        TEST_METHOD(Day13_1_Test1)
        {
            auto description = parseLayer("0: 3"s);
            Assert::AreEqual(0, description.first);
            Assert::AreEqual(3, description.second);
        }

        TEST_METHOD(Day13_1_Test2)
        {
            map<int, int> layers;
            layers.insert(parseLayer("0: 3"s));
            layers.insert(parseLayer("1: 2"s));
            layers.insert(parseLayer("4: 4"s));
            layers.insert(parseLayer("6: 4"s));

            auto hits = cross(layers);
            Assert::AreEqual(size_t(2), hits.size());
            Assert::AreEqual(0, hits[0].first);
            Assert::AreEqual(3, hits[0].second);
            Assert::AreEqual(6, hits[1].first);
            Assert::AreEqual(4, hits[1].second);
        }

        TEST_METHOD(Day13_1_Test3)
        {
            map<int, int> layers;
            layers.insert(parseLayer("0: 3"s));
            layers.insert(parseLayer("1: 2"s));
            layers.insert(parseLayer("4: 4"s));
            layers.insert(parseLayer("6: 4"s));

            Assert::AreEqual(24, severity(layers));
        }

        TEST_METHOD(Day13_1_Final)
        {
            auto lines = ReadAllLines("C:\\Day13.txt"s);
            map<int, int> layers;

            for_each(lines.begin(), lines.end(), [&layers](auto line) { layers.insert(parseLayer(line)); });

            Assert::AreEqual(1504, severity(layers));
        }

        TEST_METHOD(Day13_2_Test1)
        {
            map<int, int> layers;
            layers.insert(parseLayer("0: 3"s));
            layers.insert(parseLayer("1: 2"s));
            layers.insert(parseLayer("4: 4"s));
            layers.insert(parseLayer("6: 4"s));

            Assert::AreEqual(10, delay(layers));
        }

        TEST_METHOD(Day13_2_Final)
        {
            auto lines = ReadAllLines("C:\\Day13.txt"s);
            map<int, int> layers;

            for_each(lines.begin(), lines.end(), [&layers](auto line) { layers.insert(parseLayer(line)); });

            Assert::AreEqual(3823370, delay(layers));
        }
    };
}