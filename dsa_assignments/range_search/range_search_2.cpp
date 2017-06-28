#include <cstdio>
#include <cassert>

//二路归并：时间复杂度O(n),空间复杂度O(mi - lo)
void merge(int arr[], int lo, int mi, int hi) {
	int* A = arr + lo; //合并后的向量A[0, hi - lo) = arr[lo, hi)
	int lb = mi - lo; int* B = new int[lb]; //前子向量B[0, lb) = _elem[lo, mi)
	for (int i = 0; i < lb; B[i] = A[i++]); //复制前子向量
	int lc = hi - mi; int* C = arr + mi; //后子向量C[0, lc) = _elem[mi, hi)
	for (int i = 0, j = 0, k = 0; (j < lb) || (k < lc); ) { //B[j]和C[k]中的小者续至A末尾
		if ((j < lb) && (!(k < lc) || (B[j] <= C[k]))) A[i++] = B[j++];
		if ((k < lc) && (!(j < lb) || (C[k] <  B[j]))) A[i++] = C[k++];
	}
	delete[] B; //释放临时空间B
}

//归并排序：时间复杂度O(nlogn),空间复杂度为O(n)
void mergeSort(int arr[], int lo, int hi) { //0 <= lo < hi <= size
	assert(0 <= lo);
	//assert(hi <= sizeof(arr)/sizeof(int));
	assert(lo < hi);

	if (hi - lo < 2) return; //单元素区间自然有序，否则...
	int mi = (lo + hi) >> 1; //以中点为界
	mergeSort(arr, lo, mi); mergeSort(arr, mi, hi); //分别排序
	merge(arr, lo, mi, hi); //归并
}

//在arr[lo, hi)中二分查找target
//时间复杂度为O(logn),空间复杂度为O(1)
int binSearch(int arr[], int target, int lo, int hi) {
	while (lo < hi) {
		int mi = (lo + hi) >> 1;
		(target < arr[mi]) ? hi = mi : lo = mi + 1;
	}
	return --lo;
}

//在数组arr中查找区间[a, b]中的元素个数
int rangeSearch(int arr[], int arr_size, int a, int b) {
	int a_rank = binSearch(arr, a, 0, arr_size);
	int b_rank = binSearch(arr, b, 0, arr_size);

	bool exist = ((a_rank != -1) && (arr[a_rank] == a));
	if (exist) {
		return b_rank - a_rank + 1;
	}
	else
		return b_rank - a_rank;
}

int main() {
	//输入points的size和query的次数
	int size, num_query;
	scanf_s("%d %d", &size, &num_query);

	//构建规模为size数组，并依次输入各个point的坐标
	int* points = new int[size]; //因为数组的size是用户输入的，所以使用动态数组，别忘了delete[]！！！！！！！！！！
	for (int i = 0; i < size; i++) {
		scanf_s("%d", &points[i]);
	}

	//对数组points进行归并排序
	mergeSort(points, 0, size);

	//依次输入要查找的区间[a, b],并使用rangeSearch函数查找出points在这个区间内的元素个数
	int a, b;
	for (int i = 0; i < num_query; i++)
	{
		scanf_s("%d %d", &a, &b);

		//退化情况
		if (a > b) {
			int temp;
			temp = b;
			b = a;
			a = temp;
		}

		printf("%d\n", rangeSearch(points, size, a, b));
	}

	delete[] points;
	return 0;
}