#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day19)
    {
    private:
        enum step
        {
            none = 0,
            left = 1,
            up = 2,
            down = 3,
            right = 4,
        };

        struct state
        {
            int x;
            int y;
            int stepCount;
            step lastStep;
            shared_ptr<string> trail;
        };

        class board
        {
        private:
            vector<string> lines;

            step keepGoingOrStop(int x, int y, step lastMove) const
            {
                switch (lastMove)
                {
                case up:
                    y--;
                    break;
                case down:
                    y++;
                    break;
                case left:
                    x--;
                    break;
                case right:
                    x++;
                    break;
                }

                return pieceAt(x, y) != ' ' ? lastMove : none;
            }

            static step opposite(step direction)
            {
                switch (direction)
                {
                case left: return right;
                case up: return down;
                case right: return left;
                case down: return up;
                default: return none;
                }
            }

        public:
            board(const vector<string>& input) : lines(input) { }

            step exitFrom(int x, int y, step lastMove) const
            {
                switch (pieceAt(x, y))
                {
                case ' ':
                    return none;
                case '+':
                    for (auto direction = static_cast<int>(left); direction <= static_cast<int>(right); direction++)
                    {
                        if (lastMove != opposite(static_cast<step>(direction)) && keepGoingOrStop(x, y, static_cast<step>(direction)))
                        {
                            return static_cast<step>(direction);
                        }
                    }
                    return none;
                default:
                    return keepGoingOrStop(x, y, lastMove);
                }
            }

            char pieceAt(int x, int y) const
            {
                auto value = ' ';

                if (y >= 0 && y < static_cast<int>(lines.size()) &&
                    x >= 0 && x < static_cast<int>(lines.at(y).size()))
                {
                    value = lines.at(y).at(x);
                }

                return value;
            }

            state initialState() const
            {
                auto pos = find(lines.at(0).begin(), lines.at(0).end(), '|');

                state initial;
                initial.stepCount = 1;
                initial.lastStep = down;
                initial.y = 0;
                initial.x = pos - lines.at(0).begin();
                initial.trail = make_shared<string>(""s);

                return initial;
            }

            state advance(const state& current) const
            {
                state next;
                switch (pieceAt(current.x, current.y))
                {
                case '|':
                case '-':
                case ' ':
                case '+':
                    next.trail = current.trail;
                    break;
                default:
                    next.trail = make_shared<string>(*current.trail + pieceAt(current.x, current.y));
                    break;
                }

                next.x = current.x;
                next.y = current.y;
                next.lastStep = exitFrom(current.x, current.y, current.lastStep);

                switch (next.lastStep)
                {
                case up:
                    next.y--;
                    break;
                case down:
                    next.y++;
                    break;
                case left:
                    next.x--;
                    break;
                case right:
                    next.x++;
                    break;
                }

                next.stepCount = next.lastStep != none ? current.stepCount + 1 : current.stepCount;

                return next;
            }

            state trace() const
            {
                auto current = initialState();
                while (current.lastStep != none)
                {
                    current = advance(current);
                }

                return current;
            }
        };

    public:
        TEST_METHOD(Day19_1_Test1)
        {
            auto lines = ReadAllLines("C:\\Day19-sample.txt");
            board theBoard(lines);
            Assert::AreEqual('|', theBoard.pieceAt(5, 0), L"normal read");
            Assert::AreEqual('A', theBoard.pieceAt(5, 2), L"normal read");
            Assert::AreEqual('+', theBoard.pieceAt(14, 3), L"normal read");
            Assert::AreEqual('-', theBoard.pieceAt(7, 5), L"normal read");
            Assert::AreEqual(' ', theBoard.pieceAt(12, 1), L"normal read");
            Assert::AreEqual(' ', theBoard.pieceAt(16, 0), L"off right");
            Assert::AreEqual(' ', theBoard.pieceAt(0, 6), L"off bottom");
            Assert::AreEqual(' ', theBoard.pieceAt(-1, 0), L"off left");
            Assert::AreEqual(' ', theBoard.pieceAt(0, -1), L"off top");
        }

        TEST_METHOD(Day19_1_Test2)
        {
            auto lines = ReadAllLines("C:\\Day19-sample.txt");
            board theBoard(lines);
            auto initial = theBoard.initialState();

            Assert::AreEqual(5, initial.x, L"x");
            Assert::AreEqual(0, initial.y, L"y");
            Assert::AreEqual((int)down, (int)initial.lastStep, L"step");
            Assert::IsFalse(initial.trail == nullptr, L"trail");
            Assert::AreEqual(""s, *initial.trail);
            Assert::AreEqual(1, initial.stepCount, L"steps");
        }

        TEST_METHOD(Day19_1_Test3)
        {
            auto lines = ReadAllLines("C:\\Day19-sample.txt");
            board theBoard(lines);
            auto solution = theBoard.trace();

            Assert::AreEqual(1, solution.x, L"x");
            Assert::AreEqual(3, solution.y, L"y");
            Assert::AreEqual((int)none, (int)solution.lastStep, L"step");
            Assert::IsFalse(solution.trail == nullptr, L"trail");
            Assert::AreEqual("ABCDEF"s, *solution.trail);
            Assert::AreEqual(38, solution.stepCount, L"steps");
        }

        TEST_METHOD(Day19_1_Final)
        {
            auto lines = ReadAllLines("C:\\Day19.txt");
            board theBoard(lines);
            auto solution = theBoard.trace();

            Assert::AreEqual(199, solution.x, L"x");
            Assert::AreEqual(179, solution.y, L"y");
            Assert::AreEqual((int)none, (int)solution.lastStep, L"step");
            Assert::IsFalse(solution.trail == nullptr, L"trail");
            Assert::AreEqual("HATBMQJYZ"s, *solution.trail);
            Assert::AreEqual(16332, solution.stepCount, L"steps");
        }
    };
}