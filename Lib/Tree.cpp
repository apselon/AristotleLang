#pragma once

#include "Tree.hpp"

template <typename T>
int TNode_t<T>::is_left(){

	if (this->parent_node == nullptr || this == this->parent_node->right_son){
		return 0;
	}

	return 1;
}

template <typename T>
int TNode_t<T>::is_right(){

	if (this->parent_node == nullptr || this == this->parent_node->left_son){
		return 0;
	}

	return 1;
}

template <typename T>
void TNode_t<T>::remove_subtree(){

	if (this != nullptr){
		if (this->left_son) this->left_son->remove_subtree();
		if (this->right_son) this->right_son->remove_subtree();

		this->remove();
	}
}

template <typename T>
T TNode_t<T>::remove(){

	if (this->parent_node != nullptr && this->is_left()){
		this->parent_node->left_son = nullptr;
	}

	else if (this->parent_node != nullptr && this->is_right()){
		this->parent_node->right_son = nullptr;
	}

	T val = this->key;

	return val;
}

template <typename T>
TNode_t<T>* TNode_t<T>::attach_left(TNode_t<T>* new_left){
	
	if (new_left != nullptr){
		new_left->parent_node = this;
	}

	this->left_son = new_left;
	
	return new_left;
}

template <typename T>
TNode_t<T>* TNode_t<T>::attach_right(TNode_t<T>* new_right){
	
	if (new_right != nullptr){
		new_right->parent_node = this;
	}

	this->right_son = new_right;

	return new_right;
}

template <typename T>
TNode_t<T>* TNode_t<T>::attach_left(T val){

	return this->attach_left(new TNode_t<T>(val));
}

template <typename T>
void TNode_t<T>::unattach_from_parent(){

	if (this->is_left())  this->parent()->attach_left (nullptr);
	else if (this->is_right()) this->parent()->attach_right(nullptr);
	
	this->parent_node = nullptr;
}

template <typename T>
TNode_t<T>* TNode_t<T>::attach_right(T val){

	return this->attach_right(new TNode_t<T>(val));
}
