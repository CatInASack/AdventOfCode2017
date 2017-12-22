#include "stdafx.h"
#include "CppUnitTest.h"
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <deque>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace AdventOfCode2017
{
    TEST_CLASS(Day18)
    {
    private:
        static ostringstream log;

        enum referenceType
        {
            registerName,
            literalValue,
            empty,
        };

        struct reference
        {
            union
            {
                char registerName;
                int literalValue;
            };

            referenceType type;

            int64_t resolve(const map<char, int64_t>& registers) const
            {
                auto value = 0i64;
                switch (type)
                {
                case referenceType::literalValue:
                    value = literalValue;
                    break;
                case referenceType::registerName:
                    {
                        auto iter = registers.find(registerName);
                        if (iter != registers.end())
                        {
                            value = iter->second;
                        }
                    }
                    break;
                }

                return value;
            }

            void printLog(const map<char, int64_t>& registers) const
            {
                switch (type)
                {
                case referenceType::registerName:
                    log << ' ' << registerName << '(' << resolve(registers) << ')';
                    break;
                case referenceType::literalValue:
                    log << ' ' << literalValue;
                    break;
                case referenceType::empty:
                    log << " -";
                    break;
                }
            }

            static reference makeReference(const string& token)
            {
                reference ref;
                if (token.empty())
                {
                    ref.type = referenceType::empty;
                }
                else if (islower(token.at(0)))
                {
                    ref.type = referenceType::registerName;
                    ref.registerName = token.at(0);
                }
                else
                {
                    ref.type = referenceType::literalValue;
                    ref.literalValue = stoi(token);
                }

                return ref;
            }
        };


        enum opCode
        {
            snd,
            set,
            add,
            mul,
            mod,
            rcv,
            jgz,
        };

        static map<string, opCode> opCodes;

        class instruction
        {
        private:
            opCode operation;
            reference operandA;
            reference operandB;

            instruction(opCode code, const reference& a, const reference& b) :
                operation(code),
                operandA(a),
                operandB(b)
            {
            }

        public:
            static instruction parseInstruction(const string& input)
            {
                istringstream stream(input);

                string opCode;
                string operandA;
                string operandB;

                stream >> skipws >> opCode >> operandA >> operandB;

                return instruction(opCodes[opCode], reference::makeReference(operandA), reference::makeReference(operandB));
            }

            opCode getOpCode() const { return operation; }

            const reference& getOperandA() const { return operandA; }

            const reference& getOperandB() const { return operandB; }
        };

        class core
        {
        protected:
            vector<instruction> program;
            map<char, int64_t> registers;
            bool halted;

        public:
            static const char pcRegName = '@';
            static const char rcvRegName = '^';
            static const char sndRegName = '$';
            static const char sentCountRegName = '~';

            core(const vector<instruction>& p) : program(p), halted(false)
            {
                registers[pcRegName] = 0;
            }

            virtual int64_t executeInstruction(const instruction& instr)
            {
                auto operation = instr.getOpCode();
                auto& operandA = instr.getOperandA();
                auto& operandB = instr.getOperandB();

                log << find_if(opCodes.begin(), opCodes.end(), [operation](const pair<string, opCode>& entry) { return entry.second == operation; })->first;
                operandA.printLog(registers);
                operandB.printLog(registers);

                auto pcOffset = 1i64;

                switch (operation)
                {
                case set:
                    registers[operandA.registerName] = operandB.resolve(registers);
                    break;
                case add:
                    registers[operandA.registerName] += operandB.resolve(registers);
                    break;
                case mul:
                    registers[operandA.registerName] *= operandB.resolve(registers);
                    break;
                case mod:
                    registers[operandA.registerName] %= operandB.resolve(registers);
                    break;
                case snd:
                    registers[sndRegName] = operandA.resolve(registers);
                    break;
                case rcv:
                    if (operandA.resolve(registers) != 0)
                    {
                        registers[rcvRegName] = registers[sndRegName];
                        halt();
                    }
                    break;
                case jgz:
                    if (operandA.resolve(registers) > 0)
                    {
                        pcOffset = operandB.resolve(registers);
                    }
                    break;
                }

                log << "\t|| " << pcOffset << "\t||";
                for_each(
                    registers.begin(),
                    registers.end(),
                    [](const pair<char, int64_t>& reg)
                {
                    log << " [" << reg.first << "=" << reg.second << "]";
                }
                );
                log << endl;

                return pcOffset;
            }

            void executeInstruction()
            {
                registers[pcRegName] += executeInstruction(program[static_cast<size_t>(registers[pcRegName])]);
            }

            void halt()
            {
                halted = true;
            }

            bool isHalted() const
            {
                return halted ||
                    registers.at(pcRegName) < 0 ||
                    registers.at(pcRegName) >= static_cast<int64_t>(program.size());
            }

            map<char, int64_t> copyRegisters() const
            {
                return registers;
            }

            map<char, int64_t> runProgram()
            {
                while (!isHalted())
                {
                    executeInstruction();
                }

                return copyRegisters();
            }
        };

        class bus
        {
        private:
            deque<int64_t> queue[2];
            int starved;

        public:
            bus() : starved(0) { }

            void unstarve()
            {
                starved = 0;
            }

            int getStarved() const { return starved; }

            void send(int source, int64_t value)
            {
                queue[(source + 1) % 2].push_back(value);
            }

            pair<bool, int64_t> receive(int destination)
            {
                if (queue[destination].empty())
                {
                    starved++;
                    return make_pair(false, 0);
                }
                else
                {
                    auto value = queue[destination].front();
                    queue[destination].pop_front();
                    return make_pair(true, value);
                }
            }
        };

        class multiCore : public core
        {
        private:
            bus& myBus;
            int myId;
        public:
            multiCore(int id, bus& theBus, const vector<instruction>& program) : core(program), myId(id), myBus(theBus)
            {
                registers['p'] = myId;
            }

            virtual int64_t executeInstruction(const instruction& instr) override
            {
                auto pcOffset = 1i64;
                auto operation = instr.getOpCode();

                switch (operation)
                {
                case snd:
                    registers[sentCountRegName]++;
                    myBus.send(myId, instr.getOperandA().resolve(registers));
                    break;
                case rcv:
                    {
                        auto result = myBus.receive(myId);
                        if (result.first)
                        {
                            registers[instr.getOperandA().registerName] = result.second;
                        }
                        else
                        {
                            pcOffset = 0;
                        }
                    }
                    break;
                default:
                    pcOffset = core::executeInstruction(instr);
                    break;
                }

                return pcOffset;
            }
        };

        vector<map<char, int64_t>> runMulticore(const vector<instruction>& program)
        {
            bus theBus;
            vector<multiCore>cores =
            {
                multiCore(0, theBus, program),
                multiCore(1, theBus, program),
            };

            while (any_of(cores.begin(), cores.end(), [](const core& aCore) { return !aCore.isHalted(); }))
            {
                theBus.unstarve();

                for_each(cores.begin(), cores.end(), [](core& aCore) { aCore.executeInstruction(); });

                if (theBus.getStarved() == cores.size())
                {
                    for_each(cores.begin(), cores.end(), [](core& aCore) { aCore.halt(); });
                }
            }

            vector<map<char, int64_t>> result;
            for_each(cores.begin(), cores.end(), [&result](const core& aCore) { result.push_back(aCore.copyRegisters()); });

            return result;
        }

        static vector<instruction> makeProgram(const vector<string>& input)
        {
            vector<instruction> instructions;
            for_each(input.begin(), input.end(), [&instructions](const string& instr) { instructions.push_back(instruction::parseInstruction(instr)); });
            return instructions;
        }

        static vector<string> exampleInput;
        static vector<string> exampleInput2;
        static vector<string> finalInput;

    public:
        TEST_METHOD(Day18_1_Test1)
        {
            auto instr = instruction::parseInstruction("set a 1"s);
            Assert::AreEqual((int)set, (int)instr.getOpCode(), L"op code 1");
            Assert::AreEqual((int)registerName, (int)instr.getOperandA().type, L"val type a1");
            Assert::AreEqual('a', instr.getOperandA().registerName, L"value a1");
            Assert::AreEqual((int)literalValue, (int)instr.getOperandB().type, L"val type b1");
            Assert::AreEqual(1, instr.getOperandB().literalValue, L"value b2");

            instr = instruction::parseInstruction("snd z"s);
            Assert::AreEqual((int)snd, (int)instr.getOpCode(), L"op code 2");
            Assert::AreEqual((int)registerName, (int)instr.getOperandA().type, L"val type a2");
            Assert::AreEqual('z', instr.getOperandA().registerName, L"value a2");
            Assert::AreEqual((int)empty, (int)instr.getOperandB().type, L"val type b2");

            instr = instruction::parseInstruction("jgz g -12345"s);
            Assert::AreEqual((int)jgz, (int)instr.getOpCode(), L"op code 3");
            Assert::AreEqual((int)registerName, (int)instr.getOperandA().type, L"val type a3");
            Assert::AreEqual('g', instr.getOperandA().registerName, L"value a3");
            Assert::AreEqual((int)literalValue, (int)instr.getOperandB().type, L"val type b3");
            Assert::AreEqual(-12345, instr.getOperandB().literalValue, L"value b3");

            instr = instruction::parseInstruction("mul a b"s);
            Assert::AreEqual((int)mul, (int)instr.getOpCode(), L"op code 4");
            Assert::AreEqual((int)registerName, (int)instr.getOperandA().type, L"val type a4");
            Assert::AreEqual('a', instr.getOperandA().registerName, L"value a4");
            Assert::AreEqual((int)registerName, (int)instr.getOperandB().type, L"val type b4");
            Assert::AreEqual('b', instr.getOperandB().registerName, L"value b4");
        }

        TEST_METHOD(Day18_1_Test2)
        {
            auto registers = core(makeProgram(exampleInput)).runProgram();
            Assert::AreEqual(4, static_cast<int>(registers[core::rcvRegName]));
        }

        TEST_METHOD(Day18_1_Final)
        {
            auto registers = core(makeProgram(finalInput)).runProgram();
            auto logContents = log.str();
            Assert::AreEqual(8600, static_cast<int>(registers[core::rcvRegName]));
        }

        TEST_METHOD(Day18_2_Test1)
        {
            auto result = runMulticore(makeProgram(exampleInput2));

            Assert::AreEqual(2u, result.size(), L"Result size");

            Assert::AreEqual(6u, result[0].size(), L"Registers count 0");
            Assert::AreEqual(6, static_cast<int>(result[0][core::pcRegName]), L"PC 0");
            Assert::AreEqual(3, static_cast<int>(result[0][core::sentCountRegName]), L"Send count 0");
            Assert::AreEqual(0, static_cast<int>(result[0]['p']), L"Regs[p] 0");
            Assert::AreEqual(1, static_cast<int>(result[0]['a']), L"Regs[a] 0");
            Assert::AreEqual(2, static_cast<int>(result[0]['b']), L"Regs[b] 0");
            Assert::AreEqual(1, static_cast<int>(result[0]['c']), L"Regs[c] 0");

            Assert::AreEqual(6u, result[1].size(), L"Registers count 1");
            Assert::AreEqual(6, static_cast<int>(result[1][core::pcRegName]), L"PC 1");
            Assert::AreEqual(3, static_cast<int>(result[1][core::sentCountRegName]), L"Send count 1");
            Assert::AreEqual(1, static_cast<int>(result[1]['p']), L"Regs[p] 1");
            Assert::AreEqual(1, static_cast<int>(result[1]['a']), L"Regs[a] 1");
            Assert::AreEqual(2, static_cast<int>(result[1]['b']), L"Regs[b] 1");
            Assert::AreEqual(0, static_cast<int>(result[1]['c']), L"Regs[c] 1");
        }

        TEST_METHOD(Day18_2_Final)
        {
            auto result = runMulticore(makeProgram(finalInput));
            Assert::AreEqual(7239, static_cast<int>(result[1][core::sentCountRegName]));
        }
    };

    ostringstream Day18::log;

    vector<string> Day18::exampleInput =
    {
        "set a 1"s,
        "add a 2"s,
        "mul a a"s,
        "mod a 5"s,
        "snd a"s,
        "set a 0"s,
        "rcv a"s,
        "jgz a -1"s,
        "set a 1"s,
        "jgz a -2"s,
    };

    vector<string> Day18::exampleInput2 =
    {
        "snd 1"s,
        "snd 2"s,
        "snd p"s,
        "rcv a"s,
        "rcv b"s,
        "rcv c"s,
        "rcv d"s,
    };

    vector<string> Day18::finalInput =
    {
        "set i 31"s,
        "set a 1"s,
        "mul p 17"s,
        "jgz p p"s,
        "mul a 2"s,
        "add i -1"s,
        "jgz i -2"s,
        "add a -1"s,
        "set i 127"s,
        "set p 735"s,
        "mul p 8505"s,
        "mod p a"s,
        "mul p 129749"s,
        "add p 12345"s,
        "mod p a"s,
        "set b p"s,
        "mod b 10000"s,
        "snd b"s,
        "add i -1"s,
        "jgz i -9"s,
        "jgz a 3"s,
        "rcv b"s,
        "jgz b -1"s,
        "set f 0"s,
        "set i 126"s,
        "rcv a"s,
        "rcv b"s,
        "set p a"s,
        "mul p -1"s,
        "add p b"s,
        "jgz p 4"s,
        "snd a"s,
        "set a b"s,
        "jgz 1 3"s,
        "snd b"s,
        "set f 1"s,
        "add i -1"s,
        "jgz i -11"s,
        "snd a"s,
        "jgz f -16"s,
        "jgz a -19"s,
    };

    map<string, Day18::opCode> Day18::opCodes =
    {
        { "snd"s, Day18::opCode::snd },
        { "set"s, Day18::opCode::set },
        { "add"s, Day18::opCode::add },
        { "mul"s, Day18::opCode::mul },
        { "mod"s, Day18::opCode::mod },
        { "rcv"s, Day18::opCode::rcv },
        { "jgz"s, Day18::opCode::jgz },
    };
}