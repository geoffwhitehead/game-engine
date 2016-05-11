#pragma once
#include "../engine-base/Entity.h"
#include <vector>
#include <algorithm>
#include "Connector.h"

class Node;

using namespace std;
class ConnectionManager
{
public:
	ConnectionManager();
	~ConnectionManager();
	static vector < pair<Node*, vector<Connector*>>> edges;
	static void removeNode(Node* node);
	static void removeConnector(Node* node, Connector* con);
	static void addConnector(Node*, Connector*);
	static void addNode(Node*);
	static vector<Connector*>*  getEdges(Node* n);
	static Connector* getLastConnection(Node*n);
	static bool isNodeConnected(Node* n);
};

