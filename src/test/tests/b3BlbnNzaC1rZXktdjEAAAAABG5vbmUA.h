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
#define ARRAY_ORIGIN 0x4000
#define MAX_SIZE 0x0064  //100
#define MAX_SUM 0xFFFF

uint8_t verifyLength(lc3::sim &sim, uint16_t length_count, Tester &tester)
{
    std::stringstream stream;

    uint16_t student_length_count = sim.readMem(0x3FFF);

    tester.output("Expected " + std::to_string(length_count));
    tester.output("Got " + std::to_string(student_length_count));

    stream << "Expected " << std::to_string(length_count);
    stream << "Got " << std::to_string(student_length_count);


    return length_count == student_length_count;
}

uint8_t verifyEvenCount(lc3::sim &sim, uint16_t even_count, Tester &tester)
{
    std::stringstream stream;

    uint16_t student_even_count = sim.readMem(0x3FFE);

    tester.output("Expected " + std::to_string(even_count));
    tester.output("Got " + std::to_string(student_even_count));

    stream << "Expected " << std::to_string(even_count);
    stream << "Got " << std::to_string(student_even_count);


    return even_count == student_even_count;
}

void TestEmpty(lc3::sim &sim, Tester &tester, double total_points, bool verify_evens)
{
    sim.writeMem(0x4000, 0x0000);

    uint16_t length = 0x0000;

    sim.run();

    if (verify_evens){
        tester.verify("Test Evens @ x3FFE", verifyEvenCount(sim, 0, tester), total_points);
    }
    else{
        tester.verify("Test Length @ x3FFF", verifyLength(sim, length, tester), total_points);
    }
}

void TestZeros(lc3::sim &sim, Tester &tester, double total_points, bool verify_evens)
{
    // Full size array of 1's should give 100
    sim.writeMem(0x4000, 0x0000);

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;
        sim.writeMem(addressed, 0x0000);
    }

    sim.writeMem(ARRAY_ORIGIN + MAX_SIZE, 0x0000);

    uint16_t unsigned_sum = 0x0000;

    sim.run();

    if (verify_evens){
        tester.verify("Test Evens @ x3FFE", verifyEvenCount(sim, 0, tester), total_points);
    }
    else{
        tester.verify("Test Length @ x3FFF", verifyLength(sim, unsigned_sum, tester), total_points);
    }
}

void TestOnes(lc3::sim &sim, Tester &tester, double total_points, bool verify_evens)
{

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;

        //std::cout << std::hex << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << addressed << " ";

        sim.writeMem(addressed, 0x0001);
    }

    sim.writeMem(ARRAY_ORIGIN + MAX_SIZE, 0x0000);


    uint16_t unsigned_sum = MAX_SIZE;

    sim.run();

    if (verify_evens){
        tester.verify("Test Evens @ x3FFE", verifyEvenCount(sim, 0, tester), total_points);
    }
    else{
        tester.verify("Test Length @ x3FFF", verifyLength(sim, unsigned_sum, tester), total_points);
    }
}

void TestNegOnes(lc3::sim &sim, Tester &tester, double total_points, bool verify_evens)
{

    for(uint16_t i = 0; i < MAX_SIZE; i++){
        uint16_t addressed = ARRAY_ORIGIN + i;

        //std::cout << std::hex << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << addressed << " ";

        sim.writeMem(addressed, 0xFFFF);
    }

    sim.writeMem(ARRAY_ORIGIN + MAX_SIZE, 0x0000);

    uint16_t unsigned_sum = MAX_SIZE;

    sim.run();

    if (verify_evens){
        tester.verify("Test Evens @ x3FFE", verifyEvenCount(sim, 0, tester), total_points);
    }
    else{
        tester.verify("Test Length @ x3FFF", verifyLength(sim, unsigned_sum, tester), total_points);
    }
}

std::vector<uint16_t> gen_array(uint16_t size, std::mt19937 &mt)
{
    std::vector<uint16_t> array(MAX_SIZE - 1);
    std::iota(array.begin(), array.end(), 1);
    for (auto &num : array) {
        num = (num) | (mt() % MAX_SIZE);
        if (num == 0x0000) {
            num++;
        }
    }
    std::shuffle(array.begin(), array.end(), mt);
    array.resize(size);
    return array;
}



void TestRandom(lc3::sim &sim, Tester &tester, double total_points, std::vector<uint16_t> array, bool verify_evens)
{

    for (auto num : array)
        std::cout << std::hex << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << num << " ";
    std::cout << '\n' << std::dec;

    uint16_t unsigned_sum = array.size();

    uint16_t num_evens = 0;

    for (uint16_t i = 0; i < array.size(); i++) {
        sim.writeMem(ARRAY_ORIGIN + i, array[i]);
        if (array[i] % 2 == 0){
            num_evens++;
        }
    }
    sim.writeMem(ARRAY_ORIGIN + array.size(), 0x0000);

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


    if (verify_evens){
        tester.verify("Test Evens @ x3FFE", verifyEvenCount(sim, num_evens, tester), total_points);
    }
    else{
        tester.verify("Test Length @ x3FFF", verifyLength(sim, unsigned_sum, tester), total_points);
    }
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

        // Create an array to store random numbers
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);

        tester.registerTest("All Ones", [](lc3::sim &sim, Tester &tester, double total_points){
            TestOnes(sim,tester,total_points,false);}, 0.025, true);
        tester.registerTest("Empty", [](lc3::sim &sim, Tester &tester, double total_points){
            TestEmpty(sim,tester,total_points,false);}, 0.025, true);
        tester.registerTest("All Zeros", [](lc3::sim &sim, Tester &tester, double total_points){
            TestZeros(sim,tester,total_points,false);}, 0.025, true);
        tester.registerTest("All Negative One", [](lc3::sim &sim, Tester &tester, double total_points){
            TestNegOnes(sim,tester,total_points,false);}, 0.025, true);
        tester.registerTest("Random", [array](lc3::sim &sim, Tester &tester, double total_points){
            TestRandom(sim,tester,total_points,array, false);}
            , 1, true);

    }

}

void exam2_setup_public(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        // Create an array to store random numbers
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);

        //tester.registerTest("All Ones", TestOnes, 2.5, true);
        //tester.registerTest("Empty", TestEmpty, 2.5, true);
        //tester.registerTest("All Zeros", TestZeros, 2.5, true);
        tester.registerTest("Random", [array](lc3::sim &sim, Tester &tester, double total_points){
                                TestRandom(sim,tester,total_points,array, false);}
                , 0.5, true);

    }
}

void exam2_setup_ec(uint16_t num_tests, uint16_t seed, Tester &tester, bool isPublic)
{
    std::mt19937 mt(seed);


    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        std::stringstream stream;
        stream << std::hex << std::uppercase;

        // Create an array to store random numbers
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);

        tester.registerTest("All Ones", [](lc3::sim &sim, Tester &tester, double total_points){
            TestOnes(sim,tester,total_points,true);}, 0.025, true);
        tester.registerTest("Empty", [](lc3::sim &sim, Tester &tester, double total_points){
            TestEmpty(sim,tester,total_points,true);}, 0.025, true);
        tester.registerTest("All Zeros", [](lc3::sim &sim, Tester &tester, double total_points){
            TestZeros(sim,tester,total_points,true);}, 0.025, true);
        tester.registerTest("All Negative One", [](lc3::sim &sim, Tester &tester, double total_points){
            TestNegOnes(sim,tester,total_points,true);}, 0.025, true);
        tester.registerTest("Random", [array](lc3::sim &sim, Tester &tester, double total_points){
                                TestRandom(sim,tester,total_points,array,true);}
                , 0.4, true);

    }

}

#endif