#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
private:
	std::list<value_type> *hashTable;
	size_type size = 0;
	const size_type capacity = 64037;
	size_type beginPos = capacity;
	void alloc()
	{
		hashTable = new std::list<value_type> [capacity];
	}
	size_type makeHash(key_type key) const
	{
		return std::hash<key_type> () (key) % capacity;
	}
public:
  HashMap()
  {
		alloc();
	}

	~HashMap()
	{
		if(hashTable != nullptr)
			delete [] hashTable;
	}

  HashMap(std::initializer_list<value_type> list)
  {
		alloc();
		for(auto &&it: list)
		{
			(*this)[it.first] = it.second;
		}
  }

  HashMap(const HashMap& other)
  {
		alloc();
		for(auto &&it: other)
		{
			(*this)[it.first] = it.second;
		}
  }

  HashMap(HashMap&& other)
  {
		hashTable = other.hashTable;
		other.hashTable = nullptr;
		size = other.size;
		other.size = 0;
		beginPos = other.beginPos;
  }

  HashMap& operator=(const HashMap& other)
  {
		if(*this == other)
			return *this;
    for(size_type i = 0;i < capacity;i++)
		{
			hashTable[i].clear();
		}
		size = 0;
		beginPos = capacity;
		for(auto && it: other)
		{
			(*this)[it.first] = it.second;
		}
		return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
		if(*this == other)
			return *this;
		if(hashTable != nullptr)
			delete [] hashTable;
		hashTable = other.hashTable;
		beginPos = other.beginPos;
		size = other.size;
		other.size = 0;
		other.hashTable = nullptr;
		//other.beginPos = nullptr;
		return *this;
  }

  bool isEmpty() const
  {
    return size == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
		auto hash = makeHash(key);
		for(auto it = hashTable[hash].begin(); it != hashTable[hash].end(); ++it) {
			if((*it).first == key)
				return (*it).second;
		}
		size++;
		if(hash < beginPos) beginPos = hash;
		hashTable[hash].push_front(std::make_pair(key, mapped_type()));
		return hashTable[hash].front().second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
		auto hash = makeHash(key);
		for(auto it = hashTable[hash].begin(); it != hashTable[hash].end(); ++it)
			if((*it).first == key)
				return (*it).second;
		throw std::out_of_range("valueof");
  }

  mapped_type& valueOf(const key_type& key)
  {
		auto hash = makeHash(key);
		for(auto it = hashTable[hash].begin(); it != hashTable[hash].end(); ++it)
			if((*it).first == key)
				return (*it).second;
		throw std::out_of_range("valueof");
  }

  const_iterator find(const key_type& key) const
  {
		auto hash = makeHash(key);
		for(auto it = hashTable[hash].begin(); it != hashTable[hash].end(); ++it)
			if((*it).first == key)
				return const_iterator(const_cast<HashMap&> (*this), hashTable + hash, it);
		return end();
  }

  iterator find(const key_type& key)
  {
		auto hash = makeHash(key);
		for(auto it = hashTable[hash].begin(); it != hashTable[hash].end(); ++it)
			if((*it).first == key)
				return iterator(const_iterator(*this, hashTable + hash, it));
		return end();
  }

  void remove(const key_type& key)
  {
		size--;
		hashTable[makeHash(key)].remove(std::make_pair(key, valueOf(key)));
  }

  void remove(const const_iterator& it)
	{
		size--;
		hashTable[makeHash(it->first)].remove(std::make_pair(it->first, it->second));
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap& other) const
  {
		if(size != other.size)
			return false;
    for(size_type i = 0;i < capacity;i++){
			//if(hashTable[i] != other.hashTable[i]) return false;
			bool found = false;
			for(auto it = hashTable[i].begin(); it != hashTable[i].end(); ++it)
			{
				for(auto it2 = other.hashTable[i].begin(); it2 != other.hashTable[i].end(); ++it2)
				{
					if(*it == *it2)
					{
						found = true;
						break;
					}
				}
				if(found == false) return false;
				found = false;
			}

			/*auto it = find(i);
			auto it2 = other.find(i);
			if((it == end() && it2 == other.end()) || (it != end() && it2 == other.end()) || (it == end() && it2 != other.end()) || (*it != *it2))
				return false;*/
		}
		return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
		if(size == 0) return this->end();
		return iterator(const_iterator(*this, hashTable + beginPos, (hashTable + beginPos)->begin()));
  }

  iterator end()
  {
		return iterator(const_iterator(*this, hashTable + capacity, (hashTable + capacity - 1)->end()));
  }

  const_iterator cbegin() const
  {
		if(size == 0) return this->cend();
		return const_iterator(const_cast<HashMap&> (*this), hashTable + beginPos, (hashTable + beginPos)->begin());
  }

  const_iterator cend() const
  {
		return const_iterator(const_cast<HashMap&> (*this), hashTable + capacity, (hashTable + capacity - 1)->end());
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

private:

	HashMap<key_type, mapped_type > &list;
	std::list<value_type> *current;
	typename std::list<value_type>::iterator iterator;

public:

  explicit ConstIterator():list()
  {}

	ConstIterator(HashMap<key_type, mapped_type > &list, std::list <value_type> *current, typename std::list<value_type>::iterator iterator)
			:list(list), current(current), iterator(iterator){}

  ConstIterator(const ConstIterator& other):list(other.list),current(other.current),iterator(other.iterator) {}

  ConstIterator& operator++()
  {
		if(current == list.hashTable + list.capacity) throw std::out_of_range("op++");
		if(iterator == --(current->end()))
		{
			current++;
			while((current != list.hashTable + list.capacity) && (current->empty()))
			{
				current++;
			}
			if(current == list.hashTable + list.capacity)
			{
				iterator = ((current-1)->end());
				return *this;
			}
			iterator = current->begin();
		}
		else iterator++;
		return *this;
  }

  ConstIterator operator++(int)
  {
    auto tmp = this;
		++(*this);
		return *tmp;
  }

  ConstIterator& operator--()
  {
    if(((current == list.hashTable) && iterator == current->begin()) || ((unsigned)(current - list.hashTable) == list.beginPos))
			throw std::out_of_range("op--");
		if((iterator == current->begin()) || (current == list.hashTable + list.capacity))
		{
			current--;
			while((current != list.hashTable) && (current->empty()))
			{
				current--;
			}
			iterator = --(current->end());
		}
		else iterator--;
		return *this;
  }

  ConstIterator operator--(int)
  {
		auto tmp = this;
		--(*this);
		return *tmp;
  }

  reference operator*() const
  {
    if(current == list.hashTable + list.capacity) throw std::out_of_range("op*");
		return *iterator;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (current == other.current && iterator == other.iterator);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
