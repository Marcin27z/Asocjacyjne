#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>

namespace aisdi {

	template<typename KeyType, typename ValueType>
	class TreeMap {
	public:
		using key_type = KeyType;
		using mapped_type = ValueType;
		using value_type = std::pair<const key_type, mapped_type>;
		using size_type = std::size_t;
		using reference = value_type &;
		using const_reference = const value_type &;

		class ConstIterator;

		class Iterator;

		using iterator = Iterator;
		using const_iterator = ConstIterator;
	private:

		class Node {
		public:
			value_type value;
			bool color = 0; // 0 = black, 1 = red
			Node *parent, *left, *right;

			Node() : parent(nullptr), left(nullptr), right(nullptr) {}

			Node(const key_type &key) : value(std::make_pair(key, ValueType())), parent(nullptr), left(nullptr),
																	right(nullptr) {}

			Node(const Node *node) : value(std::make_pair(node->value.first, node->value.second)), color(node->color),
															 parent(node->parent), left(node->left), right(node->right) {}
		};

		size_type size = 0;
		Node *root = nullptr, *min = nullptr;
		Node sentinel;

		void deleteTree(Node *node) {
			if (node == nullptr)
				return;
			deleteTree(node->left);
			deleteTree(node->right);
			delete (node);
		}

		void copyInOrder(Node *node, Node *copyTreeNode) {
			if (node != nullptr) {
				if (node->left != nullptr) {
					copyTreeNode->left = new Node(node->left);
					copyInOrder(node->left, copyTreeNode->left);
				}
				if (node->right != nullptr) {
					copyTreeNode->right = new Node(node->right);
					copyInOrder(node->right, copyTreeNode->right);
				}
			}
		}

		void rotateLeft(Node *x) {
			if (x->right == nullptr)
				return;
			auto y = x->right;
			x->right = y->left;
			if (y->left != nullptr)
				y->left->parent = x;
			y->parent = x->parent;
			if (x->parent == &sentinel) {
				root = y;
				sentinel.right = root;
				root->parent = &sentinel;
			} else if (x == x->parent->left)
				x->parent->left = y;
			else
				x->parent->right = y;
			y->left = x;
			x->parent = y;
		}

		void rotateRight(Node *x) {
			if (x->left == nullptr)
				return;
			auto y = x->left;
			x->left = y->right;
			if (y->right != nullptr)
				y->right->parent = x;
			y->parent = x->parent;
			if (x->parent == &sentinel) {
				root = y;
				sentinel.right = root;
				root->parent = &sentinel;
			} else if (x == x->parent->right)
				x->parent->right = y;
			else
				x->parent->left = y;
			y->right = x;
			x->parent = y;
		}

		void insertFixup(Node *z) {
			auto y = z;
			while (z->parent->color == 1) {
				if (z->parent == z->parent->parent->left) {
					y = z->parent->parent->right;
					if ((y != nullptr) && (y->color == 1)) {
						z->parent->color = 0;
						y->color = 0;
						z->parent->parent->color = 1;
						z = z->parent->parent;
					} else {
						if (z == z->parent->right) {
							z = z->parent;
							rotateLeft(z);
						}
						z->parent->color = 0;
						z->parent->parent->color = 1;
						rotateRight(z->parent->parent);
					}
				} else {
					y = z->parent->parent->left;
					if ((y != nullptr) && (y->color == 1)) {
						z->parent->color = 0;
						y->color = 0;
						z->parent->parent->color = 1;
						z = z->parent->parent;
					} else {
						if (z == z->parent->left) {
							z = z->parent;
							rotateRight(z);
						}
						z->parent->color = 0;
						z->parent->parent->color = 1;
						rotateLeft(z->parent->parent);
					}
				}
			}
			root->color = 0;
		}

		void transplant(Node *u, Node *v) {
			if (u->parent == &sentinel) {
				root = v;
				root->parent = &sentinel;
				sentinel.right = root;
			} else if (u == u->parent->left)
				u->parent->left = v;
			else u->parent->right = v;
			v->parent = u->parent;
		}

		void removeFixup(Node *x) {
			Node * w;
			while ((x != root) && (x->color == 0)){
				if (x == x->parent->left){
					w = x->parent->right;
					if (w->color == 1){
						w->color = 0;
						x->parent->color = 1;
						rotateLeft(x->parent);
						w = x->parent->right;
					}
					if (w->left->color == 0 && w->right->color == 0) {
						w->color = 0;
						x = x->parent;
					} else {
						if (w->right->color == 0) {
							w->left->color = 0;
							w->color = 1;
							rotateRight(w);
							w = x->parent->right;
						}
						w->color = x->parent->color;
						x->parent->color = 0;
						w->right->color = 0;
						rotateLeft(x->parent);
						x = root;
						root->parent = &sentinel;
						sentinel.right = root;
					}
				} else {
					w = x->parent->left;
					if (w->color == 1){
						w->color = 0;
						x->parent->color = 1;
						rotateRight(x->parent);
						w = x->parent->left;
					}
					if (w->right->color == 0 && w->left->color == 0) {
						w->color = 0;
						x = x->parent;
					} else {
						if (w->left->color == 0) {
							w->right->color = 0;
							w->color = 1;
							rotateLeft(w);
							w = x->parent->left;
						}
						w->color = x->parent->color;
						x->parent->color = 0;
						w->left->color = 0;
						rotateRight(x->parent);
						x = root;
						root->parent = &sentinel;
						sentinel.right = root;
					}
				}
			}
		}

	public:
		TreeMap() {
			min = &sentinel;
		}

		~TreeMap() {
			deleteTree(root);
		}

		TreeMap(std::initializer_list<value_type> list) {
			min = &sentinel;
			for (auto &&it : list) {
				(*this)[it.first] = it.second;
			}
		}

		TreeMap(const TreeMap &other) {
			min = &sentinel;
			if (other.root != nullptr) {
				root = new Node(other.root);
				copyInOrder(root, other.root);
				sentinel.right = root;
				root->parent = &sentinel;
				size = other.size;
				min = other.min;
			}
		}

		TreeMap(TreeMap &&other) {
			if (!other.isEmpty()) {
				root = other.root;
				min = other.min;
				size = other.size;
				sentinel.right = root;
				root->parent = &sentinel;
				other.root = nullptr;
				other.size = 0;
			} else {
				min = &sentinel;
			}
		}

		TreeMap &operator=(const TreeMap &other) {
			if (*this != other) {
				deleteTree(root);
				root = nullptr;
				min = &sentinel;
				if (other.root != nullptr) {
					root = new Node(other.root);
					copyInOrder(root, other.root);
					sentinel.right = root;
					root->parent = &sentinel;
					min = other.min;
				}
				size = other.size;
			}
			return *this;
		}

		TreeMap &operator=(TreeMap &&other) {
			deleteTree(root);
			if (!other.isEmpty()) {
				root = other.root;
				min = other.min;
				size = other.size;
				sentinel.right = root;
				root->parent = &sentinel;
				other.root = nullptr;
				other.size = 0;
			} else {
				size = 0;
				root = nullptr;
				min = &sentinel;
			}
			return *this;
		}

		bool isEmpty() const {
			return size == 0;
		}

		mapped_type &operator[](const key_type &key) {
			if (root == nullptr) {
				size++;
				root = new Node(key);
				root->parent = &sentinel;
				sentinel.right = root;
				min = root;
				insertFixup(root);
				return root->value.second;
			}
			auto tmp = root;
			auto parent = root;
			while (tmp != nullptr) {
				parent = tmp;
				if (tmp->value.first == key)
					return tmp->value.second;
				if (key < tmp->value.first)
					tmp = tmp->left;
				else
					tmp = tmp->right;
			}
			tmp = new Node(key);
			tmp->color = 1;
			if (key > parent->value.first)
				parent->right = tmp;
			else parent->left = tmp;
			tmp->parent = parent;
			if (key < min->value.first)
				min = tmp;
			size++;
			insertFixup(tmp);
			return tmp->value.second;
		}

		const mapped_type &valueOf(const key_type &key) const {
			auto tmp = root;
			while (tmp != nullptr) {
				if (tmp->value.first == key)
					return tmp->value.second;
				if (tmp->value.first < key)
					tmp = tmp->right;
				else tmp = tmp->left;
			}
			throw std::out_of_range("valueof");
		}

		mapped_type &valueOf(const key_type &key) {
			auto tmp = root;
			while (tmp != nullptr) {
				if (tmp->value.first == key)
					return tmp->value.second;
				if (tmp->value.first < key)
					tmp = tmp->right;
				else tmp = tmp->left;
			}
			throw std::out_of_range("valueof");
		}

		const_iterator find(const key_type &key) const {
			auto tmp = root;
			while (tmp != nullptr) {
				if (tmp->value.first == key)
					return const_iterator(tmp, min);
				if (tmp->value.first < key)
					tmp = tmp->right;
				else tmp = tmp->left;
			}
			return const_iterator(&(const_cast<Node &>(sentinel)), min);
		}

		iterator find(const key_type &key) {
			auto tmp = root;
			while (tmp != nullptr) {
				if (tmp->value.first == key)
					return iterator(const_iterator(tmp, min));
				if (tmp->value.first < key)
					tmp = tmp->right;
				else tmp = tmp->left;
			}
			return iterator(const_iterator(&sentinel, min));
		}

		void remove(const key_type &key) {
			remove(find(key));
		}

		void remove(const const_iterator &it) {
			Node* x;
			Node* z = it.getCurrent();
			if(z == &sentinel) throw std::out_of_range("remove sentinel");
			auto y = z;
			bool yOriginalColor = y->color;
			if (z->left == nullptr) {
				x = z->right;
				if(z->right != nullptr)
					transplant(z, z->right);
			} else if (z->right == nullptr) {
				x = z->left;
				if(z->left != nullptr)
					transplant(z, z->left);
			} else {
				auto tmp = z->right;
				while (tmp->left != nullptr) {
					tmp = tmp->left;
				}
				y = tmp;
				yOriginalColor = y->color;
				x = y->right;
				if (y->parent == z)
					x->parent = y;
				else {
					transplant(y, y->right);
					y->right = z->right;
					y->right->parent = y;
				}
				transplant(z , y);
				y->left = z->left;
				y->left->parent = y;
				y->color = z->color;
			}
			if (yOriginalColor == 0 && x != nullptr)
				removeFixup(x);
			size--;
		}

		size_type getSize() const {
			return size;
		}

		bool operator==(const TreeMap &other) const {
			if (size != other.size) return false;
			auto it = this->begin();
			for (auto it2 = other.begin(); it2 != other.end(); ++it2) {
				if ((it->first != it2->first) || (it->second != it2->second))
					return false;
				++it;
			}
			return true;
		}

		bool operator!=(const TreeMap &other) const {
			return !(*this == other);
		}

		iterator begin() {
			return iterator(const_iterator(min, min));
		}

		iterator end() {
			return iterator(const_iterator(&sentinel, min));
		}

		const_iterator cbegin() const {
			return const_iterator(min, min);
		}

		const_iterator cend() const {
			return const_iterator(&(const_cast<Node &>(sentinel)), min);
		}

		const_iterator begin() const {
			return cbegin();
		}

		const_iterator end() const {
			return cend();
		}
	};

	template<typename KeyType, typename ValueType>
	class TreeMap<KeyType, ValueType>::ConstIterator {
	public:
		using reference = typename TreeMap::const_reference;
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename TreeMap::value_type;
		using pointer = const typename TreeMap::value_type *;

	private:
		Node *current, *min;

		Node *successor(Node *node) {
			if (node->right != nullptr) {
				node = node->right;
				while (node->left != nullptr) {
					node = node->left;
				}
				return node;
			}
			Node *tmp = node->parent;
			while (tmp->parent != nullptr && node == tmp->right) {
				node = tmp;
				tmp = tmp->parent;
			}
			return tmp;
		}

		Node *predecessor(Node *node) {
			if (node->left != nullptr) {
				node = node->left;
				while (node->left != nullptr) {
					node = node->right;
				}
				return node;
			}
			Node *tmp = node->parent;
			while (tmp->parent != nullptr && node == tmp->left) {
				node = tmp;
				tmp = tmp->parent;
			}
			return tmp;
		}

	public:
		explicit ConstIterator() {}

		ConstIterator(Node *current, Node *min) : current(current), min(min) {}

		ConstIterator(const ConstIterator &other) : current(other.current), min(other.min) {}

		ConstIterator &operator++() {
			if ((current == nullptr) || (current->parent == nullptr))
				throw std::out_of_range("++op");
			current = successor(current);
			return *this;
		}

		ConstIterator operator++(int) {
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		ConstIterator &operator--() {
			if ((current == nullptr) || (current == min)) throw std::out_of_range("op--");
			if (current->parent == nullptr) {
				while (current->right != nullptr)
					current = current->right;
			} else
				current = predecessor(current);
			return *this;
		}

		ConstIterator operator--(int) {
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		reference operator*() const {
			if ((current == nullptr) || (current->parent == nullptr))
				throw std::out_of_range("op*");
			return (this->current->value);
		}

		pointer operator->() const {
			return &this->operator*();
		}

		bool operator==(const ConstIterator &other) const {
			return this->current == other.current;
		}

		bool operator!=(const ConstIterator &other) const {
			return !(*this == other);
		}

		Node* getCurrent() const
		{
			return current;
		}
	};

	template<typename KeyType, typename ValueType>
	class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator {
	public:
		using reference = typename TreeMap::reference;
		using pointer = typename TreeMap::value_type *;

		explicit Iterator() {}

		Iterator(const ConstIterator &other)
				: ConstIterator(other) {}

		Iterator &operator++() {
			ConstIterator::operator++();
			return *this;
		}

		Iterator operator++(int) {
			auto result = *this;
			ConstIterator::operator++();
			return result;
		}

		Iterator &operator--() {
			ConstIterator::operator--();
			return *this;
		}

		Iterator operator--(int) {
			auto result = *this;
			ConstIterator::operator--();
			return result;
		}

		pointer operator->() const {
			return &this->operator*();
		}

		reference operator*() const {
			// ugly cast, yet reduces code duplication.
			return const_cast<reference>(ConstIterator::operator*());
		}
	};

}

#endif /* AISDI_MAPS_MAP_H */

