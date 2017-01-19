//
//  Graph.hpp
//  distributed
//
//  Created by 安槿 on 16/9/7.
//  Copyright © 2016年 anjin. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp
#include <cstdint>
#include <list>
#include <vector>
#include <map>
#include <stdio.h>

using namespace std;

class Graph
{
private:
    int vertex;
    int edge;
    vector<uint64_t> id_vec;
    list < list<uint64_t> > graph;
    uint64_t log_tail = 0;
    uint64_t log_block_count = 1;
    typedef struct{
        uint32_t cur_gen_num;
        uint64_t check_sum;
        uint32_t log_start;
        uint32_t block_size;
    } super_struct;
    
    typedef struct{
        uint32_t action;
        uint64_t node_a;
        uint64_t node_b;
    } log_entry;
    
    typedef struct {
        uint32_t gen_num;
        uint32_t ent_num;
        uint64_t check_sum;
        log_entry entry[170];
    } log_struct;
    
    typedef struct {
        uint64_t node_a;
        uint64_t node_b;
    } checkpoint_entry;
    
    typedef struct {
        uint64_t size;
        uint64_t signal = 1;
        checkpoint_entry entries[255];
    } checkpoint;
    
    

public:
    char* disk_path;
    int add_node(uint64_t node_id,uint64_t initial = 0);
    int add_edge(uint64_t node_a_id,uint64_t node_b_id, uint64_t initial = 0);
    int remove_node(uint64_t node_id, uint64_t initial = 0);
    int remove_edge(uint64_t node_a_id,uint64_t node_b_id,  uint64_t initial = 0);
    int get_node(uint64_t node_id);
    int get_edge(uint64_t node_a_id,uint64_t node_b_id);
    list<uint64_t> get_neighbors(uint64_t node_id);
    int shortest_path(uint64_t node_a_id,uint64_t node_b_id);
    int check_point();
    int write_log(uint32_t opcode, uint64_t node_a, uint64_t node_b);
    void format();
    int init();
    
};

#endif /* Graph_hpp */
