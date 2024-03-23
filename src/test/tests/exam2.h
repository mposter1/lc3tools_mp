#ifndef __LAB1_H__
#define __LAB1_H__

#define API_VER 2
#include "framework.h"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <iomanip>

#define INSTRUCTION_LIMIT 5000000
#define ORIGIN 0x3000
#define ARRAY_ORIGIN 0x4002
#define MAX_SIZE 0x0064  //100
#define MAX_SUM 0xFFFF

uint8_t verify_sum(lc3::sim &sim, uint16_t unsigned_sum, Tester &tester)
{
    std::stringstream stream;

    uint16_t student_unsigned_sum = sim.readMem(0x4001);


    tester.output("Expected " + std::to_string(unsigned_sum));
    tester.output("Got " + std::to_string(student_unsigned_sum));

    stream << "Expected " << std::to_string(unsigned_sum);
    stream << "Got " << std::to_string(student_unsigned_sum);


    return unsigned_sum == student_unsigned_sum;
}

void TestEmpty(lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x4000, 0x0000);

    uint16_t unsigned_sum = 0xFFFF;

    sim.run();

    tester.verify("Test Sum @ x4001", verify_sum(sim, unsigned_sum, tester), total_points);
}

void TestZeros(lc3::sim &sim, Tester &tester, double total_points)
{
    // Full size array of 1's should give 100
    sim.writeMem(0x4000, MAX_SIZE);

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;
        sim.writeMem(addressed, 0x0000);
    }

    uint16_t unsigned_sum = 0x0000;

    sim.run();

    tester.verify("Test Sum @ x4001", verify_sum(sim, unsigned_sum, tester), total_points);
}

void TestOnes(lc3::sim &sim, Tester &tester, double total_points)
{

    // Full size array of 1's should give 100
    sim.writeMem(0x4000, MAX_SIZE);

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;

        //std::cout << std::hex << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << addressed << " ";

        sim.writeMem(addressed, 0x0001);
    }

    uint16_t unsigned_sum = MAX_SIZE;

    sim.run();

    tester.verify("Test Sum @ x4001", verify_sum(sim, unsigned_sum, tester), total_points);
}

std::vector<uint16_t> gen_array(uint16_t size, std::mt19937 &mt)
{
    std::vector<uint16_t> array(MAX_SIZE - 1);
    std::iota(array.begin(), array.end(), 1);
    for (auto &num : array)
        num = (num << 8) | (mt() % MAX_SIZE);
    std::shuffle(array.begin(), array.end(), mt);
    array.resize(size);
    return array;
}



void TestRandom(lc3::sim &sim, Tester &tester, double total_points, std::mt19937 mt)
{

    // Create an array to store random numbers
    std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);

    for (auto num : array)
        std::cout << std::hex << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << num << " ";
    std::cout << '\n' << std::dec;

    // Full size array of 1's should give 100
    sim.writeMem(0x4000, array.size());

    uint16_t unsigned_sum = 0;

    for (uint16_t i = 0; i < array.size(); i++) {
        unsigned_sum = unsigned_sum + (array[i] & 0xFF);
        sim.writeMem(ARRAY_ORIGIN + i, array[i]);
    }

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

    tester.verify("Test Sum @ x4001", verify_sum(sim, unsigned_sum, tester), total_points);
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


    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        tester.registerTest("All Ones", TestOnes, 2.5, true);
        tester.registerTest("Empty", TestEmpty, 2.5, true);
        tester.registerTest("All Zeros", TestZeros, 2.5, true);
        tester.registerTest("Random", [mt](lc3::sim &sim, Tester &tester, double total_points){
            TestRandom(sim,tester,total_points,mt);}
            , 2.5, true);

    }

}

void exam2_setup_public(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        //tester.registerTest("All Ones", TestOnes, 2.5, true);
        //tester.registerTest("Empty", TestEmpty, 2.5, true);
        //tester.registerTest("All Zeros", TestZeros, 2.5, true);
        tester.registerTest("Random", [mt](lc3::sim &sim, Tester &tester, double total_points){
                                TestRandom(sim,tester,total_points,mt);}
                , 2.5, true);

    }
}

#endif