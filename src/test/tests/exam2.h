#ifndef __LAB1_H__
#define __LAB1_H__

#define API_VER 2
#include "framework.h"
#include <random>
#include <iomanip>

#define INSTRUCTION_LIMIT 5000000
#define ORIGIN 0x3000
#define ARRAY_ORIGIN 0x4002
#define MAX_SIZE 0x0064  //100
#define MAX_SUM 255

uint8_t verify_sum(lc3::sim &sim, uint8_t unsigned_sum, Tester &tester)
{
    std::stringstream stream;

    uint16_t student_unsigned_sum = sim.readMem(0x4001);

    /*
    tester.output("At x6002, expected " + std::to_string(unsigned_sum));
    tester.output("Got " + std::to_string(student_unsigned_sum));

    stream << "At x6002, expected " << std::to_string(unsigned_sum);
    stream << "Got " << std::to_string(student_unsigned_sum);
    */

    return unsigned_sum == student_unsigned_sum;
}

void TestEmpty(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x4000, 0x0000);

    uint16_t unsigned_sum = 0x0000;

    sim.run();

    tester.verify("TestEmpty", verify_sum(sim, unsigned_sum, tester), total_points);
}

void TestZeros(lc3::sim &sim, Tester &tester, double total_points)
{
    // Full size array of 1's should give 100
    sim.writeMem(0x4000, 0x0000);

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;
        sim.writeMem(addressed, 0x0000);
    }

    uint16_t unsigned_sum = 0x0000;

    sim.run();

    tester.verify("TestZeros", verify_sum(sim, unsigned_sum, tester), total_points);
}

void TestOnes(lc3::sim &sim, Tester &tester, double total_points)
{
    // Full size array of 1's should give 100
    sim.writeMem(0x4000, MAX_SIZE);

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;
        sim.writeMem(addressed, 0x0001);
    }

    uint16_t unsigned_sum = MAX_SIZE;

    sim.run();

    tester.verify("TestOnes", verify_sum(sim, unsigned_sum, tester), total_points);
}




void Test(uint16_t a, uint16_t b, double frac, lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, a);
    sim.writeMem(0x6001, b);

    sim.run();


	// .verify ("X",...) -> "X" prints to gradescope console (prints to file which is output to gradescope)
    //uint8_t verification = verify_sum(sim, a, b, tester);

    /*
    std::stringstream stream;
    stream << "x6000: " << a;
    stream << "x6001: " << b;

    std::string a_string = std::to_string(a);
    std::string b_string = std::to_string(b);
    */

    //tester.verify("Test x4001", verification, total_points * frac);
}

void testBringup(lc3::sim &sim)
{
    sim.writePC(ORIGIN);
    sim.setRunInstLimit(INSTRUCTION_LIMIT);
}

void testTeardown(lc3::sim &sim) {}

void shutdown(void) {}

void exam2_setup_private(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    //if (isPublic)
    //    tester.registerTest("ExampleTest", ExampleTest, 1, true);
    //tester.registerTest("TestFull",TestFull,1,true);
    //tester.registerTest("TestEmpty",TestEmpty,1,true);
    //tester.registerTest("TestClearingA",TestClearingA,2,true);
    //tester.registerTest("TestClearingB",TestClearingB,2,true);
    //tester.registerTest("TestUnsignedMischief",TestUnsignedMischief,4,true);

    /*
    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t A = mt() % 0x10000, B = mt() % 0x10000;
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        auto test = [&stream, &tester](std::string test_name, uint16_t a, uint16_t b, double weight)
        {
            //stream.str("");
            //stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << a;
            //stream << "_";
            //stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << b;

            double frac = 0.5;
            auto test = [a, b, frac](lc3::sim &sim, Tester &tester, double total_points)
            { Test(a, b, frac, sim, tester, total_points); };
            tester.registerTest(test_name, test, weight, true);
        };

        test("TestUnsignedRandom", A & 0x00FF, B & 0x00FF, 0.75);
        test("TestSignedRandom", A & 0xFF00, B & 0xFF00, 0.75);
        test("TestBothRandom", A, B, 1.5);
    }
    */
}

void exam2_setup_public(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    tester.registerTest("All Ones", TestOnes, 1, true);
    tester.registerTest("Empty", TestEmpty, 1, true);
    tester.registerTest("All Zeros", TestZeros, 1, true);
    //tester.registerTest("Random", TestRandom, 1, true);

    /*
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
    */
}

#endif