// Lighthouse.cpp : 定义控制台应用程序的入口点。
//

#include <cstdio>
#include <stdio.h>
#include <cassert>
#include <cstdlib>

//禁止redirecting
//#define _OJ_

// 别人写的，拿来参考！
const int SZ = 1 << 20;  //快速io  
struct fastio {
	char inbuf[SZ];
	char outbuf[SZ];
	fastio() {
		setvbuf(stdin, inbuf, _IOFBF, SZ);
		setvbuf(stdout, outbuf, _IOFBF, SZ);
	}
}io;

//test case generator测例生成器
void generateTextCases(int num_houses) {
	freopen("input.txt", "w", stdout);
	for (int i = 0; i < num_houses; i++)
	{
		srand(i);
		printf("%d %d\n", rand() % 1000, rand() % 1000);
	}
}



//二路归并：时间复杂度O(n),空间复杂度O(mi - lo)
void merge(int(*arr)[2], int lo, int mi, int hi) {
	int(*A)[2] = arr + lo; //合并后的向量A[0, hi - lo) = arr[lo, hi)
	int lb = mi - lo; int(*B)[2] = new int[lb][2]; //前子向量B[0, lb) = _elem[lo, mi)
	for (int i = 0; i < lb; (B[i][0] = A[i][0], B[i][1] = A[i++][1])); //复制前子向量
	int lc = hi - mi; int(*C)[2] = arr + mi; //后子向量C[0, lc) = _elem[mi, hi)
	for (int i = 0, j = 0, k = 0; (j < lb) || (k < lc); ) { //B[j]和C[k]中的小者续至A末尾
		if ((j < lb) && (!(k < lc) || (B[j][0] <= C[k][0]))) (A[i][0] = B[j][0], A[i++][1] = B[j++][1]);
		if ((k < lc) && (!(j < lb) || (C[k][0] < B[j][0]))) (A[i][0] = C[k][0], A[i++][1] = C[k++][1]);
	}
	delete[] B; //释放临时空间B
}

//归并排序：时间复杂度O(nlogn),空间复杂度为O(n)
//按二维数组的x坐标值来排序，即x为排序的key
void mergeSort(int(*arr)[2], int lo, int hi) { //0 <= lo < hi <= size
	assert(0 <= lo);
	//assert(hi <= sizeof(arr)/sizeof(int));
	assert(lo < hi);

	if (hi - lo < 2) return; //单元素区间自然有序，否则...
	int mi = (lo + hi) >> 1; //以中点为界
	mergeSort(arr, lo, mi); mergeSort(arr, mi, hi); //分别排序
	merge(arr, lo, mi, hi); //归并
}


//得到两个数组中的逆序对的数目
long long inverBetween(int(*arr)[2], int lo, int mi, int hi) {
	long long num_inver = 0;

	int(*A)[2] = arr + lo; //合并后的向量A[0, hi - lo) = arr[lo, hi)
	int lb = mi - lo; int(*B)[2] = new int[lb][2]; //前子向量B[0, lb) = arr[lo, mi)
	for (int i = 0; i < lb; i++) {
		B[i][0] = A[i][0];
		B[i][1] = A[i][1];
	} //复制前子向量
	int lc = hi - mi; int(*C)[2] = arr + mi; //后子向量C[0, lc) = arr[mi, hi)
	for (int i = 0, j = 0, k = 0; (j < lb) || (k < lc); ) { //B[j]和C[k]中的小者续至A末尾
		if ((j < lb) && (!(k < lc) || (B[j][1] < C[k][1]))) {
			A[i][0] = B[j][0];
			A[i][1] = B[j][1];
			num_inver += k; //注意：此处是k，不是k - 1， 更不是k - mi!!!!!!!!!!!!!!!!!!!
			i++;
			j++;
		}
		if ((k < lc) && (!(j < lb) || (C[k][1] < B[j][1]))) {
			A[i][0] = C[k][0];
			A[i][1] = C[k][1];
			i++;
			k++;
		};
	}
	delete[] B; //释放临时空间B

	return num_inver;
}

//得到数组区间arr[lo, hi)中的逆序对的数目
//时间复杂度O(nlogn),空间复杂度O(nlogn)
long long inverInside(int(*arr)[2], int lo, int hi) {
	if (hi - lo < 2) return 0;
	int mi = (hi + lo) >> 1;
	//得到arr[lo, mi)内部的invertion pairs数
	long long num_inver_left = inverInside(arr, lo, mi);
	//得到arr[mi, hi)内部的invertion pairs数
	long long num_inver_right = inverInside(arr, mi, hi);
	//inverBetween可以得到[lo, mi)和[mi, hi)之间的逆序对数
	return inverBetween(arr, lo, mi, hi) + num_inver_left + num_inver_right;
}

//显示houses中所有元素的值
void display(int(*houses)[2], int num) {
	for (int i = 0; i < num; i++)
	{
		printf("坐标值为：%d %d\n", houses[i][0], houses[i][1]);
	}
}

int main()
{
	//生成test cases
	//generateTextCases(1000);

	long long num = 0;//灯塔的总数
	scanf_s("%d", &num);

	int(*houses)[2] = new int[num][2];

	int x, y;//灯塔的x和y坐标
#ifndef _OJ_
	freopen("input.txt", "r", stdin);
#endif
	for (int i = 0; i < num; i++)
	{
		scanf_s("%d %d", &x, &y);
		houses[i][0] = x;
		houses[i][1] = y;
	}

	//对各个Lighthouse，根据x值来排序
	mergeSort(houses, 0, num);//O(nlogn)

	//显示houses中所有元素的值
	//display(houses, num);

	long long total_pairs = (num * (num - 1)) / 2;
	printf("%ld", total_pairs - inverInside(houses, 0, num));//O(nlogn)

	delete[] houses;
	return 0;
}

