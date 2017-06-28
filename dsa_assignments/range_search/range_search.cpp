// range_search.cpp : 定义控制台应用程序的入口点。
//

#define NDEBUG

#include <cassert>
#include <iostream>
#include <cstdio>
using namespace std;

typedef int Rank;
const int DEFAULT_CAPACITY = 5;

//功能：类vector封装了对数组的方法
template<typename T>
class Vector {
private:
	int _size;//规模
	T* _elem;//数据区
public:
	//构造函数
	Vector():_size(DEFAULT_CAPACITY){
		_elem = new T[_size];
	}
	Vector(int size) :_size(size){
		_elem = new T[_size];
	}

	//析构函数
	~Vector() {
		delete[] _elem;
	}

	//重载[]运算符,用于对元素的“引用”
	T& operator[] (Rank i) {
		return _elem[i];
	}
    
	//获得当前向量的规模
	int size() {
		return _size;
	}

	//在"有序向量"区间[lo, hi)中二分查找target元素，返回不大于target的最大元素的秩
	Rank binSearch(T& targrt, int lo, int hi) {
		while (lo < hi) {
			Rank mi = (lo + hi) >> 1;
			(targrt < _elem[mi]) ? hi = mi : lo = mi + 1;
		}
		return --lo;
	}

	void merge(Rank lo, Rank mi, Rank hi) { //各自有序的子向量[lo, mi)和[mi, hi)
		T* A = _elem + lo; //合并后的向量A[0, hi - lo) = _elem[lo, hi)
		int lb = mi - lo; T* B = new T[lb]; //前子向量B[0, lb) = _elem[lo, mi)
		for (Rank i = 0; i < lb; B[i] = A[i++]); //复制前子向量
		int lc = hi - mi; T* C = _elem + mi; //后子向量C[0, lc) = _elem[mi, hi)
		for (Rank i = 0, j = 0, k = 0; (j < lb) || (k < lc); ) { //B[j]和C[k]中的小者续至A末尾
			if ((j < lb) && (!(k < lc) || (B[j] <= C[k]))) A[i++] = B[j++];
			if ((k < lc) && (!(j < lb) || (C[k] <  B[j]))) A[i++] = C[k++];
		}
		delete[] B; //释放临时空间B
	} //归并后得到完整的有序向量[lo, hi)

	void mergeSort(Rank lo, Rank hi) { //0 <= lo < hi <= size
		///*DSA*/printf("\tMERGEsort [%3d, %3d)\n", lo, hi);
		if (hi - lo < 2) return; //单元素区间自然有序，否则...
		int mi = (lo + hi) / 2; //以中点为界
		mergeSort(lo, mi); mergeSort(mi, hi); //分别排序
		merge(lo, mi, hi); //归并
	}
};


//功能：范围查找函数，查找向量src中在目标区间[a, b]中的元素个数
int rangeSearch(int a, int b, Vector<int> &src) {
	assert(a <= b);

	src.mergeSort(0, src.size());

	int a_rank = src.binSearch(a, 0, src.size());
	//cout << a_rank << " ";
	int b_rank = src.binSearch(b, 0, src.size());
	//cout << b_rank << " ";

	bool exist = ((a_rank != -1)&&(src[a_rank] == a));
	//cout << exist << " ";

	if (exist) {
		//cout << "num: " << b_rank - a_rank + 1 << endl;
		return b_rank - a_rank + 1;
	}
	else
		//cout << "num: " << b_rank - a_rank << endl;
		return b_rank - a_rank;
}

//int main()
//{
//	//测试
//	//Vector<int> vec(5);
//	//int temp;
//	//for (int i = 0; i < vec.size(); i++)
//	//{
//	//	cin >> temp;
//	//	vec[i] = temp;
//	//}
//	//rangeSearch(3, 3, vec);
//	//rangeSearch(4, 4, vec);
//	//rangeSearch(4, 6, vec);
//	//rangeSearch(4, 5, vec);
//	//rangeSearch(5, 5, vec);
//	//rangeSearch(7, 2, vec);
//	//rangeSearch(2, 11, vec);
//
//
//
//	int size = 0, num_query = 0;
//	//cin >> size >> num_query;
//	scanf_s("%d %d", &size, &num_query);
//
//	int temp;
//	Vector<int> points(size);
//	for (int i = 0; i < size; i++)
//	{
//		//cin >> temp;
//		scanf_s("%d", &temp);
//		points[i] = temp;
//	}
//
//	Vector<int> outcomes(num_query);
//	int temp_a, temp_b;
//	for (int i = 0; i < num_query; i++)
//	{
//		//cin >> temp_a >> temp_b;
//		scanf_s("%d %d", &temp_a, &temp_b);
//		outcomes[i] = rangeSearch(temp_a, temp_b, points);
//	}
//
//	for (int i = 0; i < outcomes.size(); i++)
//	{
//		//cout << outcomes[i] << endl;
//		printf_s("%d\n", outcomes[i]);
//	}
//
//	return 0;
//}
//
