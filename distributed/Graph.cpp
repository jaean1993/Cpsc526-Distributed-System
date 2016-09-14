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
    uint64_t position_a = *find(id_list.begin(), id_list.end(), node_a_id);
    uint64_t position_b = *find(id_list.begin(), id_list.end(), node_b_id);
    
    if (position_a == graph.size() || position_b == graph.size() || position_a == position_b) {
        return 0;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    }
    
    // get list according to id
    list<list<uint64_t>>::iterator list_a = graph.begin();
    advance(list_a, position_a);
    list<list<uint64_t>>::iterator list_b = graph.begin();
    advance(list_b, position_b);
    
    //check specified nodes in list
    bool found_a = (std::find(*list_a -> begin(), *list_a -> end(), node_b_id) != *list_a -> end());
    bool found_b = (std::find(*list_b -> begin(), *list_b -> end(), node_a_id) != *list_b -> end());
    
    if (found_a && found_b) {
        return 2; //the edge already exists
    }
    
    if (!found_a) {
        list_a->push_back(node_b_id);
    }
    if (!found_b) {
        list_b->push_back(node_a_id);
    }
    
    return 1;//success
}

int Graph::remove_node(uint64_t node_id) {
    //need to remove all edge contains this node
    
    if (!get_node(node_id)) {
        return 0;//node does not exist
    }else {
        list<uint64_t>::iterator position = find(id_list.begin(), id_list.end(), node_id);
        list<list<uint64_t>>::iterator list = graph.begin();
        advance(list, *position);
        
        for (std::list<uint64_t>::iterator it = list->begin(); it != list->end(); it++) {
            remove_single_edge(*it, node_id);
        }//erase node'edge in other nodes
        
        graph.erase(list);//erase node's own list
        
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
    list<list<uint64_t>>::iterator list_a = graph.begin();
    advance(list_a, position_a);
    list_a->erase(find(id_list.begin(), id_list.end(), node_b_id));
    
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
    
    list<list<uint64_t>>::iterator list_a = graph.begin();
    advance(list_a, position_a);
    list<list<uint64_t>>::iterator list_b = graph.begin();
    advance(list_b, position_b);
    
    bool found_a = (std::find(*list_a -> begin(), *list_a -> end(), node_b_id) != *list_a -> end());
    bool found_b = (std::find(*list_b -> begin(), *list_b -> end(), node_a_id) != *list_b -> end());
    
    if (found_a && found_b) {
        return 1; //the edge already exists
    }
    return 0;
}

list<uint64_t> Graph::get_neighbors(uint64_t node_id) {
    list<uint64_t> result;
    if (!get_node(node_id)) {
        return result;
    }
    
    list<uint64_t>::iterator position = find(id_list.begin(), id_list.end(), node_id);
    list<list<uint64_t>>::iterator list = graph.begin();
    advance(list, *position);
    
    for (std::list<uint64_t>::iterator it = list->begin(); it != list->end(); it++) {
        result.push_back(*it);
    }
    
    return result;
}




