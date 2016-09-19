//
//  main.cpp
//  distributed
//
//  Created by 安槿 on 16/9/7.
//  Copyright © 2016年 anjin. All rights reserved.
//

#include "mongoose.h"  // Include Mongoose API definitions
#include "Graph.hpp"
#include <string>
#include <iostream>
#include <stdlib.h>
using namespace std;

static Graph graph;


static uint64_t get_node(struct json_token *tokens, string node_name) {
    struct json_token *node;
    //Find the json_token named node_id
    node = find_json_token(tokens, node_name.c_str());
    
    char *temp = (char*)malloc(node -> len * sizeof(char));
    strncpy(temp, node -> ptr, node -> len);
    uint64_t node_id = atoi(temp);
    free(temp);
    return node_id;
}

static void ev_handler(struct mg_connection *c, int ev, void *p) {
    if ( ev == MG_EV_HTTP_REQUEST ) {

        struct http_message *hm = (struct http_message*) p;
    
        //Parse http_message body with type json
        struct json_token tokens[(int)hm -> body.len];
        parse_json(hm -> body.p, (int)hm -> body.len, tokens, (int)hm -> body.len);
        
        int status_code = 200; //Default value
        
        if (mg_vcmp(&hm -> method, "POST") != 0) {
            string response = "Wrong command!\r\n";
            mg_send_head(c, 400, response.size(), "Content-Type:plain/text");
            mg_printf(c, response.c_str());
        }
        if (mg_vcmp(&hm -> uri, "/api/v1/add_node") == 0) {
            
            uint64_t node_id = get_node(tokens, "node_id");
            
            //Try to add a node
            status_code = graph.add_node(node_id);
            switch (status_code) {
                case 200: //Add the node successfully
                {
                    string json_result = "{\r\nnode_id:"+std::to_string(node_id)+"\r\n}\r\n";
                    mg_send_head(c, status_code, json_result.size(), "Content-Type: application/json");
                    mg_printf(c, "%s", json_result.c_str());
                    break;
                }
                case 204: //The node already exists
                {
                    mg_send_head(c, status_code, 0, NULL);
                    break;
                }
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/add_edge") == 0) {
            //Exstract node_a_id and node_b_id
            uint64_t node_a_id = get_node(tokens,"node_a_id");
            uint64_t node_b_id = get_node(tokens,"node_b_id");
            
            //Try to add an edge
            status_code = graph.add_edge(node_a_id, node_b_id);
            switch (status_code) {
                case 200: //Add the edge successfully
                {
                    string json_result = "{\r\nnode_a_id:"+std::to_string(node_a_id)+"\r\nnode_b_id:"+std::to_string(node_b_id)+"\r\n}\r\n";
                    mg_send_head(c, status_code, json_result.size(), "Content-Type:application/json");
                    mg_printf(c, "%s", json_result.c_str());
                    
                    break;
                }
                case 204: //The edge already exists
                    mg_send_head(c, status_code, 0, NULL);
                    break;
                case 400: //Either node doesn't exist, or if node_a_id is the same as node_b_id
                    mg_send_head(c, status_code, 0, NULL);
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/remove_node") == 0) {
            uint64_t node_id = get_node(tokens,"node_id");
            
            //Try to remove a node
            status_code = graph.remove_node(node_id);
            switch (status_code) {
                case 200: //Remove the node successfully
                    mg_send_head(c, status_code, 0, NULL);
                    break;
                case 400: //The node does not exist
                    mg_send_head(c, status_code, 0, NULL);
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/remove_edge") == 0) {
            uint64_t node_a_id = get_node(tokens,"node_a_id");
            uint64_t node_b_id = get_node(tokens,"node_b_id");
            
            //Try to remove an edge
            status_code = graph.remove_edge(node_a_id, node_b_id);
            switch (status_code) {
                case 200: //Add the edge successfully
                    mg_send_head(c, status_code, 0, NULL);
                    break;
                case 400: //The edge does not exist
                    mg_send_head(c, status_code, 0, NULL);
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/get_node") == 0) {

            uint64_t node_id = get_node(tokens,"node_id");
            
            //Try to get the node
            int in_graph = graph.get_node(node_id);
            string json_result = "{\r\nin_graph:"+std::to_string(in_graph)+"\r\n}\r\n";
            mg_send_head(c, status_code, json_result.size(), "Content-Type:application/json");
            mg_printf(c, "%s", json_result.c_str());
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/get_edge") == 0) {
            uint64_t node_a_id = get_node(tokens,"node_a_id");
            uint64_t node_b_id = get_node(tokens,"node_b_id");
            
            //Try to add an edge
            int myflag = graph.get_edge(node_a_id, node_b_id);
            switch (myflag) {
                case 0: //The edge does not exist
                    mg_send_head(c, 400, 0, NULL);
                    break;
                case 1: //The edge exists
                {
                    status_code = 200;
                    string json_result = "{\r\nin_graph:"+std::to_string(myflag)+"\r\n}\r\n";
                    mg_send_head(c, status_code, json_result.size(), "Content-Type:application/json");
                    mg_printf(c, "%s", json_result.c_str());
                    break;
                }
                case -1: //Either node doesn't exist, or node_a_id is the same as node_b_id
                    mg_send_head(c, 400, 0, NULL);
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/get_neighbors") == 0) {
            
            
            //Exstract node_id
            uint64_t node_id = get_node(tokens,"node_id");
            
            if(graph.get_node(node_id) == 0) { //If the node does not exist
                mg_send_head(c, 400, 0, NULL);
            }
            else {
                //Get the neighbors
                list<uint64_t> neighbors = graph.get_neighbors(node_id);
                
                if (!neighbors.empty()) {
                    list<uint64_t>::iterator it = neighbors.begin();
                    string json_result = "\r\n{\n\"node_id\":" + std::to_string(node_id) + ",\n\"neighbors\":[";
                    json_result += std::to_string(*it);
                    for (it++; it != neighbors.end(); it++) {
                        json_result += ",";
                        json_result += std::to_string(*it);
                    }
                    json_result += "]\r\n}\r\n";
                    mg_send_head(c, status_code, json_result.size(), "Content-Type:plain/text");
                    mg_printf(c, "%s", json_result.c_str());
                }else {
                    mg_send_head(c, 400, 0, NULL);
                }
                
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/shortest_path") == 0) {
            uint64_t node_a_id = get_node(tokens,"node_a_id");
            uint64_t node_b_id = get_node(tokens,"node_b_id");
            
            //Try to find the shortest path
            int dis = graph.shortest_path(node_a_id, node_b_id);
            switch (dis) {
                case -1:
                    status_code = 400;
                    mg_send_head(c, status_code, 0, NULL);
                    break;
                case -2:
                    status_code = 204;
                    mg_send_head(c, status_code, 0, NULL);
                    break;
                default:
                    status_code = 200;
                    string json_result = "{\r\n\"distance\":"+std::to_string(dis)+"\r\n}\r\n";
                    mg_send_head(c, status_code, json_result.size(), "Content-Type:application/json");
                    mg_printf(c, "%s", json_result.c_str());
                    break;
                    
            }

        }
    }
}


int main(int argc, char *argv[]) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    char *s_http_port = "8000";
    if(argc > 1) {
        s_http_port =  argv[1];
    }
    mg_mgr_init(&mgr, NULL);
    c = mg_bind(&mgr, s_http_port, ev_handler);
    mg_set_protocol_http_websocket(c);
    
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
    
    return 0;
}

