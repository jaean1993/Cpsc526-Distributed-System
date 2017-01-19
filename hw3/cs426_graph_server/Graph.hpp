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
    static list < list<uint64_t> > graph;
    static int vertex;
    static int edge;
    static vector<uint64_t> id_vec;
    ;

public:
    
    static int add_node(uint64_t node_id);
    static int add_edge(uint64_t node_a_id,uint64_t node_b_id);
    static int remove_node(uint64_t node_id);
    static int remove_edge(uint64_t node_a_id,uint64_t node_b_id);
    static int get_node(uint64_t node_id);
    static int get_edge(uint64_t node_a_id,uint64_t node_b_id);
    static list<uint64_t> get_neighbors(uint64_t node_id);
    static int shortest_path(uint64_t node_a_id,uint64_t node_b_id);

};

#endif /* Graph_hpp */
