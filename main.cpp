#include <cstddef>
#include <cstdlib>
#include <string>
#include <random>
#include <chrono>

#include "TreeMap.h"
#include "HashMap.h"

using namespace aisdi;

void treeAppend(TreeMap<int, std::string> &tree, int i) {
	tree[i] = "testString";
}
void hashMapAppend(HashMap<int, std::string> &hashMap, int i) {
	hashMap[i] = "testString";
}
void treeFind(TreeMap<int, std::string> &tree, int i) {
	tree.find(i);
}
void hashMapFind(HashMap<int, std::string> &hashMap, int i) {
	hashMap.find(i);
}
void iterateTree(TreeMap<int, std::string> &tree, int i) {
	(void) i;
	for(auto &&it: tree) (void) it;
}
void iterateHashMap(HashMap<int, std::string> &hashMap, int i) {
	(void) i;
	for(auto &&it: hashMap) (void) it;
}
TreeMap<int, std::string> createTree(size_t elements)
{
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);
	TreeMap<int, std::string> tree;
	for(size_t i = 0; i < elements; i++)
		tree[distribution(generator)] = "testString";
	return tree;
}
HashMap<int, std::string> createHashMap(size_t elements) {
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);
	HashMap<int, std::string> hashMap;
	for(size_t i = 0; i < elements; i++)
		hashMap[distribution(generator)] = "testString";
	return hashMap;
}
void testTree(void (*function)(TreeMap<int, std::string> &tree, int i), double tests, size_t messageData, size_t elements, size_t startElements, std::string name){
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);
	std::chrono::duration<double> treeTime;
	for(double i = 0; i < tests; i++) {
		TreeMap<int, std::string> tree = createTree(startElements);
		auto startTree = std::chrono::steady_clock::now();
		for(size_t j = 0; j < elements; j++)
		{
			function(tree, distribution(generator));
		}
		auto endTree = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedSecondsTree = endTree-startTree;
		treeTime += elapsedSecondsTree;
	}
	std::cout<<"Tree "<<name<<" time of "<<messageData<<" elements: "<<std::chrono::duration_cast<std::chrono::microseconds>(treeTime).count()/tests<<"\n";
}

void testHashMap(void (*function)(HashMap<int, std::string> &hashMap, int i), double tests, size_t messageData, size_t elements, size_t startElements, std::string name){
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);
	std::chrono::duration<double> hashMapTime;
	for(double i = 0; i < tests; i++) {
		HashMap<int, std::string> hashMap = createHashMap(startElements);
		auto startHashMap = std::chrono::steady_clock::now();
		for(size_t j = 0; j < elements; j++)
		{
			function(hashMap, distribution(generator));
		}
		auto endHashMap = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedSecondHashMap = endHashMap-startHashMap;
		hashMapTime += elapsedSecondHashMap;
	}
	std::cout<<"HashMap "<<name<<" time of "<<messageData<<" elements: "<<std::chrono::duration_cast<std::chrono::microseconds>(hashMapTime).count()/tests<<"\n";
}

int main()
{
	const int tests = 2000;
  testTree(treeAppend, tests, 1000, 1000, 0, "append");
	testHashMap(hashMapAppend, tests, 1000, 1000, 0, "append");
	testTree(treeAppend, tests, 10000, 10000, 0, "append");
	testHashMap(hashMapAppend, tests, 10000, 10000, 0, "append");
	testTree(treeFind, tests, 1000, 1000, 1000, "find");
	testHashMap(hashMapFind, tests, 1000, 1000, 1000, "find");
	testTree(treeFind, tests, 10000, 10000, 10000, "find");
	testHashMap(hashMapFind, tests, 10000, 10000, 10000, "find");
	testTree(iterateTree, tests, 1000, 1, 1000, "iterate");
	testHashMap(iterateHashMap, tests, 1000, 1, 1000, "iterate");
	testTree(iterateTree, tests, 10000, 1, 10000, "iterate");
	testHashMap(iterateHashMap, tests, 10000, 1, 10000, "iterate");
  return 0;
}
