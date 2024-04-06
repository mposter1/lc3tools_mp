#include "lab4.h"

void setup(Tester &tester)
{
    //std::mt19937 rng(0);
    /*for (uint16_t i = 1; i <= 6; i++)
    {
        std::map<MapStruct, LinkedListNode> memory;
        std::map<MapStruct, std::string> stringMemory;
        std::unordered_set<std::string> courses;
        std::pair<uint16_t, uint16_t> heads = {generateList(rng() % MAX_SIZE, rng, memory, stringMemory, courses), generateList(rng() % MAX_SIZE, rng, memory, stringMemory, courses)};
        auto queries = generateQueries(rng, courses, 0.75);

        tester.registerTest("Test " + std::to_string(i), [heads, memory, stringMemory, queries](lc3::sim &sim, Tester &tester, double total_points) {
            Test(heads, memory, stringMemory, queries, sim, tester, total_points);
        }, 5, true);
    }*/
    //uint16_t num_correct_queries = 1;
    //uint16_t num_incorrect_queries = 0;
    uint16_t num_nodes;
    bool query_type;
    int seed;

    for(uint16_t i = 2; i < 10; i++){
        seed = i;
        if (!(i % 2)){
            query_type = true;
        }else{
            query_type = false;
        }

        num_nodes = i;

        tester.registerTest("Test Random",[num_nodes, query_type, seed](lc3::sim &sim, Tester &tester, double total_points){
            Test(num_nodes,query_type,seed,sim,tester,total_points);
        }, 5,true);
    }
}