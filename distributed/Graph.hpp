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
    vector<uint64_t> id_list;
    vector<vector<uint64_t>> graph;
public:
    
    int add_node(uint64_t node_id);
    int add_edge(uint64_t node_a_id,uint64_t node_b_id);
    int remove_node(uint64_t node_id);
    int remove_edge(uint64_t node_a_id,uint64_t node_b_id);
    int get_node(uint64_t node_id);
    int get_edge(uint64_t node_a_id,uint64_t node_b_id);
    vector<uint64_t> get_neighbors(uint64_t node_id);
    vector<uint64_t> shortest_path(uint64_t node_a_id,uint64_t node_b_id);
    void remove_single_edge(uint64_t node_a_id, uint64_t node_b_id);
};

#endif /* Graph_hpp */
