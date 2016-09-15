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

static const char *s_http_port = "8000";
static Graph graph;

static void ev_handler(struct mg_connection* c, int ev, void *p) {
    if ( ev == MG_EV_HTTP_REQUEST ) {
        struct http_message* hm = (struct http_message*) p;
        if ( strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/add_node") == 0) {
        	mg_rpc_create_reply(char *buf, int len, const struct mg_rpc_request *req);

            if ( ) {
                mg_send_head(c, 200, hm->message.len, "Content-Type: application/json");
            }
            else {
               mg_send_head(c, 400, hm->message.len, "Content-Type: application/json");
            }
            mg_printf(c, "%.*s", hm->message.len, hm->message.p);
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/remove_node") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/add_edge") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/remove_edge") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/get_node") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/get_edge") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/get_neighbors") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
        } else if (strcmp(hm->method.p, "POST") == 0 && strcmp(hm->uri.p, "/api/v1/shortest_path") == 0) {
//            int result = graph.remove_node(get_arg(hm->body,"node_id"));
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

