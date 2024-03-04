#ifndef __LAB2_H__
#define __LAB2_H__

#define API_VER 2
#include "framework.h"
#include <random>
#include <iomanip>

void Test(uint16_t addr1, uint16_t addr2, uint16_t val, lc3::sim &sim, Tester &tester, double total_points)
{
    sim.writeMem(0x6000, addr1);
    sim.writeMem(0x6001, addr2);
    sim.writeMem(addr1, val);

    sim.run();

    uint16_t student_sum = sim.readMem(addr2);
    uint16_t expected_sum = (val & 0xFF) + ((val >> 8) & 0xFF);

    /*
    tester.output("At x" + fmt::format("{:#x}", addr1) + ", expected " + fmt::format("{:#x}",expected_sum));
    tester.output("Got " + fmt::format("{:#x}", student_sum));
    tester.output("Was given addr1=" + fmt::format("{:#x}",addr1) + ", which stored a value of " + fmt::format("{:#x}",val));
    */

    tester.verify("Test", student_sum == expected_sum, total_points);

}

void testBringup(lc3::sim &sim)
{
    sim.writePC(0x3000);
    sim.setRunInstLimit(10000000);
}

void testTeardown(lc3::sim &sim) {}

void shutdown(void) {}

void lab2_setup_private(uint16_t num_tests, uint16_t seed, Tester &tester, bool includeCornerCases)
{
    std::mt19937 mt(seed);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t num = mt() % 0x10000;
        uint16_t addr1 = 0x6002 + (mt() % 0x8DFE);
        uint16_t addr2 = 0x6002 + (mt() % 0x8DFE);
        std::stringstream stream;
        if (includeCornerCases && (num_test & 0b10)){
            addr2 = addr1;
            stream << "Overloaded_Addresses";
        }
        else{
            stream << "Random_Addresses";
        }

        //stream << std::hex << std::uppercase << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << addr1 << "_0x" << std::setw(sizeof(uint16_t) * 2) << addr2 << "_0x" << std::setw(sizeof(uint16_t) * 2) << num;

        auto test = [addr1, addr2, num](lc3::sim &sim, Tester &tester, double total_points) {
            Test(addr1, addr2, num, sim, tester, total_points);
        };
        tester.registerTest("Test_" + stream.str(), test, 1, true);
    }
}

void lab2_setup_public(uint16_t num_tests, uint16_t seed, Tester &tester, bool includeCornerCases)
{
    std::mt19937 mt(seed);

    for (uint16_t num_test = 0; num_test < num_tests; num_test++)
    {
        uint16_t num = mt() % 0x10000;
        uint16_t addr1 = 0x6002 + (mt() % 0x8DFE);
        uint16_t addr2 = 0x6002 + (mt() % 0x8DFE);
        if (includeCornerCases && (num_test & 0b10))
            addr2 = addr1;
        std::stringstream stream;
        stream << std::hex << std::uppercase << "(0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << addr1 << ",0x" << std::setw(sizeof(uint16_t) * 2) << addr2 << ",0x" << std::setw(sizeof(uint16_t) * 2) << num << ")";

        auto test = [addr1, addr2, num](lc3::sim &sim, Tester &tester, double total_points) {
            Test(addr1, addr2, num, sim, tester, total_points);
        };
        tester.registerTest("(Values_Address,Storage_Address,Values_To_Add):" + stream.str(), test, 1, true);
    }
}

#endif