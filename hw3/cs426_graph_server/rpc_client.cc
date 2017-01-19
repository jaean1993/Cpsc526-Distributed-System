#include <iostream>
#include <memory>
#include <string>
#include <stdint.h>

#include <grpc++/grpc++.h>
#include "rpc_client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using rpc::Request;
using rpc::Reply;
using rpc::Replication;

using namespace std;

RPCClient::RPCClient(std::shared_ptr<Channel> channel): stub_(Replication::NewStub(channel)) {};
  // Assambles the client's payload, sends it and presents the response back
  // from the server.
bool RPCClient::RpcGraph(int action, uint64_t node_one, uint64_t node_two) {
  // Data we are sending to the server.
  Request request;
  request.set_action(action);
  request.set_node_one(node_one);
  request.set_node_two(node_two);

  // Container for the data we expect from the server.
  Reply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->RpcGraph(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}
