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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

#include <iostream>


using namespace std;

int Graph::init() {
    int fd = open(disk_path, O_RDWR | O_DIRECT);
    
    super_struct * superblock = (super_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    bool checksum = (superblock->check_sum == (superblock->block_size ^ superblock->log_start + 6));
    
    if (!checksum || superblock->cur_gen_num == 0) {
        munmap(superblock,4096);
        close(fd);
        return 400;
    }

    //restore the graph from check point
    uint64_t count = 0;
    checkpoint * entry_map;
    
    while (true) {
        uint64_t temp = (uint64_t)2048*1024*1024 + count++ * 4096;
        entry_map = (checkpoint*) mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, temp);
    
        uint64_t size = entry_map->size;
        uint64_t signal = entry_map->signal;

        //1 is the signal that the memory has been modified, 2 means that the block is just full;
        if (signal != 1 && signal != 2) {
            break;
        }
    
    
    
        for(int i = 0; i < size; i++) {
            uint64_t node_a = entry_map->entries[i].node_a;
            uint64_t node_b = entry_map->entries[i].node_b;

            if (get_node(node_a) == 0) {
                add_node(node_a,1);
            }
            if (get_node(node_b) == 0) {
                add_node(node_b,1);
            }
            if (get_edge(node_a, node_b) != 1 || get_edge(node_b, node_a) != 1) {
                add_edge(node_a, node_b,1);
            }
            
        }
        munmap(entry_map,4096);
    
        if (size < 255 || signal == 2) {
            break;
        }
    }


    //restore the graph from log
    
    log_struct * logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (uint64_t)4096);
    cout<<superblock->cur_gen_num<<endl;
    cout<<logblock->gen_num<<endl;
    cout<<log_block_count<<endl;
    while (logblock->gen_num != 0 && logblock->gen_num < superblock->cur_gen_num) {

        logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, ++log_block_count * (uint64_t)4096);
    }
    cout<<log_block_count<<endl;
    while (logblock->gen_num == superblock->cur_gen_num) {
        log_tail = 0;
        for(log_entry entry: logblock->entry) {
            uint32_t action = entry.action;

            //action == 0  means meeting empty zone
            if (action == 0) {
                cout<<"logblock->gen_num"<<endl;
                cout<<logblock->gen_num<<endl;
                cout<<"log_block_count when restore"<<endl;
                cout<<log_block_count<<endl;
                cout<<"log_tail when restore"<<endl;
                cout<<log_tail<<endl;
                break;
            }
            cout<<"action"<<endl;
            cout<<action<<endl;
            switch (action) {
                case 4:
                    cout<<"what node"<<endl;
                    cout<<entry.node_a<<endl;
                    add_node(entry.node_a, 1);
                    break;
                case 1:
//                    cout<<"add edge"<<endl;
//                    cout<<"entry.node_a"<<endl;
//                    cout<<entry.node_a<<endl;
//                    cout<<"entry.node_b"<<endl;
//                    cout<<entry.node_b<<endl;
                    add_edge(entry.node_a, entry.node_b, 1);
                    break;
                case 2:
                    remove_node(entry.node_a, 1);
                    break;
                case 3:
                    cout<<"remove edge"<<endl;
                    cout<<entry.node_a<<endl;
                    cout<<entry.node_b<<endl;
                    remove_edge(entry.node_a, entry.node_b, 1);
                    break;
                default:
                    break;
            }
            log_tail++;
        }
        //change to next block
        logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, ++log_block_count * (uint64_t)4096);
        if (logblock->gen_num != superblock->cur_gen_num) {
            log_block_count--;
            break;
        }
//        cout<<"logblock->gen_num"<<endl;
//        cout<<logblock->gen_num<<endl;
//        cout<<"logblock count"<<endl;
//        cout<<log_block_count<<endl;
    }
    
    
    munmap(superblock,4096);
    
    close(fd);
    return 0;
}

int Graph::add_node(uint64_t node_id, uint64_t initial){
    
    if (find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end()) {//node does not exist
        id_vec.push_back(node_id);
        list<uint64_t> a;
        graph.push_back(a);
        vertex++;
        
        if (initial == 0) {
            int result = write_log(4,node_id,0);
            if (result == 507) {
                return 507;
            }
        }
        
        return 200;//success
    } else{
        return 204;//node already exists
    }
    
}


int Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id,uint64_t initial){
    //get position from id_vec
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);
    
    if (ita == id_vec.end() || itb == id_vec.end() || ita == itb) {
        return 400;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    }

//    Get list according to id
    list< list<uint64_t> >::iterator list_a = graph.begin();
    advance(list_a, (int)(ita - id_vec.begin()));
    list< list<uint64_t> >::iterator list_b = graph.begin();
    advance(list_b, (int)(itb - id_vec.begin()));
    
//    Check specified nodes in list
    if ( find(list_a -> begin(), list_a -> end(), node_b_id) != list_a -> end() ) {
        return 204; //The edge already exists
    } else {
    	list_a->push_back(node_b_id);
    	list_b->push_back(node_a_id);
        edge++;
        if (initial == 0) {
            cout<<"add edge in log"<<endl;
            if (write_log(1,node_a_id,node_b_id) == 507) {
                return 507;
            }
        }
        
    	return 200;//Success

    }
    return 200;
}


int Graph::remove_node(uint64_t node_id, uint64_t initial) {

    //Need to remove all edges contains this node
    if (find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end()) {
        return 400;//Node does not exist
    } else {
        vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_id);
        id_vec.erase(ita);
        list< list<uint64_t> >::iterator itb = graph.begin();
        advance(itb, (int)(ita - id_vec.begin()));
        graph.erase(itb);
        for(list< list<uint64_t> >::iterator itc = graph.begin(); itc != graph.end(); itc++) {
        	itc -> remove(node_id);
        }
        vertex--;
        if (initial == 0) {
            if(write_log(2,node_id,0) == 507) {
                return 507;
            }
        }
        
        return 200;//Success

    }
    
}


int Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id, uint64_t initial) {
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);

    if(ita == id_vec.end() || itb == id_vec.end()) {
        return 400; //if one or two nodes do not exist
    } else {
        list< list<uint64_t> >::iterator itc = graph.begin();
        advance(itc, (int)(ita - id_vec.begin()));
        if ( find(itc -> begin(), itc -> end(), node_b_id) == itc -> end() ) {
        	return 400;
        } else {
        	itc -> remove(node_b_id);
        	itc = graph.begin();
        	advance(itc, (int)(itb - id_vec.begin()));
        	itc -> remove(node_a_id);
            edge--;
            if (initial == 0) {
                cout<<"remove edge in log"<<endl;
                if(write_log(3,node_a_id,node_b_id) == 507) {
                    return 507;
                }
            }
            
            return 200;   //success
        }
    }
    
}


int Graph::get_node(uint64_t node_id) {
    if ( find(id_vec.begin(), id_vec.end(), node_id) == id_vec.end() ) {
    	return 0;
    } else {
        return 1;
    }
}

int Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id) {
    //get position from id_vec
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);
    // get list according to id
    list< list<uint64_t> >::iterator list_a = graph.begin();
    advance(list_a, (int)(ita - id_vec.begin()));

    //check specified nodes in list
    if (ita == id_vec.end() || itb == id_vec.end() || ita == itb) {
        return -1;   //if either node doesn't exist, or if node_a_id is the same as node_b_id
    } else if ( find(list_a -> begin(), list_a -> end(), node_b_id) != list_a -> end() ) {
        return 1; //the edge exists
    } else {
    	return 0; //the edge does not exist
    }
}

list<uint64_t> Graph::get_neighbors(uint64_t node_id) {
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_id);
    list< list<uint64_t> >::iterator itb = graph.begin();
    advance(itb, (int)(ita - id_vec.begin()));
    return *itb; //Return a list of neighbors
}

int Graph::shortest_path(uint64_t node_a_id, uint64_t node_b_id) {
    //get position from id_vec
    vector<uint64_t>::iterator ita = find(id_vec.begin(), id_vec.end(), node_a_id);
    vector<uint64_t>::iterator itb = find(id_vec.begin(), id_vec.end(), node_b_id);
    int alias_a = (int)(ita - id_vec.begin());
    
    if (ita == id_vec.end() || itb == id_vec.end()) {
        return -1;   //If either node doesn't exist
    }
    else if (ita == itb) {
        return 0;   //If node_a_id and node_b_id are the same
    }
    else {
        int distance[vertex];
        for (int i = 1; i < vertex; i++) {
            distance[i] = -1;
        }
        distance[alias_a] = 0;
        queue<uint64_t> myqueue;
        myqueue.push(node_a_id);
        while(!myqueue.empty()) {
            uint64_t node = myqueue.front();
            myqueue.pop();
            vector<uint64_t>::iterator itx = find(id_vec.begin(), id_vec.end(), node);
            int alias = (int)(itx - id_vec.begin());
            list< list<uint64_t> >::iterator ity = graph.begin();
            advance(ity, alias);
            list<uint64_t> li = *ity;
            for (list<uint64_t>::iterator itz = li.begin(); itz != li.end(); itz++) {
                if (*itz == node_b_id) {
                    return distance[alias] + 1;
                }
                vector<uint64_t>::iterator itt = find(id_vec.begin(), id_vec.end(), *itz);
                int inalias = (int)(itt - id_vec.begin());
                if (distance[inalias] == -1) {
                    myqueue.push(*itz);
                    distance[inalias] = distance[alias] + 1;
                }
            }
        }
        return -2;

    }
}

void Graph::format() {
    int fd = open(disk_path, O_RDWR|O_DIRECT);
    super_struct * superblock = (super_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    bool checksum = (superblock->check_sum == (superblock->block_size ^ superblock->log_start + 6));
    
    if (checksum && superblock->cur_gen_num != 0) {
        superblock->cur_gen_num += 1;
        uint64_t temp = (uint64_t)2048*1024*1024;
        checkpoint * entry_map = (checkpoint *) mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, temp);
        entry_map->signal = 0;
        log_struct * logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 4096);
        logblock->gen_num = 0;
    } else {
        superblock->cur_gen_num = 1;
        superblock->block_size = 500000;
        superblock->log_start = 4096;
        superblock->check_sum = superblock->block_size ^ superblock->log_start + 6;
    }

    munmap(superblock,4096);
    close(fd);
}

int Graph::check_point() {
    int fd = open(disk_path, O_RDWR|O_DIRECT);
    super_struct * superblock = (super_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (superblock->cur_gen_num == superblock->block_size) {
        return 507;
    }
    
    // store the whole graph in blocks with size of 255
    uint64_t temp = (uint64_t)2048*1024*1024;
    checkpoint * entry_map = (checkpoint *) mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, temp);
    
    entry_map->signal = 1;
    entry_map->size = 0;
    // initialize: clear previous entry
    for(checkpoint_entry entry: entry_map -> entries) {
        entry.node_a = 0;
        entry.node_b = 0;
    }
  
    
    uint64_t node_a = 0, map_count = 0, entry_count = 0;
    bool empty = false;
    list<list<uint64_t>> :: iterator node_a_it = graph.begin();
    list<uint64_t>::iterator node_b_it = node_a_it->begin();
    while(true) {
        
        entry_map->entries[entry_count].node_a = id_vec[node_a];
        if (empty) {
            entry_map->entries[entry_count].node_b = id_vec[node_a];
            empty = false;
        } else {
            entry_map->entries[entry_count].node_b = *node_b_it;
        }
        
        entry_map->size += 1;
        node_b_it++;
        entry_count++;
        
        if (node_b_it == node_a_it->end()) {
            node_a_it++;
            node_b_it = node_a_it->begin();
            if (node_b_it == node_a_it->end()) {
                empty = true;
            }
            node_a++;
            if (node_a_it == graph.end()) {
                
                if (entry_count == 255) {
                    entry_map->signal = 2;
                }
                printf("node_a_it to end");
                uint64_t temp = (uint64_t)2048*1024*1024 + ++map_count * 4096;
                //set next entry map as no used, so in init stage one can stop there
                entry_map = (checkpoint *) mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, temp);
                entry_map->signal = 0;
                entry_map->size = 0;
                printf("threr");
                
                break;
            }
        }
        
        if (entry_count == 255) {

            uint64_t temp = (uint64_t)2048*1024*1024 + ++map_count * 4096;
            entry_map = (checkpoint *) mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, temp);
            entry_map->size = 0;
            //clear next entry map
            for(checkpoint_entry entry: entry_map -> entries) {
                entry.node_a = 0;
                entry.node_b = 0;
            }
            
            entry_map->signal = 1;
            
            entry_count = 0;
        }
        
    }
    superblock[0].cur_gen_num += 1;
    
    log_struct * logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 4096);
    
    logblock->gen_num = 0;
    logblock->ent_num = 0;
    logblock->check_sum = 0;
    log_tail = 0;
    log_block_count = 1;
    
    munmap(superblock,4096);
    
    munmap(logblock, 4096);
    close(fd);
    
    return 200;
}


int Graph::write_log(uint32_t opcode, uint64_t node_a, uint64_t node_b) {
    int fd = open(disk_path, O_RDWR|O_DIRECT);
    super_struct * superblock = (super_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (superblock->block_size == log_block_count || superblock[0].cur_gen_num == 0) {
        return 507;
    }
    
    log_struct * logblock;
    logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd,log_block_count * 4096);
    
    cout<<"log_block_count_write"<<endl;
    cout<<log_block_count<<endl;
    cout<<"log_block_gen"<<endl;
    cout<<logblock->gen_num<<endl;
    
    log_entry entry;
    entry.action = opcode;
    entry.node_a = node_a;
    entry.node_b = node_b;
    
    cout<<"log tail"<<endl;
    cout<<log_tail<<endl;
    
    logblock->gen_num = superblock->cur_gen_num;
    
    logblock->entry[log_tail] = entry;
    logblock->ent_num += 1;
    logblock->check_sum = logblock->check_sum ^ node_b ^ node_a ^ opcode;
    log_tail += 1;
    
    //clean next log entry
    if (log_tail != 169) {
        logblock->entry[log_tail].action = 0;
    }
    
    //next log block when the current one is full
    if (log_tail >= 170) {
//        cout<<"change log block *****************\n\n\n\n\n\n"<<endl;
        log_tail = 0;
        cout<<log_block_count<<endl;
        log_block_count = log_block_count + 1;
        logblock = (log_struct *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd,log_block_count * 4096);
        cout<<log_block_count<<endl;
        logblock->gen_num = superblock->cur_gen_num;
    }
    
    munmap(superblock,4096);
    munmap(logblock, 4096);
    close(fd);
    return 200;
}


/*

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
*/




