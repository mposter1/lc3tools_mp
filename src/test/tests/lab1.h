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

    /*
    tester.output("At x6002, expected " + std::to_string(unsigned_sum));
    tester.output("Got " + std::to_string(student_unsigned_sum));

    stream << "At x6002, expected " << std::to_string(unsigned_sum);
    stream << "Got " << std::to_string(student_unsigned_sum);
    */

    ret |= 0b01 * (unsigned_sum == student_unsigned_sum);

    uint16_t signed_sum = ((a >> 8) + (b >> 8)) << 8, student_signed_sum = sim.readMem(0x6003);
    if ((a & 0x8000) == (b & 0x8000) && (a & 0x8000) != (signed_sum & 0x8000))
        signed_sum = 0xFFFF;

    /*
    tester.output("At x6003, expected " + std::to_string(signed_sum));
    tester.output("Got " + std::to_string(student_signed_sum));

    stream << "At x6003, expected " << std::to_string(signed_sum);
    stream << "Got " << std::to_string(student_signed_sum);
    */

    ret |= 0b10 * (signed_sum == student_signed_sum);

    return ret;
}

void ExampleTest_1(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0x6FA2);
    sim.writeMem(0x6001, 0xB0B7);

    sim.run();

    tester.verify("Example 1", (verify_sum(sim, 0x6FA2, 0xB0B7, tester) & 0b11) == 0b11, total_points);
}

void ExampleTest_2(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0xAF62);
    sim.writeMem(0x6001, 0xB0B7);

    sim.run();

    tester.verify("Example 2", (verify_sum(sim, 0xAF62, 0xB0B7, tester) & 0b11) == 0b11, total_points);
}

void TestFull(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0xFFFF);
    sim.writeMem(0x6001, 0xFFFF);

    sim.run();

    tester.verify("TestFull", (verify_sum(sim, 0xFFFF, 0xFFFF, tester) & 0b11) == 0b11, total_points);
}

void TestEmpty(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0x0000);
    sim.writeMem(0x6001, 0x0000);

    sim.run();

    tester.verify("TestEmpty", (verify_sum(sim, 0x0000, 0x0000, tester) & 0b11) == 0b11, total_points);
}

void TestClearing1(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0x01FF);
    sim.writeMem(0x6001, 0x00FF);

    sim.run();

    tester.verify("TestClearing1", (verify_sum(sim, 0x01FF, 0x00FF, tester) & 0b11) == 0b11, total_points);
}

void TestClearing2(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0xF8F0);
    sim.writeMem(0x6001, 0xF7F0);

    sim.run();

    tester.verify("TestClearing2", (verify_sum(sim, 0xF8F0, 0xF7F0, tester) & 0b11) == 0b11, total_points);
}

void TestUnsignedMischief(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, 0xFF80);
    sim.writeMem(0x6001, 0x0180);

    sim.run();

    tester.verify("TestUnsignedMischief", (verify_sum(sim, 0xFF80, 0x0180, tester) & 0b11) == 0b11, total_points);
}

void Test(uint16_t a, uint16_t b, double frac, lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, a);
    sim.writeMem(0x6001, b);

    sim.run();


	// .verify ("X",...) -> "X" prints to gradescope console (prints to file which is output to gradescope)
    uint8_t mask = verify_sum(sim, a, b, tester);

    /*
    std::stringstream stream;
    stream << "x6000: " << a;
    stream << "x6001: " << b;

    std::string a_string = std::to_string(a);
    std::string b_string = std::to_string(b);
    */

    tester.verify("Test x6002", mask & 0b01, total_points * frac);
    tester.verify("Test x6003", mask & 0b10, total_points * (1 - frac));
}

void testBringup(lc3::sim &sim)
{
    sim.writePC(0x3000);
    sim.setRunInstLimit(1000);
}

void testTeardown(lc3::sim &sim) {}

void shutdown(void) {}

void lab1_setup_private(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    //if (isPublic)
    //    tester.registerTest("ExampleTest", ExampleTest, 1, true);
    //tester.registerTest("TestFull",TestFull,1,true)
    //tester.registerTest("TestEmpty",TestEmpty,1,true)
    //tester.registerTest("TestClearing1",TestClearing1,2,true)
    //tester.registerTest("TestClearing2",TestClearing2,2,true)
    //tester.registerTest("TestUnsignedMischief",TestUnsignedMischief,4,true)

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t A = mt() % 0x10000, B = mt() % 0x10000;
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        auto test = [&stream, &tester](std::string test_name, uint16_t a, uint16_t b, double weight)
        {
            stream.str("");
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << a;
            stream << "_";
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << b;

            double frac = 0.5;
            auto test = [a, b, frac](lc3::sim &sim, Tester &tester, double total_points)
            { Test(a, b, frac, sim, tester, total_points); };
            tester.registerTest(test_name + "_" + stream.str(), test, weight, true);
        };

        test("TestUnsigned", A & 0x00FF, B & 0x00FF, 0.75);
        test("TestSigned", A & 0xFF00, B & 0xFF00, 0.75);
        test("TestBoth", A, B, 1.5);
    }
}

void lab1_setup_public(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    tester.registerTest("ExampleTest 1 - Randomized", ExampleTest_1, 3.5, true);
    tester.registerTest("ExampleTest 2 - Randomized", ExampleTest_2, 3.5, true);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t A = mt() % 0x10000, B = mt() % 0x10000;
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        auto test = [&stream, &tester](std::string test_name, uint16_t a, uint16_t b, double weight)
        {
            stream.str("");
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << a;
            stream << "_";
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << b;

            double frac = 0.5;
            auto test = [a, b, frac](lc3::sim &sim, Tester &tester, double total_points)
            { Test(a, b, frac, sim, tester, total_points); };
            tester.registerTest(test_name + "_" + stream.str(), test, weight, true);
        };

        test("TestUnsigned", A & 0x00FF, B & 0x00FF, 0.75);
        test("TestSigned", A & 0xFF00, B & 0xFF00, 0.75);
        test("TestBoth", A, B, 1.5);
    }
}

#endif