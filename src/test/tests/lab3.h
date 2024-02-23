#ifndef __LAB3_H__
#define __LAB3_H__

#define API_VER 2
#include "framework.h"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <iomanip>

#define INSTRUCTION_LIMIT 5000000
#define MAX_SIZE 0x100

void Test(std::vector<uint16_t> array, uint16_t terminator, lc3::sim &sim, Tester &tester, double total_points)
{
    for (auto num : array)
        std::cout << std::hex << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << num << " ";
    std::cout << '\n' << std::dec;

    for (uint16_t i = 0; i < array.size(); i++)
        sim.writeMem(0x4000 + i, array[i]);
    sim.writeMem(0x4000 + array.size(), terminator & 0xFF);

    sim.run();

    std::vector<uint16_t> student_array;
    for (uint16_t i = 0; i < array.size(); i++)
        student_array.push_back(sim.readMem(0x4000 + i));
    
    std::unordered_set<uint16_t> numbers;
    for (auto num : array)
        numbers.insert(num);
    for (auto num : student_array)
        numbers.erase(num);
    
    tester.verify("Correct array?", numbers.size() == 0, total_points * 0.15);
    tester.verify("Sorted?", numbers.size() == 0 && std::is_sorted(student_array.begin(), student_array.end(), [](uint16_t a, uint16_t b) { return (a & 0xFF) < (b & 0xFF); }), total_points * 0.75);
    tester.verify("Termination", (sim.readMem(0x4000 + array.size()) & 0xFF00) == 0, total_points * 0.1);
}

void testBringup(lc3::sim &sim)
{
    sim.writePC(0x3000);
    sim.setRunInstLimit(INSTRUCTION_LIMIT);
}

void testTeardown(lc3::sim &sim)
{
    std::cout << "Done in " << sim.getInstExecCount() << " instructions.\n";
    if (sim.didExceedInstLimit())
        std::cout << "Test exceeded instruction limit of " << INSTRUCTION_LIMIT << " instructions.\n";
}

void shutdown(void) {}

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

void gen_random(uint16_t num_tests, std::mt19937 &mt, Tester &tester)
{
    for (uint16_t i = 0; i < num_tests; i++)
    {
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);
        uint16_t terminator = mt() % MAX_SIZE;
        tester.registerTest("Test random", [array, terminator](lc3::sim &sim, Tester &tester, double total_points) {
            Test(array, terminator, sim, tester, total_points);
        }, 1, true);
    }
}

#endif