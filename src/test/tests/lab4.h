#ifndef __LAB4_H__
#define __LAB4_H__

#define API_VER 2
#include "framework.h"
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <iomanip>

#define INSTRUCTION_LIMIT 5000000
#define NUM_QUERIES 50
#define MAX_SIZE 100

struct LinkedListNode
{
    uint16_t next;
    uint16_t prev;
    uint16_t data;
};

struct MapStruct
{
    uint16_t addr;
    size_t size;

    bool operator<(const MapStruct &other) const
    {
        return addr + size < other.addr;
    }
};

void Test(std::pair<uint16_t, uint16_t> heads, std::map<MapStruct, LinkedListNode> memory, std::map<MapStruct, std::string> stringMemory, std::array<std::string, NUM_QUERIES> queries, lc3::sim &sim, Tester &tester, double total_points)
{
    std::string queries_str;
    for (auto &query : queries)
        queries_str += query + "\n";
    tester.setInputString(queries_str);

    for (auto [addr, node] : memory)
    {
        sim.writeMem(addr.addr, node.next);
        sim.writeMem(addr.addr + 1, node.prev);
        sim.writeMem(addr.addr + 2, node.data);
        sim.writeStringMem(node.data, stringMemory.find({node.data, 0})->second);
        if (sim.readMem(node.data + stringMemory.find({node.data, 0})->second.size()) != 0)
            tester.error("String", "String not null terminated.");
    }
    sim.writeMem(0x4000, heads.first);
    sim.writeMem(0x4001, heads.second);

    std::unordered_set<std::string> list1, list2;
    auto processList = [&sim, &stringMemory](uint16_t head, std::unordered_set<std::string> &list)
    {
        while (head != 0)
        {
            std::string course;
            uint16_t data = sim.readMem(head + 2);
            while (sim.readMem(data) != 0)
                course += sim.readMem(data++);
            list.insert(course);
            head = sim.readMem(head);
        }
    };
    processList(heads.first, list1);
    processList(heads.second, list2);

    sim.run();

    std::stringstream outputs_str_stream = std::stringstream(tester.getOutput());
    std::cout << tester.getOutput() << std::endl;
    std::vector<std::string> outputs;
    std::string line;
    while (std::getline(outputs_str_stream, line))
        if (tester.checkContain(line, "Type Course Number and press Enter: "))
            outputs.push_back(line);

    for (uint16_t i = 0; i < queries.size(); i++)
    {
        if (i == outputs.size())
        {
            tester.error("Incomplete", "Not all queries were answered.");
            break;
        }
        std::string query = queries[i];
        if (list1.find(query) != list1.end())
            tester.verify(query, tester.checkMatch(outputs[i], "Type Course Number and press Enter: " + query + " is already being offered this semester."), total_points / queries.size());
        else if (list2.find(query) != list2.end())
        {
            tester.verify(query, tester.checkMatch(outputs[i], "Type Course Number and press Enter: " + query + " has been added to this semester's course offerings."), total_points / queries.size());
            list1.insert(query);
            list2.erase(query);
        }
        else
            tester.verify(query, tester.checkMatch(outputs[i], "Type Course Number and press Enter: The entered Course Number does not exist in the current course catalog."), total_points / queries.size());
    }
    if (outputs.size() > queries.size())
        tester.error("Extra", "Extra queries were answered.");
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

std::string generateRandomCourse(std::mt19937 &rng, std::unordered_set<std::string> &courses, bool add = true)
{
    std::string ret;
    uint16_t size = rng() % 3 + 1;
    while (size--)
    {
        char c = rng() % 26 + 'A';
        ret += c;
    }
    size = 3;
    while (size--)
    {
        char c = rng() % 10 + '0';
        ret += c;
    }
    if (rng() % 2)
        ret += rng() % 26 + 'A';
    if (courses.find(ret) != courses.end())
        return generateRandomCourse(rng, courses);

    if (add)
        courses.insert(ret);
    return ret;
}

uint16_t generateList(uint16_t size, std::mt19937 &rng, std::map<MapStruct, LinkedListNode> &memory, std::map<MapStruct, std::string> &stringMemory, std::unordered_set<std::string> &courses)
{
    std::uniform_int_distribution<uint16_t> dist(0x4002, 0xFD00);
    uint16_t nextNode = 0;
    MapStruct nodeMemory = {0, 3};
    while (size--)
    {
        do
            nodeMemory.addr = dist(rng);
        while (memory.find(nodeMemory) != memory.end() || stringMemory.find(nodeMemory) != stringMemory.end());

        std::string course = generateRandomCourse(rng, courses);
        MapStruct courseMemory = {0, course.size() + 1};
        do
            courseMemory.addr = dist(rng);
        while (memory.find(courseMemory) != memory.end() || stringMemory.find(courseMemory) != stringMemory.end());

        memory.insert({nodeMemory, {nextNode, 0, courseMemory.addr}});
        if (nextNode != 0)
            memory.find({nextNode, 0})->second.prev = nodeMemory.addr;
        nextNode = nodeMemory.addr;
        stringMemory.insert({courseMemory, course});
    }
    return nodeMemory.addr;
}

std::array<std::string, NUM_QUERIES> generateQueries(std::mt19937 &rng, std::unordered_set<std::string> &courses, double randomProb, bool unique = false)
{
    std::uniform_real_distribution<double> dist(0, 1);
    std::uniform_int_distribution<uint16_t> indexDist(0, courses.size() - 1);
    std::array<std::string, NUM_QUERIES> ret;
    for (auto &query : ret)
    {
        if (dist(rng) < randomProb || courses.size() == 0)
            query = generateRandomCourse(rng, courses, false);
        else
        {
            if (!unique)
            {
                auto it = courses.begin();
                std::advance(it, indexDist(rng));
                query = *it;
            }
            else
            {
                query = *courses.begin();
                courses.erase(courses.begin());
            }
        }
    }
    return ret;
}

#endif