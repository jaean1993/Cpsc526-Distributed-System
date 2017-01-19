#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "rpc.grpc.pb.h"
#include "rpc_server.h"
#include "helper.h"
#include "Graph.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using rpc::Request;
using rpc::Reply;
using rpc::Replication;


// Logic and data behind the server's behavior.
class GraphServiceImpl final : public Replication::Service {
  Status RpcGraph(ServerContext* context, const Request* request,
                    Reply* reply) override {
    int action = request->action();

    if (action == 1)
    {
      Graph::add_node(request->node_one());
    } else if (action == 2)
    {
      Graph::remove_node(request->node_one());
    } else if (action == 3)
    {
      Graph::add_edge(request->node_one(), request->node_two());
    } else if (action == 4)
    {
      Graph::remove_edge(request->node_one(), request->node_two());
    }

    if (Helper::rpcClient != NULL && !Helper::rpcClient->RpcGraph(request->action(), request->node_one(), request->node_two())) {
      return Status::CANCELLED;
    }

    return Status::OK;
  }
};

void* RPCServer::RunServer(void* n) {
  std::string server_address("0.0.0.0:50051");
  GraphServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
  server->Wait();
}
