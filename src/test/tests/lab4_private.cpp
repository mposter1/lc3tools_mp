#include "lab4.h"

void setup(Tester &tester)
{
    /*std::mt19937 rng(611);
    std::map<MapStruct, LinkedListNode> memory;
    std::map<MapStruct, std::string> stringMemory;
    std::unordered_set<std::string> courses;
    std::pair<uint16_t, uint16_t> heads;
    std::array<std::string, NUM_QUERIES> queries;

    for (uint16_t i = 1; i <= 3; i++)
    {
        heads = {generateList(MAX_SIZE, rng, memory, stringMemory, courses), generateList(MAX_SIZE, rng, memory, stringMemory, courses)};
        queries = generateQueries(rng, courses, 0.1);
        tester.registerTest("Test max size: " + std::to_string(i), [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
            Test(heads, memory, stringMemory, queries, sim, tester, total_points);
        }, 5, true);
    }

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(0, rng, memory, stringMemory, courses), generateList(0, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 1);
    tester.registerTest("Test empty list", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(0, rng, memory, stringMemory, courses), generateList(MAX_SIZE, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 0.5);
    tester.registerTest("Test empty list 1", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(MAX_SIZE, rng, memory, stringMemory, courses), generateList(0, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 0.5);
    tester.registerTest("Test empty list 2", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(1, rng, memory, stringMemory, courses), generateList(1, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 0.5);
    tester.registerTest("Test single element list", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(NUM_QUERIES / 2, rng, memory, stringMemory, courses), generateList(NUM_QUERIES / 2, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 0, true);
    tester.registerTest("Test unique queries", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(NUM_QUERIES, rng, memory, stringMemory, courses), generateList(0, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 0, true);
    tester.registerTest("Test list 1 unique queries", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);

    memory.clear();
    stringMemory.clear();
    courses.clear();
    heads = {generateList(0, rng, memory, stringMemory, courses), generateList(NUM_QUERIES, rng, memory, stringMemory, courses)};
    queries = generateQueries(rng, courses, 0, true);
    tester.registerTest("Test list 2 unique queries", [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
        Test(heads, memory, stringMemory, queries, sim, tester, total_points);
    }, 5, true);*/


    uint16_t num_nodes;
    bool query_type;
    int seed;

    for(uint16_t i = 80; i < 85; i++){
        seed = i;
        if (!(i % 2)){
            query_type = true;
        }else{
            query_type = false;
        }

        num_nodes = i;

        tester.registerTest("Test Random (Large)",[num_nodes, query_type, seed](lc3::sim &sim, Tester &tester, double total_points){
            Test(num_nodes,query_type,seed,sim,tester,total_points);
        }, 3,true);

        tester.registerTest("Test Empty",[seed](lc3::sim &sim, Tester &tester, double total_points){
            testEmpty(seed,sim,tester,total_points);
        }, 3,true);

        tester.registerTest("Test Single",[num_nodes, query_type, seed](lc3::sim &sim, Tester &tester, double total_points){
            testSingle(seed,query_type,sim,tester,total_points);
        }, 2,true);
    }
}