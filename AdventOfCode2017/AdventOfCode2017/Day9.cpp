#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utilities.h"
#include <stack>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017::Day9
{
    TEST_CLASS(Day9)
    {
    private:
        class dataStream;
        class state;
        class neutralState;
        class groupState;
        class ignoreState;
        class garbageState;

        class dataStream
        {
            friend class state;
        public:
            static pair<int, int> process(const string& input)
            {
                dataStream d;

                for (auto iter = input.begin(); iter != input.end(); iter++)
                {
                    d.process(*iter);
                }

                Assert::AreEqual(size_t(1), d.history.size());

                return make_pair(d.getScore(), d.getGarbage());
            }

            dataStream() : score(0), garbage(0)
            {
                history.push(new neutralState(*this));
            }

            ~dataStream()
            {
                while (!history.empty())
                {
                    auto state = history.top();
                    history.pop();
                    delete state;
                }
            }

            void process(char c)
            {
                history.top()->process(c);
            }

            int getScore() const { return score; }

            int getGarbage() const { return garbage; }

        private:
            stack<state*> history;
            int score;
            int garbage;
        };

        class state
        {
        public:
            virtual void process(char c) const
            {
                switch (c)
                {
                case '{':
                    goToState(new groupState(myStream));
                    break;
                case '<':
                    goToState(new garbageState(myStream));
                    break;
                case '!':
                    goToState(new ignoreState(myStream));
                    break;
                }
            }
        protected:
            state(dataStream& stream) : myStream(stream) { }

            void goToState(state* state) const
            {
                myStream.history.push(state);
            }

            void endState() const
            {
                auto ended = myStream.history.top();
                myStream.history.pop();
                delete ended;
            }

            void addScore() const
            {
                myStream.score += myStream.history.size() - 1;
            }

            void addGarbage() const
            {
                myStream.garbage++;
            }

            dataStream& myStream;
        };

        class neutralState : public state
        {
        public:
            neutralState(dataStream& stream) : state(stream) { }
        };

        class groupState : public state
        {
        public:
            groupState(dataStream& stream) : state(stream) { }

            virtual void process(char c) const override
            {
                switch (c)
                {
                case '}':
                    addScore();
                    endState();
                    break;
                default:
                    state::process(c);
                    break;
                }
            };
        };

        class ignoreState : public state
        {
        public:
            ignoreState(dataStream& stream) : state(stream) { }

            virtual void process(char c) const override
            {
                endState();
            };
        };

        class garbageState : public state
        {
        public:
            garbageState(dataStream& stream) : state(stream) { }

            virtual void process(char c) const override
            {
                switch (c)
                {
                case '>':
                    endState();
                    break;
                case '!':
                    goToState(new ignoreState(myStream));
                    break;
                default:
                    addGarbage();
                    break;
                }
            };
        };

    public:
        TEST_METHOD(Day9_1_Test1)
        {
            Assert::AreEqual(1, dataStream::process("{}"s).first);
        }

        TEST_METHOD(Day9_1_Test2)
        {
            Assert::AreEqual(6, dataStream::process("{{{}}}"s).first);
        }

        TEST_METHOD(Day9_1_Test3)
        {
            Assert::AreEqual(5, dataStream::process("{{},{}}"s).first);
        }

        TEST_METHOD(Day9_1_Test4)
        {
            Assert::AreEqual(16, dataStream::process("{{{},{},{{}}}}"s).first);
        }

        TEST_METHOD(Day9_1_Test5)
        {
            Assert::AreEqual(1, dataStream::process("{<a>,<a>,<a>,<a>}"s).first);
        }

        TEST_METHOD(Day9_1_Test6)
        {
            Assert::AreEqual(9, dataStream::process("{{<ab>},{<ab>},{<ab>},{<ab>}}"s).first);
        }

        TEST_METHOD(Day9_1_Test7)
        {
            Assert::AreEqual(9, dataStream::process("{{<!!>},{<!!>},{<!!>},{<!!>}}"s).first);
        }

        TEST_METHOD(Day9_1_Test8)
        {
            Assert::AreEqual(3, dataStream::process("{{<a!>},{<a!>},{<a!>},{<ab>}}"s).first);
        }

        TEST_METHOD(Day9_2_Test1)
        {
            Assert::AreEqual(0, dataStream::process("<>"s).second);
        }

        TEST_METHOD(Day9_2_Test2)
        {
            Assert::AreEqual(17, dataStream::process("<random characters>"s).second);
        }

        TEST_METHOD(Day9_2_Test3)
        {
            Assert::AreEqual(3, dataStream::process("<<<<>"s).second);
        }

        TEST_METHOD(Day9_2_Test4)
        {
            Assert::AreEqual(2, dataStream::process("<{!>}>"s).second);
        }

        TEST_METHOD(Day9_1_2_Final)
        {
            auto input = ReadFile("C:\\Day9.txt");
            auto result = dataStream::process(input);
            Assert::AreEqual(7616, result.first);
            Assert::AreEqual(3838, result.second);
        }
    };
}