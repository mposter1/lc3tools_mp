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

#define INSTRUCTION_LIMIT 500000
#define NUM_QUERIES 50
#define MAX_SIZE 100
#define UPPER_ADDRESS_LIM 0xFD00
#define LOWER_ADDRESS_LIM 0x4002
#define HEAD_PTR 0x4000
#define ROOM_NAME_SIZE 10
#define NULL_SENTINEL 0x0000
#define ORIGIN 0x3000
#define MAX_NODES_PRINTED 15

std::vector<uint16_t> stringToUint16Array(const std::string& str) {
    std::vector<uint16_t> result;
    for (char c : str) {
        result.push_back(static_cast<uint16_t>(c));
    }
    return result;
}

std::string generateRandomRoomASCII(std::mt19937 &rng)
{
    std::string ret;
    uint16_t size = 3;
    char c;
    while (size--)
    {
        c = rng() % 26 + 'A';
        ret += c;
    }
    ret += ' ';
    c = rng() % 10 + '0';
    ret += c;
    ret += '.';
    size = 3;
    while (size--)
    {
        c = rng() % 10 + '0';
        ret += c;
    }
    if (rng() % 2)
        ret += rng() % 26 + 'A';
    //if (courses.find(ret) != courses.end())
    //    return generateRandomCourse(rng, courses);

    //if (add)
    //    courses.insert(ret);
    return ret;
}

bool checkStringZ(uint16_t string_ptr, uint16_t string_size, std::unordered_set<int> occupied_addresses){
    for(uint16_t zIndex = 0; zIndex < string_size; zIndex++){
        if(occupied_addresses.find(string_ptr+zIndex) != occupied_addresses.end()){
            return false;
        }
    }
    return true;
}

void Test(uint16_t num_nodes, bool query_type, int seed,
          lc3::sim &sim, Tester &tester, double total_points) {
    std::mt19937 rng(seed);
    sim.writeMem(HEAD_PTR, 0x000);
    std::uniform_int_distribution<uint16_t> address_distro(LOWER_ADDRESS_LIM, UPPER_ADDRESS_LIM);
    uint16_t node_next_address;
    uint16_t curr_node_ptr = HEAD_PTR;
    uint16_t node_data_address;

    uint16_t string_ptr;
    std::vector<uint16_t> string_ptrs;
    std::string room_string;
    std::vector<std::string> room_strings;
    std::vector<uint16_t> room_ascii;
    std::unordered_set<int> occupied_addresses;
    int total_queries = 1;

    if (num_nodes > 0){
        curr_node_ptr = address_distro(rng);
        sim.writeMem(HEAD_PTR,curr_node_ptr);
        occupied_addresses.insert(curr_node_ptr);
        occupied_addresses.insert(curr_node_ptr+1);
    }

    std::cout << std::setfill(' ') << std::setw(17) << "Nodes Address: ";

    for (int i = 0; i < num_nodes; i++) {

        if (i != num_nodes - 1) {
            node_next_address = address_distro(rng);
            node_data_address = node_next_address + 1;

            while ((occupied_addresses.find(node_next_address) != occupied_addresses.end()) ||
                   (occupied_addresses.find(node_data_address) != occupied_addresses.end())) {
                node_next_address = address_distro(rng);
                node_data_address = node_next_address + 1;
            }
        }else{
            node_next_address = 0x0000;
        }

        sim.writeMem(curr_node_ptr, node_next_address);

        occupied_addresses.insert(node_next_address);

        room_string = generateRandomRoomASCII(rng);
        room_strings.push_back(room_string);
        room_ascii = stringToUint16Array(room_string);

        string_ptr = address_distro(rng);
        while (!checkStringZ(string_ptr, room_string.size() + 1, occupied_addresses)) {
            string_ptr = address_distro(rng);
        }
        string_ptrs.push_back(string_ptr);
        sim.writeMem(curr_node_ptr+1, string_ptr);


        for (uint16_t zIndex = 0; zIndex < room_ascii.size(); zIndex++) {
            sim.writeMem(string_ptr + zIndex, room_ascii[zIndex]);
            occupied_addresses.insert(string_ptr + zIndex);
        }
        sim.writeMem(string_ptr + room_ascii.size(), NULL_SENTINEL);
        occupied_addresses.insert(string_ptr + room_ascii.size());

        if (num_nodes < MAX_NODES_PRINTED) {
            std::cout << "0x" << std::hex << curr_node_ptr << ' ';
        }

        curr_node_ptr = node_next_address;

    }
    std::cout << std::endl;

    std::uniform_int_distribution<uint16_t> query_distro(0,num_nodes-1);
    std::cout << std::setfill(' ') << std::setw(17) << "Str Address: ";
    if(num_nodes < MAX_NODES_PRINTED) {
        for (const std::uint16_t str_ptr: string_ptrs) {
            std::cout << "0x" << std::hex << str_ptr << ' ';
        }
    }
    std::cout << std::endl;

    std::cout << std::setfill(' ') << std::setw(17) << "Rooms: ";

    if(num_nodes < MAX_NODES_PRINTED){
        for (const std::string& str : room_strings) {
            std::cout << str << ' ';
        }
    }

    std::cout << std::endl;

    bool check;
    std::string output;

    std::stringstream outputs_str_stream = std::stringstream(tester.getOutput());

    tester.setInputCharDelay(0);

    bool success = true;
    tester.clearOutput();
    sim.writePC(ORIGIN);

    std::string string_in_list;
    if (query_type) {
        string_in_list = room_strings.at(rand() % (num_nodes-1));//query_distro(rng) % num_nodes);
    }else{
        string_in_list = generateRandomRoomASCII(rng);
    }
    tester.setInputString(string_in_list + "\n");

    success &= sim.runUntilHalt();

    output = tester.getOutput();

    check = tester.checkContain(output, "Type the room to be reserved and press Enter: ");
    tester.verify("Is Prompt Correct?", check, 0);
    if (query_type) {
        check = tester.checkContain(output, string_in_list + " is currently available!");
    }else{
        check = tester.checkContain(output, string_in_list + " is NOT currently available.");
    }
    tester.verify("Correct Output for Correct Query in List?", check, total_points/(total_queries));

    tester.verify("Timeout?", !sim.didExceedInstLimit(), 0);

}
void testEmpty( int seed,lc3::sim &sim, Tester &tester, double total_points) {

    sim.writeMem(HEAD_PTR,0x0000);
    sim.writePC(ORIGIN);
    std::mt19937 rng(seed);
    std::string bad_string = generateRandomRoomASCII(rng);
    tester.clearOutput();
    tester.setInputString(bad_string + "\n");
    bool success = sim.runUntilHalt();
    std::string output = tester.getOutput();
    bool check = tester.checkContain(output, "Type the room to be reserved and press Enter: ");
    tester.verify("Is Prompt Correct?", check, 0);
    check = tester.checkContain(output, bad_string + " is NOT currently available.");
    tester.verify("Correct Output for Query in List?", check, total_points);
    tester.verify("Timeout?", !sim.didExceedInstLimit(), 0);
}

void testSingle(int seed, bool query_type, lc3::sim &sim, Tester &tester, double total_points) {

    sim.writeMem(HEAD_PTR,0x0000);
    sim.writePC(ORIGIN);
    std::mt19937 rng(seed);

    std::string room_string = generateRandomRoomASCII(rng);
    std::vector<uint16_t> room_ascii = stringToUint16Array(room_string);
    std::uniform_int_distribution<uint16_t> address_distro(LOWER_ADDRESS_LIM, UPPER_ADDRESS_LIM);

    tester.clearOutput();
    std::string query_string;
    if(query_type){
        query_string = room_string;
    }else{
        query_string = generateRandomRoomASCII(rng);
    }

    tester.setInputString(query_string + "\n");

    uint16_t node = address_distro(rng);
    uint16_t string_ptr = address_distro(rng);

    sim.writeMem(HEAD_PTR,node);
    sim.writeMem(node,NULL_SENTINEL);
    sim.writeMem(node+1,string_ptr);

    for (uint16_t zIndex = 0; zIndex < room_ascii.size(); zIndex++) {
        sim.writeMem(string_ptr + zIndex, room_ascii[zIndex]);
    }
    sim.writeMem(string_ptr + room_ascii.size(), NULL_SENTINEL);

    bool success = sim.runUntilHalt();
    std::string output = tester.getOutput();
    bool check = tester.checkContain(output, "Type the room to be reserved and press Enter: ");
    tester.verify("Is Prompt Correct?", check, 0);
    if (query_type) {
        check = tester.checkContain(output, query_string + " is currently available!");
    }else{
        check = tester.checkContain(output, query_string + " is NOT currently available.");
    }
    tester.verify("Correct Output for Query in List?", check, total_points);
    tester.verify("Timeout?", !sim.didExceedInstLimit(), 0);
}

void testBringup(lc3::sim &sim)
{
    sim.writePC(0x3000);
    sim.setRunInstLimit(INSTRUCTION_LIMIT);
}

void testTeardown(lc3::sim &sim)
{
    std::cout << "Done in " << std::to_string(sim.getInstExecCount()) << " instructions.\n";
    if (sim.didExceedInstLimit())
        std::cout << "Test exceeded instruction limit of " << INSTRUCTION_LIMIT << " instructions.\n";
}

void shutdown(void) {}

#endif