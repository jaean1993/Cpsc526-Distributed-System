//
//  Graph.cpp
//  distributed
//
//  Created by 安槿 on 16/9/7.
//  Copyright © 2016年 anjin. All rights reserved.
//

#include "Graph.hpp"
#include <cstdint>
#include <list>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include<stack>

using namespace std;

int Graph::add_node(uint64_t node_id){
    
    if (!get_node(node_id)) {
        id_list.push_back(node_id);
        return 1;//success
    }else{
        return 0;//node already exists
    }
}

int Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id){

    //get position from id_list
    int position_a = *find(id_list.begin(), id_list.end(), node_a_id);
    int position_b = *find(id_list.begin(), id_list.end(), node_b_id);
    
    if (position_a == graph.size() || position_b == graph.size() || position_a == position_b) {
        return 0;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    }
    
    // get list according to id
    vector<uint64_t> list_a = graph[position_a];
    vector<uint64_t> list_b = graph[position_b];
    
    //check specified nodes in list
    bool found_a = (std::find(list_a.begin(), list_a.end(), node_b_id) != list_a.end());
    bool found_b = (std::find(list_b.begin(), list_b.end(), node_a_id) != list_b.end());
    
    if (found_a && found_b) {
        return 2; //the edge already exists
    }
    
    if (!found_a) {
        list_a.push_back(node_b_id);
    }
    if (!found_b) {
        list_b.push_back(node_a_id);
    }
    
    return 1;//success
}

int Graph::remove_node(uint64_t node_id) {
    //need to remove all edge contains this node
    
    if (!get_node(node_id)) {
        return 0;//node does not exist
    }else {
        vector<uint64_t>::iterator position = find(id_list.begin(), id_list.end(), node_id);
        vector<uint64_t> list = graph[*position];
        
        for (vector<uint64_t>::iterator it = list.begin(); it != list.end(); it++) {
            remove_single_edge(*it, node_id);
        }//erase node'edge in other nodes
        
        graph.erase(graph.begin()+ *position - 1);//erase node's own list
        
        id_list.erase(position); //erase node in id_list
        
        return 1;//success
    }
}

int Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id) {
    
    if(get_edge(node_a_id, node_b_id) == 0 ) {
        return 0;
    }
    remove_single_edge(node_a_id, node_b_id);
    remove_single_edge(node_b_id, node_a_id);
    return 1;   //success
}

void Graph::remove_single_edge(uint64_t node_a_id, uint64_t node_b_id) {
    uint64_t position_a = *find(id_list.begin(), id_list.end(), node_a_id);
    vector<uint64_t> list_a = graph[position_a];
    list_a.erase(find(id_list.begin(), id_list.end(), node_b_id));
    
}

int Graph::get_node(uint64_t node_id) {
    bool found = std::find(id_list.begin(), id_list.end(), node_id) == id_list.end();
    if (!found) {
        return 0; // not found node_id
    }
    return 1; // found node_id
}

int Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id) {
    uint64_t position_a = *find(id_list.begin(), id_list.end(), node_a_id);
    uint64_t position_b = *find(id_list.begin(), id_list.end(), node_b_id);
    
    if (position_a == graph.size() || position_b == graph.size() || position_a == position_b) {
        return 0;//if either node doesn't exist, or if node_a_id is the same as node_b_id
    }
    
    vector<uint64_t> list_a = graph[position_a];
    vector<uint64_t> list_b = graph[position_b];
    
    bool found_a = (std::find(list_a.begin(), list_a.end(), node_b_id) != list_a.end());
    bool found_b = (std::find(list_b.begin(), list_b.end(), node_a_id) != list_b.end());
    
    if (found_a && found_b) {
        return 1; //the edge already exists
    }
    return 0;
}

vector<uint64_t> Graph::get_neighbors(uint64_t node_id) {
    vector<uint64_t> result;
    if (!get_node(node_id)) {
        return result;
    }
    
    vector<uint64_t>::iterator position = find(id_list.begin(), id_list.end(), node_id);
    vector<uint64_t> list = graph[*position];
    
    for (vector<uint64_t>::iterator it = list.begin(); it != list.end(); it++) {
        result.push_back(*it);
    }
    
    return result;
}

vector<uint64_t> Graph::shortest_path(uint64_t node_a_id,uint64_t node_b_id) {
    
    uint64_t position_a = *find(id_list.begin(), id_list.end(), node_a_id);
    uint64_t position_b = *find(id_list.begin(), id_list.end(), node_b_id);
    
    bool* mark = (bool*)malloc(id_list.size() * sizeof(bool));
    uint64_t* edge_node = (uint64_t*)malloc(id_list.size() * sizeof(uint64_t));
    queue<uint64_t> q;
    mark[position_a] = true;
    q.push(node_a_id);
    
    bool found = false;
    
    while (q.size() != 0 && !found) {
        uint64_t position = q.front();
        q.pop();
        vector<uint64_t> list = graph[position];
        for (vector<uint64_t>::iterator it = list.begin(); it != list.end(); it++) {
            uint64_t position_new = *find(id_list.begin(), id_list.end(), *it);
            if (!mark[position_new]) {
                edge_node[position] = position_new;
                mark[position_new] = true;
                q.push(position_new);
            }
            if(mark[position_b]) {
                found = true;
                break;
            }
        }
    }
    
    vector<uint64_t> result;
    stack<uint64_t> stack_temp;
    
    for (uint64_t start = position_b; start != position_a; start = edge_node[start]) {
        stack_temp.push(start);
    }
    while (stack_temp.size()) {
        result.push_back(id_list[stack_temp.top()]);
        stack_temp.pop();
    }
    result.push_back(node_a_id);
    free(mark);
    free(edge_node);
    return result;
}




