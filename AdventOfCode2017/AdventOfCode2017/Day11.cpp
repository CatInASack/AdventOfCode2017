#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    enum direction
    {
        n  = 0,
        ne = 1,
        se = 2,
        s  = 3,
        sw = 4,
        nw = 5,
    };

    const map<string, direction> Directions =
    {
        { "n"s, direction::n },
        { "ne"s, direction::ne },
        { "se"s, direction::se },
        { "s"s, direction::s },
        { "sw"s, direction::sw },
        { "nw"s, direction::nw },
    };

    void parseDirection(const string& str, vector<direction>& directions)
    {
        if (!str.empty())
        {
            auto directionIter = Directions.find(str);
            if (directionIter != Directions.end())
            {
                directions.push_back(directionIter->second);
            }
            else
            {
                Assert::Fail(L"Invalid direction in input");
            }
        }
    }

    vector<direction> parsePath(const string& path)
    {
        istringstream stream(path);
        vector<direction> result;

        string buffer;
        for (auto letter = path.begin(); letter != path.end(); letter++)
        {
            switch (*letter)
            {
            case 'n':
            case 's':
            case 'e':
            case 'w':
                buffer.push_back(*letter);
                break;
            case ',':
                parseDirection(buffer, result);
                buffer.resize(0);
                break;
            }
        }

        parseDirection(buffer, result);

        return result;
    }

    TEST_CLASS(Day11)
    {
    private:
        class walker
        {
        private:
            int x;
            int y;
            int furthest;
        public:
            walker() : x(0), y(0), furthest(0) { }

            int getX() const { return x; }

            int getY() const { return y; }

            int getDistance() const { return getShortestPath().size(); }

            // It would be greatly more efficient to solve the problem by tracking number of each type
            // of steps and cancelling/coalescing steps, instead of trying to maintain the actual position
            // on the grid. It would also remove the unexpected error conditions where the x/y falls on a
            // grid line. But, I already solved it the long way and hardware can cover for me :)
            int getFurthest() const { return furthest; }

            vector<direction> getShortestPath() const
            {
                vector<direction> result;
                auto dX = x;
                auto dY = y;

                while (dX != 0 || dY != 0)
                {
                    if (dX > 0 && dY > 0)
                    {
                        result.push_back(ne);
                        dX -= 1;
                        dY -= 1;
                    }
                    else if (dX == 0 && dY > 0)
                    {
                        result.push_back(n);
                        dY -= 2;
                    }
                    else if (dX < 0 && dY > 0)
                    {
                        result.push_back(nw);
                        dX += 1;
                        dY -= 1;
                    }
                    else if (dX < 0 && dY == 0)
                    {
                        Assert::IsTrue(dX % 2 == 0, L"Moving to an invalid grid point (W).");
                        result.push_back(nw);
                        result.push_back(sw);
                        dX += 2;
                    }
                    else if (dX < 0 && dY < 0)
                    {
                        result.push_back(sw);
                        dX += 1;
                        dY += 1;
                    }
                    else if (dX == 0 && dY < 0)
                    {
                        result.push_back(s);
                        dY += 2;
                    }
                    else if (dX > 0 && dY < 0)
                    {
                        result.push_back(se);
                        dX -= 1;
                        dY += 1;
                    }
                    else if (dX > 0 && dY == 0)
                    {
                        Assert::IsTrue(dX % 2 == 0, L"Moving to an invalid grid point (E).");
                        result.push_back(ne);
                        result.push_back(se);
                        dX -= 2;
                    }
                }

                return result;
            }

            walker& walk(direction d)
            {
                switch (d)
                {
                case n: { y += 2; break; }
                case ne: { x += 1; y += 1; break; }
                case se: { x += 1; y -= 1; break; }
                case s: { y -= 2; break; }
                case sw: { x -= 1; y -= 1; break; }
                case nw: { x -= 1; y += 1; break; }
                }

                furthest = max(furthest, getDistance());

                return *this;
            }
        };
    public:

        TEST_METHOD(Day11_1_Test1)
        {
            auto directions = parsePath("n,ne,se,s,sw,nw"s);
            Assert::AreEqual(size_t(6), directions.size());
            Assert::AreEqual((int)n, (int)directions[0]);
            Assert::AreEqual((int)ne, (int)directions[1]);
            Assert::AreEqual((int)se, (int)directions[2]);
            Assert::AreEqual((int)s, (int)directions[3]);
            Assert::AreEqual((int)sw, (int)directions[4]);
            Assert::AreEqual((int)nw, (int)directions[5]);
        }

        TEST_METHOD(Day11_1_Test2)
        {
            walker w;
            w.walk(ne).walk(ne).walk(ne);
            Assert::AreEqual(3, w.getX(), L"x");
            Assert::AreEqual(3, w.getY(), L"y");
            Assert::AreEqual(3, w.getDistance(), L"distance");
        }

        TEST_METHOD(Day11_1_Test3)
        {
            walker w;
            w.walk(ne).walk(ne).walk(sw).walk(sw);
            Assert::AreEqual(0, w.getX(), L"x");
            Assert::AreEqual(0, w.getY(), L"y");
            Assert::AreEqual(0, w.getDistance(), L"distance");
        }

        TEST_METHOD(Day11_1_Test4)
        {
            walker w;
            w.walk(ne).walk(ne).walk(s).walk(s);
            Assert::AreEqual(2, w.getX(), L"x");
            Assert::AreEqual(-2, w.getY(), L"y");
            Assert::AreEqual(2, w.getDistance(), L"distance");
        }

        TEST_METHOD(Day11_1_Test5)
        {
            walker w;
            w.walk(se).walk(sw).walk(se).walk(sw).walk(sw);
            Assert::AreEqual(-1, w.getX(), L"x");
            Assert::AreEqual(-5, w.getY(), L"y");
            Assert::AreEqual(3, w.getDistance(), L"distance");
        }

        TEST_METHOD(Day11_1_2_Final)
        {
            auto input = ReadFile("C:\\Day11.txt");
            auto steps = parsePath(input);

            walker w;
            for_each(steps.begin(), steps.end(), [&w](auto step) { w.walk(step); });

            Assert::AreEqual(764, w.getDistance());
            Assert::AreEqual(1532, w.getFurthest());
        }
    };
}