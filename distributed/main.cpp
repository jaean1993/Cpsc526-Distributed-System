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
static uint64_t get_arg(string arg_name) {
    
}
static void ev_handler(struct mg_connection *c, int ev, void *p) {
    if (ev == MG_EV_HTTP_REQUEST) {

        struct http_message *hm = (struct http_message *) p;
        char string_test[hm->uri.len];
        
        strncpy(string_test, hm->uri.p, hm->uri.len);
        
        if (strcmp(string_test, "/api/v1/add_node") == 0) {
            
            int result = graph.add_node(get_arg("node_id"));
            
            mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
            mg_printf(c, "%.*s", hm->message.len, hm->message.p);
        }else if (strcmp(string_test, "/api/v1/remove_node") == 0) {
            int result = graph.remove_node(get_arg("node_id"));
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

