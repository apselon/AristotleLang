template <typename T>
class Node {
public:
	T val;
	Node* next_;
	Node* prev_;

	Node(T val): val(val), next_(nullptr), prev_(nullptr) {};
};

