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
    if (find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end()) {//node does not exist
        id_vec.push_back(node_id);
        list<uint64_t> a;
        graph.push_back(a);
        return 200;//success
    }
    else{
        return 204;//node already exists
    }
}


int Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id){
    //get position from id_vec
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);
    if (ita == id_vec.end() || itb == id_vec.end() || ita == itb) {
        return 400;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    }

    // get list according to id
    list<list<uint64_t>>::iterator list_a = graph.begin();
    advance(list_a, (int)(ita - id_vec.begin()));
    list<list<uint64_t>>::iterator list_b = graph.begin();
    advance(list_b, (int)(itb - id_vec.begin()));
    
    //check specified nodes in list
    
    if ( find(list_a -> begin(), list_a -> end(), node_b_id) != list_a -> end() ) {
        return 204; //the edge already exists
    }
    else {
    	list_a->push_back(node_b_id);
    	list_b->push_back(node_a_id);
    	return 200;//success
    }
}


int Graph::remove_node(uint64_t node_id) {
    //need to remove all edges contains this node
    if (find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end()) {
        return 400;//node does not exist
    }
    else {
        vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_id);
        id_vec.erase(ita);
        list<list<uint64_t>>::iterator itb = graph.begin();
        advance(itb, (int)(ita - id_vec.begin()));
        graph.erase(itb);
        for (itb = graph.begin(); itb < graph.end(); itb++) {
        	itb -> remove(node_id);
        	}
        return 200;//success
    }
}


int Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id) {
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);

    if(ita == id_vec.end() || itb == id_vec.end()) {
        return 400; //if one or two nodes do not exist
    }
    else {
        list<list<uint64_t>>::iterator itc = graph.begin();
        advance(itc, (int)(ita - id_vec.begin()));
        if ( find(itc -> begin(), itc -> end(), node_b_id) == itc -> end() ) {
        	return 400;
        }
        else {
        	itc -> remove(node_b_id);
        	itc = graph.begin();
        	advance(itc, (int)(itb - id_vec.begin()));
        	itc -> remove(node_a_id);
            return 200;   //success
        }
    }
}


int Graph::get_node(uint64_t node_id) {
    if ( find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end() ) {
    			// not found node_id
    	return 200;
    }
    else {
        return 200; // found node_id
    }
}

int Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id) {
    //get position from id_vec
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);
    // get list according to id
    list<list<uint64_t>>::iterator list_a = graph.begin();
    advance(list_a, (int)(ita - id_vec.begin()));

    //check specified nodes in list
    if (ita == id_vec.end() || itb == id_vec.end() || ita == itb) {
        return 400;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    }
    else if ( find(list_a -> begin(), list_a -> end(), node_b_id) != list_a -> end() ) {
        return 200; //the edge already exists
    }
    else {
    	return 200; //the edge does not exist
    }
}

int Graph::get_neighbors(uint64_t node_id) {
    if (find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end()) {
        return 400; //if the node does not exist
    }
    else {
    	vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_id);
    	list<list<uint64_t>>::iterator itb = graph.begin();
    	advance(itb, (int)(ita - id_vec.begin()));
//    	return *itb; //should also return a list of neighbors
    	return 200;
    }
}

int Graph::shortest_path(uint64_t node_a_id, uint64_t node_b_id) {
    //get position from id_vec
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);
    if (ita == id_vec.end() || itb == id_vec.end() || ita == itb) {
        return 400;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    }

}


/*
list<uint64_t> Graph::shortest_path(uint64_t node_a_id,uint64_t node_b_id) {
    
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
        list<list<uint64_t>>::iterator list = graph.begin();
        advance(list, position);
        for (std::list<uint64_t>::iterator it = list->begin(); it != list->end(); it++) {
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
    
    list<uint64_t> result;
    stack<uint64_t> stack_temp;
    
    for (uint64_t start = position_b; start != position_a; start = edge_node[start]) {
        stack_temp.push(start);
    }
    while (stack_temp.size()) {
        list<uint64_t>::iterator list = id_list.begin();
        advance(list, stack_temp.top());
        result.push_back(*list);
        stack_temp.pop();
    }
    
    free(mark);
    free(edge_node);
    return result;
}
*/



