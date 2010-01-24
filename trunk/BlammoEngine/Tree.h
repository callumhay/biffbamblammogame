#ifndef __TREE_H__
#define __TREE_H__

#include "Collision.h"

class QuadTree {

public:

	struct QuadTreeNode {
		enum ChildIndex { TopLeft = 0, TopRight = 1, BottomLeft = 2, BottomRight = 3 };
		
		Collision::AABB2D bounds;
		QuadTreeNode* children[4];
		
		QuadTreeNode(const Collision::AABB2D bounds) : bounds(bounds) {
			for (size_t i = 0; i < 4; i++) {
				this->children[i] = NULL;
			}
		}
		
		~QuadTreeNode() {
			for (size_t i = 0; i < 4; i++) {
				if (this->children[i] != NULL) {
					delete this->children[i];
					this->children[i] = NULL;
				}
			}
		}

		bool HasChildren() const {
			return (this->children[0] == NULL || this->children[1] == NULL || this->children[2] == NULL || this->children[3] == NULL);
		}

	};

	~QuadTree() {
		if (this->root != NULL) {
			delete this->root;
		}
	}

	QuadTree(const Collision::AABB2D& maxSizeBounds, const Vector2D& minDimensions);
	std::list<const QuadTree::QuadTreeNode*> GetCollidingNodes(const Collision::Ray2D& ray) const;

private:
	
	QuadTreeNode* root;

};

#endif