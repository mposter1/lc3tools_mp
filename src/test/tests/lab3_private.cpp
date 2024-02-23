#include "lab3.h"

void setup(Tester &tester)
{
    std::mt19937 mt(0x1000);

    gen_random(10, mt, tester);

    for (uint16_t i = 0; i < 5; i++)
    {
        std::vector<uint16_t> array = gen_array(0, mt);
        uint16_t terminator = mt() % MAX_SIZE;
        tester.registerTest("Empty array", [array, terminator](lc3::sim &sim, Tester &tester, double total_points) {
            Test(array, terminator, sim, tester, total_points);
        }, 1, true);
    }

    for (uint16_t i = 0; i < 10; i++)
    {
        std::vector<uint16_t> array = gen_array(MAX_SIZE - 1, mt);
        uint16_t terminator = mt() % MAX_SIZE;
        tester.registerTest("Max size array", [array, terminator](lc3::sim &sim, Tester &tester, double total_points) {
            Test(array, terminator, sim, tester, total_points);
        }, 1, true);
    }

    for (uint16_t i = 0; i < 5; i++)
    {
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);
        std::sort(array.begin(), array.end(), [](uint16_t a, uint16_t b) { return (a & 0xFF) < (b & 0xFF); });
        uint16_t terminator = mt() % MAX_SIZE;
        tester.registerTest("Sorted array", [array, terminator](lc3::sim &sim, Tester &tester, double total_points) {
            Test(array, terminator, sim, tester, total_points);
        }, 1, true);
    }

    for (uint16_t i = 0; i < 10; i++)
    {
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);
        std::sort(array.rbegin(), array.rend(), [](uint16_t a, uint16_t b) { return (a & 0xFF) < (b & 0xFF); });
        uint16_t terminator = mt() % MAX_SIZE;
        tester.registerTest("Reverse array", [array, terminator](lc3::sim &sim, Tester &tester, double total_points) {
            Test(array, terminator, sim, tester, total_points);
        }, 1, true);
    }

    for (uint16_t i = 0; i < 10; i++)
    {
        std::vector<uint16_t> array = gen_array(mt() % MAX_SIZE, mt);
        uint16_t val = mt() % MAX_SIZE;
        for (auto &num : array)
            num = (num & 0xFF00) | val;
        uint16_t terminator = mt() % MAX_SIZE;
        tester.registerTest("Equal values array", [array, terminator](lc3::sim &sim, Tester &tester, double total_points) {
            Test(array, terminator, sim, tester, total_points);
        }, 1, true);
    }
}