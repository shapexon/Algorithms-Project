/*
 * RaceCarDriver.h
 *
 *  Created on: Spring, 2026
 *      Author: bill_booth
 */

#ifndef RACECARDRIVER_H_
#define RACECARDRIVER_H_

#include "Racer.h"
#include <vector>
#include <unordered_set>
#include <functional>
#include <list>
#include <stack>
#include <algorithm>
using namespace std;

class RaceCarDriver{
private:
	Racer* car;
	int x = 0;
	int y = 0;
	int runOn = 0;
	int finishX = -99;
	int finishY = -99;
	int endX;
	int endY;
	int startX;
	int startY;
	bool secondDone = false;
	stack<DIRECTION> pathInstructions;
	stack<DIRECTION> pathInstructionsCopy;

	enum Color {
		WHITE,
		GREY,
		BLACK,
	};

	struct BFSNode {
		Color color;
		int x;
		int y;
		BFSNode* parent;
		int cost;
		vector<DIRECTION> adjVec;
		BFSNode(int xx, int yy, vector<DIRECTION> adj) {
			color = WHITE;
			x = xx;
			y = yy;
			cost = 0;
			parent = nullptr;
			adjVec = adj;
		}
		bool operator==(const BFSNode& other) const {
			return (x == other.x && y == other.y);
		}
	};

	BFSNode* nodeAtBFS(list<BFSNode>& BFSNodeList, int x, int y) {
		vector<DIRECTION> v;
		BFSNode n(x, y, v);
		auto it = find(BFSNodeList.begin(), BFSNodeList.end(), n);
		if (it != BFSNodeList.end()) {
			return const_cast<BFSNode*>(&(*it));
		}
		return nullptr;
	}



	// Node for DFS searching
	struct Node {
		Node* parent;
		int x;
		int y;
		vector<DIRECTION> adjVec;
		DIRECTION parentDirection;
		Node(int xx, int yy, Node* p, DIRECTION d) {
			x = xx;
			y = yy;
			parentDirection = d;
			parent = p;
			adjVec.push_back(d);
		}
		bool operator==(const Node& other) const {
			return (x == other.x && y == other.y);
		}
		
	};


	Node* nodeAt(list<Node>& nodeList, int x, int y) {
		Node n(x, y, nullptr, EAST);
		auto it = find(nodeList.begin(), nodeList.end(), n);
		if (it != nodeList.end()) {
			return const_cast<Node*>(&(*it));
		}
		return nullptr;
	}

	list<Node> nodeList; 


public:
	RaceCarDriver(Racer* p = nullptr): car{p}{
	};

	DIRECTION nextMove() {


		// In djrksta mode, just follow the path!
		if (runOn == 3) {
			DIRECTION d =  pathInstructions.top();
			if (d == EAST) x++;
			if (d == WEST) x--;
			if (d == NORTH) y--;
			if (d == SOUTH) y++;
			finishX = x;
			finishY = y;
			pathInstructions.pop();
			return d;
		}
		// Use car location to get which run we are on:
		point p = car->getLocation();
		if (runOn == 0) {
			startX = p.x;
			startY = p.y;
		}
		

		// reset x and y to startX
		if (p.x == startX && p.y == startY) {
			x = 0;
			y = 0;
		}


		// Move to the next mode based on which run:
		if (x == 0 && y == 0
		&& (finishX > 1 || finishX < -1)
		&& (finishY > 1 || finishY < -1)) {
			runOn ++;
			// Set up round 2!!!!
			if (runOn == 2) {
				endX = finishX;
				endY = finishY;
				Node* f = nodeAt(nodeList, endX, endY);
				Node finishNode(f->x, f->y, f->parent, f->parentDirection);
				finishNode.adjVec = f->adjVec;
				nodeList.clear();
				nodeList.push_back(finishNode);
			}
			if (runOn == 3) {
				
			}
 		}



		// In djrksta mode, just follow the path!
		if (secondDone) {
			DIRECTION d =  pathInstructions.top();
			if (d == EAST) x++;
			if (d == WEST) x--;
			if (d == NORTH) y--;
			if (d == SOUTH) y++;
			finishX = x;
			finishY = y;
			pathInstructions.pop();
			if (pathInstructions.size() == 0) {
				pathInstructions = pathInstructionsCopy;
			}
			return d;
		}


		// For first spot make a new node:
		Node* currentNode = nodeAt(nodeList,x,y);
		if (currentNode == nullptr) {
			Node thisNode(x, y, nullptr, EAST);
			nodeList.push_back(thisNode);
			currentNode = &thisNode;
		}


		// Test the different directions in depth first search mode:
		if(!car->look(EAST)) {
			if (nodeAt(nodeList,x+1,y) == nullptr
			&& !(x+1 == endX && y == endY)) {
				x ++;
				finishX = x;
				finishY = y;
				currentNode->adjVec.push_back(EAST);
				Node newNode(x, y, currentNode, WEST);
				nodeList.push_back(newNode);
				return EAST;
			}
		}
		if(!car->look(SOUTH)) {
			if (nodeAt(nodeList,x,y+1) == nullptr
			&& !(x == endX && y+1 == endY)) {
				y ++;
				finishX = x;
				finishY = y;
				currentNode->adjVec.push_back(SOUTH);
				Node newNode(x, y, currentNode, NORTH);
				nodeList.push_back(newNode);
				return SOUTH;
			}
		}
		if(!car->look(NORTH)) {
			if (nodeAt(nodeList,x,y-1) == nullptr
			&& !(x == endX && y-1 == endY)) {
				y --;
				finishX = x;
				finishY = y;
				currentNode->adjVec.push_back(NORTH);
				Node newNode(x, y, currentNode, SOUTH);
				nodeList.push_back(newNode);
				return NORTH;
			}
		}
		if(!car->look(WEST))   {
			if (nodeAt(nodeList,x-1,y) == nullptr
			&& !(x-1 == endX && y == endY)) {
				x --;
				finishX = x;
				finishY = y;
				currentNode->adjVec.push_back(WEST);
				Node newNode(x, y, currentNode, EAST);
				nodeList.push_back(newNode);
				return WEST;
			}
		}

		// We walked back to the start, therefore pathfind!
		if (x == 0 && y == 0) {
			secondDone = true;
			list<BFSNode> BFSNodeList;
			Node* finishNode = nodeAt(nodeList, endX, endY);
			if (find(finishNode->adjVec.begin(), finishNode->adjVec.end(), EAST) != finishNode->adjVec.end()) {
				Node* other = nodeAt(nodeList, endX+1, endY);
				other->adjVec.push_back(WEST);
			}
			if (find(finishNode->adjVec.begin(), finishNode->adjVec.end(), WEST) != finishNode->adjVec.end()) {
				Node* other = nodeAt(nodeList, endX-1, endY);
				other->adjVec.push_back(EAST);
			}
			if (find(finishNode->adjVec.begin(), finishNode->adjVec.end(), NORTH) != finishNode->adjVec.end()) {
				Node* other = nodeAt(nodeList, endX, endY-1);
				other->adjVec.push_back(SOUTH);
			}
			if (find(finishNode->adjVec.begin(), finishNode->adjVec.end(), SOUTH) != finishNode->adjVec.end()) {
				Node* other = nodeAt(nodeList, endX, endY+1);
				other->adjVec.push_back(NORTH);
			}


			for (Node n : nodeList) {
				BFSNode newNode(n.x, n.y, n.adjVec);
				BFSNodeList.push_back(newNode);
			}
			BFSNode* s = nodeAtBFS(BFSNodeList, 0, 0);
			s->color = WHITE;
			s->cost = 0;
			queue<BFSNode*> q;
			q.push(s);

			while (!q.empty()) {
				BFSNode* current = q.front();
				q.pop();
				// If we found the end, nice job! We win these:
				if (current->x == endX && current->y == endY) {
					while (current->parent != nullptr) {
						if (current->x < current->parent->x) {
							pathInstructions.push(WEST);
						}
						if (current->x > current->parent->x) {
							pathInstructions.push(EAST);
						}
						if (current->y < current->parent->y) {
							pathInstructions.push(NORTH);
						}
						if (current->y > current->parent->y) {
							pathInstructions.push(SOUTH);
						}
						current = current->parent;
					}
					break;
				}


				// Check all neighbors
				BFSNode* adjNode = nodeAtBFS(BFSNodeList, current->x+1, current->y);
				if (adjNode != nullptr && adjNode->color == WHITE) {
					if (find(current->adjVec.begin(), current->adjVec.end(), EAST) != current->adjVec.end()) {
						adjNode->color = GREY;
						adjNode->cost = current->cost+1;
						adjNode->parent = current;
						q.push(adjNode);
					}
				}
				adjNode = nodeAtBFS(BFSNodeList, current->x-1, current->y);
				if (adjNode != nullptr && adjNode->color == WHITE) {
					if (find(current->adjVec.begin(), current->adjVec.end(), WEST) != current->adjVec.end()) {
						adjNode->color = GREY;
						adjNode->cost = current->cost+1;
						adjNode->parent = current;
						q.push(adjNode);
					}
				}

				adjNode = nodeAtBFS(BFSNodeList, current->x, current->y-1);
				if (adjNode != nullptr && adjNode->color == WHITE) {
					if (find(current->adjVec.begin(), current->adjVec.end(), NORTH) != current->adjVec.end()) {
						adjNode->color = GREY;
						adjNode->cost = current->cost+1;
						adjNode->parent = current;
						q.push(adjNode);
					}
				}
				adjNode = nodeAtBFS(BFSNodeList, current->x, current->y+1);
				if (adjNode != nullptr && adjNode->color == WHITE) {
					if (find(current->adjVec.begin(), current->adjVec.end(), SOUTH) != current->adjVec.end()) {
						adjNode->color = GREY;
						adjNode->cost = current->cost+1;
						adjNode->parent = current;
						q.push(adjNode);
					}
				}
			

				current->color = BLACK;
			}

			pathInstructionsCopy = pathInstructions;
			DIRECTION d =  pathInstructions.top();
			pathInstructions.pop();
			return d;
		}



		// Walk back for depth first search!
		if (currentNode->parentDirection == EAST) {
			x += 1;
		}
		else if (currentNode->parentDirection == WEST) {
			x -= 1;
		}
		else if (currentNode->parentDirection == NORTH) {
			y --;
		}
		else if (currentNode->parentDirection == SOUTH) {
			y++;
		}
		finishX = x;
		finishY = y;
		return currentNode->parentDirection;
	}

};


#endif /* RACECARDRIVER_H_ */