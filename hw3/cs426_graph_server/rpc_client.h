#ifndef RPC_CLIENT_H_
#define RPC_CLIENT_H_

#include "rpc.grpc.pb.h"

using grpc::Channel;
using rpc::Replication;

class RPCClient {
	std::unique_ptr<Replication::Stub> stub_;
public:
	RPCClient(std::shared_ptr<Channel> channel);
	bool RpcGraph(int action, uint64_t node_one, uint64_t node_two);
};

#endif
