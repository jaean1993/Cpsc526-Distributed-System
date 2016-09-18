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

static const char *s_http_port = "8000";
static Graph graph;

static void ev_handler(struct mg_connection *c, int ev, void *p) {
    if ( ev == MG_EV_HTTP_REQUEST ) {

        struct http_message *hm = (struct http_message*) p;
    
        //Parse http_message body with type json
        struct json_token tokens[100];
        parse_json(hm -> body.p, (int)hm -> body.len, tokens, 100);
        
        int status_code = 200; //Default value
        const char *status_message = "OK"; //Default value
        
        if (mg_vcmp(&hm -> method, "POST") != 0) {
            mg_send(c, "\r\n", 2);
            mg_printf(c, "Wrong command!\r\n");
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/add_node") == 0) {
            struct json_token *node;
            //Find the json_token named node_id
            node = find_json_token(tokens, "node_id");

            //Exstract node_id
            char *temp = (char*)malloc(node -> len * sizeof(char));
            strncpy(temp, node -> ptr, node -> len);
            uint64_t node_id = atoi(temp);
            free(temp);
            
            //Try to add a node
            status_code = graph.add_node(node_id);
            switch (status_code) {
                case 200: //Add the node successfully
                    //Print the output
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, " node_id: %llu\r\n", node_id);
                    mg_printf(c, "}\r\n");
                    break;
                case 204: //The node already exists
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/add_edge") == 0) {
            struct json_token *node_a, *node_b;
            //Find the json_token named node_a_id
            node_a = find_json_token(tokens, "node_a_id");
            node_b = find_json_token(tokens, "node_b_id");
            
            //Exstract node_a_id and node_b_id
            char* temp = (char*)malloc(node_a -> len * sizeof(char));
            strncpy(temp, node_a -> ptr, node_a -> len);
            uint64_t node_a_id = atoi(temp);
            strncpy(temp, node_b -> ptr, node_b -> len);
            uint64_t node_b_id = atoi(temp);
            free(temp);
            
            //Try to add an edge
            status_code = graph.add_edge(node_a_id, node_b_id);
            switch (status_code) {
                case 200: //Add the edge successfully
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, " node_a_id: %llu\r\n", node_a_id);
                    mg_printf(c, " node_b_id: %llu\r\n", node_b_id);
                    mg_printf(c, "}\r\n");
                    break;
                case 204: //The edge already exists
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    break;
                case 400: //Either node doesn't exist, or if node_a_id is the same as node_b_id
                    status_message = "Bad Request";
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "Either node doesn't exist, or if node_a_id is the same as node_b_id.\r\n");
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/remove_node") == 0) {
            struct json_token *node;
            //Find the json_token named node_id
            node = find_json_token(tokens, "node_id");
            
            //Exstract node_id
            char *temp = (char*)malloc(node -> len * sizeof(char));
            strncpy(temp, node -> ptr, node -> len);
            uint64_t node_id = atoi(temp);
            free(temp);
            
            //Try to remove a node
            status_code = graph.remove_node(node_id);
            switch (status_code) {
                case 200: //Remove the node successfully
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, " node_id: %llu\r\n", node_id);
                    mg_printf(c, "}\r\n");
                    break;
                case 400: //The node does not exist
                    status_message = "Bad Request";
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "The node does not exist.\r\n");
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/remove_edge") == 0) {
            struct json_token *node_a, *node_b;
            //Find the json_token named node_a_id
            node_a = find_json_token(tokens, "node_a_id");
            node_b = find_json_token(tokens, "node_b_id");
            
            //Exstract node_a_id and node_b_id
            char* temp = (char*)malloc(node_a -> len * sizeof(char));
            strncpy(temp, node_a -> ptr, node_a -> len);
            uint64_t node_a_id = atoi(temp);
            strncpy(temp, node_b -> ptr, node_b -> len);
            uint64_t node_b_id = atoi(temp);
            free(temp);
            
            //Try to remove an edge
            status_code = graph.remove_edge(node_a_id, node_b_id);
            switch (status_code) {
                case 200: //Add the edge successfully
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, " node_a_id: %llu\r\n", node_a_id);
                    mg_printf(c, " node_b_id: %llu\r\n", node_b_id);
                    mg_printf(c, "}\r\n");
                    break;
                case 400: //The edge does not exist
                    status_message = "Bad Request";
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "The edge does not exist\r\n");
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/get_node") == 0) {
            struct json_token *node;
            //Find the json_token named node_id
            node = find_json_token(tokens, "node_id");
            
            //Exstract node_id
            char *temp = (char*)malloc(node -> len * sizeof(char));
            strncpy(temp, node -> ptr, node -> len);
            uint64_t node_id = atoi(temp);
            free(temp);
            
            //Try to get the node
            int in_graph = graph.get_node(node_id);
            mg_send(c, "\r\n", 2);
            mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
            mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
            mg_printf(c, "Application-Type: %s\r\n", "application/json");
            mg_send(c, "\r\n", 2);
            mg_printf(c, "{\r\n");
            mg_printf(c, " in_graph: %d\r\n", in_graph);
            mg_printf(c, "}\r\n");
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/get_edge") == 0) {
            struct json_token *node_a, *node_b;
            //Find the json_token named node_a_id
            node_a = find_json_token(tokens, "node_a_id");
            node_b = find_json_token(tokens, "node_b_id");
            
            //Exstract node_a_id and node_b_id
            char* temp = (char*)malloc(node_a -> len * sizeof(char));
            strncpy(temp, node_a -> ptr, node_a -> len);
            uint64_t node_a_id = atoi(temp);
            strncpy(temp, node_b -> ptr, node_b -> len);
            uint64_t node_b_id = atoi(temp);
            free(temp);
            
            //Try to add an edge
            int myflag = graph.get_edge(node_a_id, node_b_id);
            switch (myflag) {
                case 0: //The edge does not exist
                case 1: //The edge exists
                    status_code = 200;
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, " in_graph: %d\r\n", myflag);
                    mg_printf(c, "}\r\n");
                    break;
                case -1: //Either node doesn't exist, or node_a_id is the same as node_b_id
                    status_code = 400;
                    status_message = "Bad Request";
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "Either node doesn't exist, or node_a_id is the same as node_b_id.\r\n");
                    break;
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/get_neighbors") == 0) {
            struct json_token *node;
            //Find the json_token named node_id
            node = find_json_token(tokens, "node_id");
            
            //Exstract node_id
            char *temp = (char*)malloc(node -> len * sizeof(char));
            strncpy(temp, node -> ptr, node -> len);
            uint64_t node_id = atoi(temp);
            free(temp);
            
            if(graph.get_node(node_id) == 0) { //If the node does not exist
                status_code = 400;
                mg_send(c, "\r\n", 2);
                mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                mg_printf(c, "Application-Type: %s\r\n", "application/json");
                mg_send(c, "\r\n", 2);
                mg_printf(c, "{\r\n");
                mg_printf(c, "The node does not exist.\r\n");
                mg_printf(c, "}\r\n");
            }
            else {
                status_message = "Bad Request";
                //Get the neighbors
                list<uint64_t> neighbors = graph.get_neighbors(node_id);
                mg_send(c, "\r\n", 2);
                mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                mg_printf(c, "Application-Type: %s\r\n", "application/json");
                mg_send(c, "\r\n", 2);
                mg_printf(c, "{\r\n");
                mg_printf(c, " \"node_id\": %llu\r\n", node_id);
                mg_printf(c, " \"neighbors\": [");
                if (!neighbors.empty()) {
                    list<uint64_t>::iterator it = neighbors.begin();
                    mg_printf(c, "%llu", *it);
                    for (it++; it != neighbors.end(); it++) {
                        mg_printf(c, ", %llu", *it);
                    }
                }
                mg_printf(c, "]\r\n");
                mg_printf(c, "}\r\n");
            }
        }
        else if (mg_vcmp(&hm -> uri, "/api/v1/shortest_path") == 0) {
            struct json_token *node_a, *node_b;
            //Find the json_token named node_a_id
            node_a = find_json_token(tokens, "node_a_id");
            node_b = find_json_token(tokens, "node_b_id");
            
            //Exstract node_a_id and node_b_id
            char* temp = (char*)malloc(node_a -> len * sizeof(char));
            strncpy(temp, node_a -> ptr, node_a -> len);
            uint64_t node_a_id = atoi(temp);
            strncpy(temp, node_b -> ptr, node_b -> len);
            uint64_t node_b_id = atoi(temp);
            free(temp);
            
            //Try to find the shortest path
            int dis = graph.shortest_path(node_a_id, node_b_id);
            switch (dis) {
                case -1:
                    status_code = 400;
                    status_message = "Bad Request";
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, "Either node does not exist.\r\n");
                    mg_printf(c, "}\r\n");
                    break;
                case -2:
                    status_code = 204;
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, "There is no path.\r\n");
                    mg_printf(c, "}\r\n");
                    break;
                default:
                    status_code = 200;
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "%.*s %d %s\r\n", hm -> proto.len, hm -> proto.p, status_code, status_message);
                    mg_printf(c,"Content-Length: %ld\r\n", hm -> message.len);
                    mg_printf(c, "Application-Type: %s\r\n", "application/json");
                    mg_send(c, "\r\n", 2);
                    mg_printf(c, "{\r\n");
                    mg_printf(c, "The shortest path is %d\r\n", dis);
                    mg_printf(c, "}\r\n");
                    break;
                    
            }

        }
    }
}

int main(void) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    
    mg_mgr_init(&mgr, NULL);
    c = mg_bind(&mgr, s_http_port, ev_handler);
    mg_set_protocol_http_websocket(c);
    
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
    
    return 0;
}

