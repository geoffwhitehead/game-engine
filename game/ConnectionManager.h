#pragma once
#include "../engine-base/Entity.h"
#include <vector>
#include <algorithm>

class Connector;
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
	static vector<Connector*>*  ConnectionManager::getEdges(Node* n);
};

