#include <cstring>
#include <utility>

template <typename T>
class Vector {
private:
	size_t size_;
	size_t max_size_;
	T* data_;

public:
	explicit Vector(size_t max_size_ = 1);

	void push_back(const T& new_elem);
	void reshape(size_t new_size);

	size_t size();
	size_t capacity();

	T& operator[](size_t index);
	~Vector();
};

template <typename T>
Vector<T>::Vector(size_t max_size_): size_(0), max_size_(max_size_) {
	data_ = new T[max_size_];
};

template <typename T>
T& Vector<T>::operator[](size_t pos){
	return data_[pos];
}

template <typename T>
void Vector<T>::reshape(size_t new_size){
	if (new_size <= max_size_) return;

	T* new_data = new T[new_size];
	memcpy(new_data, data_, sizeof(T) * size_);

	delete [] data_;
	data_ = new_data;

	max_size_ = new_size;
}

template <typename T>
void Vector<T>::push_back(const T& new_elem){
	if (size_ == max_size_) reshape(2 * max_size_);
	data_[size_++] = new_elem;
	
}

template <typename T>
size_t Vector<T>::size(){
	return size_;
}

template <typename T>
size_t Vector<T>::capacity(){
	return max_size_;
}

template <typename T>
Vector<T>::~Vector(){
	delete [] data_;
}
