#ifndef __LAB1_H__
#define __LAB1_H__

#define API_VER 2
#include "framework.h"
#include <random>
#include <iomanip>

uint8_t verify_sum(lc3::sim &sim, uint16_t a, uint16_t b, Tester &tester)
{
    std::stringstream stream;
    uint8_t ret = 0b00;

    uint16_t unsigned_sum = (a & 0x00FF) + (b & 0x00FF), student_unsigned_sum = sim.readMem(0x6002);
    if (unsigned_sum & 0x0100)
        unsigned_sum = 0xFFFF;

    tester.output("At x6002, expected " + std::to_string(unsigned_sum));
    tester.output("Got " + std::to_string(student_unsigned_sum));

    stream << "At x6002, expected " << std::to_string(unsigned_sum);
    stream << "Got " << std::to_string(student_unsigned_sum);

    ret |= 0b01 * (unsigned_sum == student_unsigned_sum);

    uint16_t signed_sum = ((a >> 8) + (b >> 8)) << 8, student_signed_sum = sim.readMem(0x6003);
    if ((a & 0x8000) == (b & 0x8000) && (a & 0x8000) != (signed_sum & 0x8000))
        signed_sum = 0xFFFF;

    tester.output("At x6003, expected " + std::to_string(signed_sum));
    tester.output("Got " + std::to_string(student_signed_sum));

    stream << "At x6003, expected " << std::to_string(signed_sum);
    stream << "Got " << std::to_string(student_signed_sum);

    ret |= 0b10 * (signed_sum == student_signed_sum);

    return ret;
}

void ExampleTest(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0x6FA2);
    sim.writeMem(0x6001, 0xB0B7);

    sim.run();

    tester.verify("Example 1", (verify_sum(sim, 0x6FA2, 0xB0B7, tester) & 0b11) == 0b11, total_points);
}

void Test(uint16_t a, uint16_t b, double frac, lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, a);
    sim.writeMem(0x6001, b);

    sim.run();


	// .verify ("X",...) -> "X" prints to gradescope console (prints to file which is output to gradescope)
    uint8_t mask = verify_sum(sim, a, b, tester);

    std::stringstream stream;
    stream << "x6000: " << a;
    stream << "x6001: " << b;

    std::string a_string = std::to_string(a);
    std::string b_string = std::to_string(b);

    tester.verify("Test x6002", mask & 0b01, total_points * frac);
    tester.verify("Test x6003", mask & 0b10, total_points * (1 - frac));
}

void testBringup(lc3::sim &sim)
{
    sim.writePC(0x6000);
    sim.setRunInstLimit(1000);
}

void testTeardown(lc3::sim &sim) {}

void shutdown(void) {}

void lab1_setup(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    if (isPublic)
        tester.registerTest("ExampleTest", ExampleTest, 1, true);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t A = mt() % 0x10000, B = mt() % 0x10000;
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        auto test = [&stream, &tester](std::string test_name, uint16_t a, uint16_t b, double frac)
        {
            stream.str("");
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << a;
            stream << "_";
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << b;

            auto test = [a, b, frac](lc3::sim &sim, Tester &tester, double total_points)
            { Test(a, b, frac, sim, tester, total_points); };
            tester.registerTest(test_name + "_" + stream.str(), test, 1, true);
        };

        test("TestUnsigned", A & 0x00FF, B & 0x00FF, 0.8);
        test("TestSigned", A & 0xFF00, B & 0xFF00, 0.2);
        test("TestBoth", A, B, 0.5);
    }
}

#endif