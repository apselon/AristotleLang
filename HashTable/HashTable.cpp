#pragma once
#include "List.cpp"

template <typename T1, typename T2>
struct Pair_t {
	T1 first;
	T2 second;
};

template <typename key_t, typename val_t, unsigned long (*hash)(key_t), int (*cmp)(key_t, key_t), unsigned long max_size>
class HashTable {
public:
	List<Pair_t<key_t, val_t>>* table = nullptr; 

	HashTable(){
		table = new List<Pair_t<key_t, val_t>>[max_size]();
	}

	void insert(key_t key, val_t val){
		unsigned long h = hash(key) % max_size;
		
		Node<Pair_t<key_t, val_t>>* new_node = new Node<Pair_t<key_t, val_t>>(Pair_t<key_t, val_t>({key, val}));
		table[h].push_front(new_node);
	}

	Node<Pair_t<key_t, val_t>>* find(key_t key){

		unsigned long h = hash(key) % max_size;
		Node<Pair_t<key_t, val_t>>* cur = table[h].head_;

		while (cur != nullptr){
			if (cmp(key, cur->val.first) == 0){
				return cur;
			}

			cur = cur->next_;
		}

		return cur;
	}

	~HashTable(){
		delete [] table;
	}
};
