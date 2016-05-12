#include "ConnectionManager.h"
 

ConnectionManager::ConnectionManager() {

}

vector < pair<Node*, vector<Connector*>>> ConnectionManager::edges;


ConnectionManager::~ConnectionManager() {
}

void ConnectionManager::addConnector(Node* node, Connector* con) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].first == node ) {
			edges[i].second.push_back(con);
		}
	}
}
Connector* ConnectionManager::getLastConnection(Node*n) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].first == n) {
			Connector* latest = edges[i].second[0];
			for (int j = 1; j < edges[i].second.size(); j++) {
				if (edges[i].second[j]->created_on > latest->created_on) {
					latest = edges[i].second[j];
				}
			}
			return latest;
		}
	}
}

void ConnectionManager::removeConnector(Node* node, Connector* con) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].first == node) {
			edges[i].second.erase(std::remove(edges[i].second.begin(), edges[i].second.end(), con), edges[i].second.end());
		}
	}
}
void ConnectionManager::removeNode(Node* node) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].first == node) {
			edges.erase(std::remove(edges.begin(), edges.end(), edges[i]), edges.end());
		}
	}
}

bool ConnectionManager::isNodeConnected(Node* n) {
	if (getEdges(n)->size() == 0) {
		return false;
	}
	return true;
}

vector<Connector*>* ConnectionManager::getEdges(Node* n) {
	for (int i = 0; i < edges.size(); i++) {

		if (edges[i].first == n) {
			return &(edges[i].second);
		}

	}
}

void ConnectionManager::addNode(Node* n) {
	bool found = false;
	for (int i = 0; i < edges.size(); i++) {
		
		if (edges[i].first == n) {
			found == true;
		}

	}
	if (!found) {
		vector<Connector*> cons;
		edges.push_back(pair<Node*, vector<Connector*>>(n, cons));
	}
	else {
		cout << "couldnt find node in connection manager" << endl;
	}
}