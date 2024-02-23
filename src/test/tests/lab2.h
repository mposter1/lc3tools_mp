#ifndef __LAB2_H__
#define __LAB2_H__

#define API_VER 2
#include "framework.h"
#include <random>
#include <iomanip>

void Test(uint16_t addr1, uint16_t addr2, uint16_t val, lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x4000, addr1);
    sim.writeMem(0x4001, addr2);
    sim.writeMem(addr1, val);

    sim.run();

    uint16_t student_sum = sim.readMem(addr2);
    uint16_t expected_sum = (val & 0xFF) + ((val >> 8) & 0xFF);

    tester.verify("Test", student_sum == expected_sum, total_points);
}

void testBringup(lc3::sim &sim)
{
    sim.writePC(0x3000);
    sim.setRunInstLimit(1000);
}

void testTeardown(lc3::sim &sim) {}

void shutdown(void) {}

void lab2_setup(uint16_t num_tests, uint16_t seed, Tester &tester, bool includeCornerCases)
{
    std::mt19937 mt(seed);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t num = mt() % 0x10000;
        uint16_t addr1 = 0x4002 + (mt() % 0xBDFE);
        uint16_t addr2 = 0x4002 + (mt() % 0xBDFE);
        if (includeCornerCases && (num_test & 0b10))
            addr2 = addr1;
        std::stringstream stream;
        stream << std::hex << std::uppercase << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << addr1 << "_0x" << std::setw(sizeof(uint16_t) * 2) << addr2 << "_0x" << std::setw(sizeof(uint16_t) * 2) << num;

        auto test = [addr1, addr2, num](lc3::sim &sim, Tester &tester, double total_points) {
            Test(addr1, addr2, num, sim, tester, total_points);
        };
        tester.registerTest("Test_" + stream.str(), test, 1, true);
    }
}

#endif