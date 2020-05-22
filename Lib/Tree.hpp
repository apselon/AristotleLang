#pragma once

template <typename key_t>
class TNode_t {

private:
	TNode_t* parent_node = nullptr;
	TNode_t* left_son    = nullptr;
	TNode_t* right_son   = nullptr;

public:
	key_t key = {};

	TNode_t() = default;
	explicit TNode_t(key_t key_, TNode_t* left_ = nullptr, TNode_t* right_ = nullptr, 
            TNode_t* parent_ = nullptr):
            key(key_), left_son(left_), right_son(right_), parent_node(parent_){};

	//virtual ~TNode_t(){ this->remove(); };
	
	virtual TNode_t* parent() { return this->parent_node; }
	virtual TNode_t* left()   { return this->left_son;    }
	virtual TNode_t* right()  { return this->right_son;   }

	key_t remove();
	void remove_subtree();

	int is_left();
	int is_right();


	virtual TNode_t* attach_left(TNode_t* new_left);
	virtual TNode_t* attach_right(TNode_t* new_right);

	virtual TNode_t* attach_left(key_t new_val);		
	virtual TNode_t* attach_right(key_t new_val);		

	virtual void unattach_from_parent();

	void dump(const char* filename);
};
