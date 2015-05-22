#include <vector>


template <typename vector_type>
class swapVector {

private:
	std::vector<vector_type> data;
	int lastIndex = 0;
public:
	int size() {
		return lastIndex;
	}
	vector_type& operator [] (int index) {
		return data[index];
	}
	void push(vector_type input) {
		if (data.size() == lastIndex) {
			data.push_back(input);
			lastIndex++;
		}else{
			data[++lastIndex] = input;			
		}
	}
	void remove(int index) {
		data[index] = data[lastIndex];
		data[lastIndex--] = 0;
	}
};

