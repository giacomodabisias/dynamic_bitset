#include <iostream>
#include <stdint.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <memory.h>
#include <stdexcept>

class Proxy{

public:

	void setIndex(int index){
		index_ = index;
	}

	void setUint8tSize(int length){
		uint8t_size_ = length;
	}

	void setBitSize(int length){
		bit_size_ = length;
	}

	void setData(uint8_t * data){
		data_ = data;
	}

	void operator=(bool bit){
		uint8_t tmp[uint8t_size_];
		for(int i = 0; i < uint8t_size_; ++i)
			tmp[i] = 0;
		tmp[index_ / 8] = (1 << (index_ % 8));
		if(!bit){
			for(int i = 0; i < uint8t_size_; ++i)
				data_[i] &= ~tmp[i];
		}
		else{
			for(int i = 0; i < uint8t_size_; ++i)
				data_[i] |= tmp[i];
		}
	}
		
private:

	int uint8t_size_, bit_size_, index_;
	uint8_t * data_;

};

class Bits {

public:

	friend Proxy;

	friend std::ostream& operator<< (std::ostream & o, const Bits & b){	
		for(int j = b.bit_size_ - 1; j >=0 ; --j)
				o << ((1 << (j % 8)) & (b.data_[j /8])? 1 : 0) << " ";
		return o;
	}

	Bits(){
		proxy_.setData(data_);
	}

	Bits(int value){
		if(value > 0){
			bit_size_ = getBitsize(value);
			uint8t_size_ = ceil((float)bit_size_ / 8);
			std::cout << "number: " << value <<"\nnumber of Bits: " << bit_size_ << "\nuint8_t size: " << uint8t_size_<< std::endl;
			data_ = new uint8_t[uint8t_size_];
			proxy_.setData(data_);
			int2bit(data_, value);
		}else  
			throw std::domain_error("negative values not allowed");
	}

	Bits(uint8_t *bit_value, const int length){
		if(length > 0){
			bit_size_ = length;
			uint8t_size_ = ceil((float)bit_size_ / 8);
			std::cout << "\nnumber of input Bits: " << bit_size_ << "\nuint8_t size: " << uint8t_size_<< std::endl;
			data_ = new uint8_t[uint8t_size_];
			proxy_.setData(data_);
			memcpy(data_, bit_value, uint8t_size_);
		}else  
			throw std::runtime_error("invalid length value");
	}

	int getInt(){
		int tmp = 0;
		if(data_){
			bit2int(data_, tmp);
		}
		return tmp;
	}

	void getBitVector(std::vector<bool> & v){
		v.resize(bit_size_);
		for(int j = 0; j < bit_size_ ; ++j)
			v[j] = (1 << (j % 8)) & (data_[j /8])? 1 : 0;
	}

	Bits operator=(const Bits & b){
		this->bit_size_ = b.bit_size_;
		this->uint8t_size_ = b.uint8t_size_;
		this->data_ = new uint8_t[this->uint8t_size_];
		memcpy(this->data_, b.data_, this->uint8t_size_);
		return *this;
	}

	Bits operator=(const int bit_value){
		if(bit_value > 0){
			this->bit_size_ = getBitsize(bit_value);
			this->uint8t_size_ = ceil((float)bit_size_ / 8);
			this->data_ = new uint8_t[uint8t_size_];
			this->int2bit(data_, bit_value);
			return *this;
		}
		else  
			throw std::domain_error("negative values not allowed");
	}

	bool operator[](int i) const{
		if(data_ && i < bit_size_)
			return (bool)(data_[i]);
		else
			throw std::out_of_range("out of range!");
	}

	Proxy& operator[](int i){
		if(data_ && i < bit_size_){
			proxy_.setIndex(i);
			proxy_.setBitSize(bit_size_);
			proxy_.setUint8tSize(uint8t_size_);
			return proxy_;
		}
		else
			throw std::out_of_range("out of range!");
	}

	Bits& operator &= (Bits & b){
		for(int i = 0; i < this->uint8t_size_; ++i)
			this->data_[i] &= b.data_[i];
		return *this;
	}

	Bits& operator |= (Bits & b){
		for(int i = 0; i < this->uint8t_size_; ++i)
			this->data_[i] |= b.data_[i];
		return *this;
	}

private:

	void bit2int(uint8_t * data, int & value){
		for(int j = 0; j < bit_size_ ; ++j){
			value += (1 << (j % 8)) & data_[j / 8] ? (1 << j) : 0; 	
		}
	}

	void int2bit(uint8_t * data, int value){
		int j = 0;
		while(value != 0){
			if(j % 8 == 0) 
				data_[j / 8] = 0;
			data[j / 8] += (value % 2) << (j % 8);
			value /= 2;
			++j;
		}
	}

	int getBitsize(const int value){
		return ceil(log2(value + 1));  
	}

	uint8_t  * data_ = 0;
	short bit_size_ = 0;
	short uint8t_size_ = 0;
	Proxy proxy_;
};




int main(int argc, char * argv[]){

	if(argc < 2){
		std::cout << "input number missing" << std::endl;
		return -1;
	}

	std::cout << "creation from int" << std::endl;
	Bits b(atoi(argv[1]));
	std::cout << "created" << std::endl;
	std::cout << b << std::endl;

	std::cout << "creation from uint8_t" << std::endl;
	uint8_t *t;
	t = new uint8_t[2];
	t[0] = 0;
	t[1] = 0;

	t[0] = 1;
	t[1] += (1 << 3);
	Bits b1(t, 8 + 4);

	std::cout << "check" << std::endl;
	std::cout << b1 << std::endl;
	std::cout << b1.getInt() << std::endl;

	std::cout << "reference operator" << std::endl;
	b1[6] = true;
	std::cout << b1 << std::endl;
	std::cout << b1.getInt() << std::endl;
	b1[6] = false;
	std::cout << b1 << std::endl;
	std::cout << b1.getInt() << std::endl;

	std::cout << "copy test" << std::endl;
	b1 = b;
	std::cout << b1 << std::endl;
	b = 9;
	std::cout << b1 << std::endl;
	std::cout << b << std::endl;

	std::cout << "empty constructor" << std::endl;
	Bits b2;
	b2 = 6;
	std::cout << b2 << std::endl;



	return 0;
}