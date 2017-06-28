// Toy: 状态转换图及其搜索
//
//#define NDEBUG

#include "stdafx.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//定义常量和全局变量**********************************************************************
const int MAX_TOY = 40320; //8！= 40320，即该玩具的最大状态数
int POS_TO_NUM[8][8][8][8][8][8][8]; //8MB 空间利用率2%  pos_to_num[x1][x2][x3][x4][x5][x6][x7]表示排列x1x2x3x4x5x6x7在全排列前七位中的位置
unsigned char NUM_TO_POS[MAX_TOY][7]; //275KB
unsigned char HASH_TABLE[MAX_TOY]; //39KB
int QUEUE[MAX_TOY]; //158KB

//初始化函数*********************************************************************

//打表POS_TO_NUM[]和 NUM_TO_POS[][]: 顺序枚举出0~7的全排列的前七位，并记录在两个表中。
//i表示枚举排列的第i位，rank表示当前排列在全排列中的位置
//搜索规模:40320 时间复杂度:O(40320)，若cpu是1GHz，则大概需要0.04ms 空间复杂度:19byte左右 
void init(int i) {
	assert(0 <= i);
	assert(7 >= i);
	static int rank = 0;
	static bool check_num[8]; //该表格，用于检查i的状态check_num[i],若为false，表示可用于后面几位的枚举，若为true，则表示被占用
	static unsigned char permu[7];//static局部变量，若为初始化，则默认初始化为0
	
	if (i == 7) {//递归基，即递归分解的边界
		for (int j = 0; j < 7; j++) NUM_TO_POS[rank][j] = permu[j];
		POS_TO_NUM[permu[0]][permu[1]][permu[2]][permu[3]][permu[4]][permu[5]][permu[6]] = rank;
		rank++; //位置码加1
		return;
	}
	for (int num = 0; num < 8; num++)
	{
		if (false == check_num[num]) {
			check_num[num] = true;  //重要
			permu[i] = num;
			init(i + 1); //递归（递归分解+逐级回归）
			check_num[num] = false; //别忘了！！！！！！ 
		}
	}

}
void preTester(int num_case) {
	init(0);
	for (int i = 0; i < num_case; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			printf("%d ", NUM_TO_POS[i][j]);
		}
		printf("\n");
	}
}

//辅助函数*************************************************************************
//对两个int类型数据实现交换
void swap(int& a, int& b) {
	a = a^b;
	b = b^a;
	a = a^b;
}
void swapTester(int a, int b) {
	printf("%d %d => ", a, b);
	swap(a, b);
	printf("%d %d", a, b);
}

//对状态数字num_pos进行转动编号为i的转动
int turn(int num_pos, int i) {
	assert(0 <= num_pos);
	assert(MAX_TOY > num_pos);

	//解压缩unzip，将状态数字解压缩为状态数组
	int* positions = new int[8];
	int adder = 0; //统计数组前7位的总和
	for (size_t j = 0; j < 7; j++)
	{
		adder += NUM_TO_POS[num_pos][j];
		positions[j] = NUM_TO_POS[num_pos][j];
	}
	positions[7] = 28 - adder; //最后一位在打表时没有存储，所以解压缩时要额外赋值

	if (0 == i)//上下翻转
	{
		swap(positions[0], positions[7]); swap(positions[1], positions[6]);
		swap(positions[2], positions[5]); swap(positions[3], positions[4]);
	}
	else if (1 == i)//循环右移
	{
		int old_position3 = positions[3];
		positions[3] = positions[2]; positions[2] = positions[1]; positions[1] = positions[0]; positions[0] = old_position3;
		int old_position4 = positions[4];
		positions[4] = positions[5]; positions[5] = positions[6]; positions[6] = positions[7]; positions[7] = old_position4;
	}
	else if (2 == i)//中心顺时针旋转
	{
		int old_position6 = positions[6];
		positions[6] = positions[5]; positions[5] = positions[2]; positions[2] = positions[1]; positions[1] = old_position6;
	}
	else assert(false);

	int new_num_pos = POS_TO_NUM[positions[0]][positions[1]][positions[2]][positions[3]][positions[4]][positions[5]][positions[6]];
	delete[] positions;
	return new_num_pos;
}

//搜索状态数字为num_pos的Toy，并返回到还原态的最短路径
int search(int num_pos) {
	assert(0 <= num_pos);
	assert(MAX_TOY > num_pos);
	memset(HASH_TABLE, 0, MAX_TOY*sizeof(unsigned char));//初始化散列表HASH_TABLE[]
	int front = 0, rear = 0; //队列，每个元素是状态数组

	if (0 == num_pos) return 0; //表示Toy已经还原了
	QUEUE[front] = num_pos; HASH_TABLE[num_pos] = 1; //初始化散列表和广搜队列的首元素

	while (front <= rear) {
		int now = QUEUE[front]; front++;

		for (int mode = 0; mode < 3; mode++)
		{
			int result = turn(now, mode);

			////调试
			//for (int i = 0; i < 7; i++)
			//{
			//	printf("%d ", NUM_TO_POS[result][i]);
			//}
			//printf("\n");

			/*int result_rank = POS_TO_NUM[result[0]][result[1]][result[2]][result[3]][result[4]][result[5]][result[6]];*/

			if (0 == result) return HASH_TABLE[now];

			if (HASH_TABLE[result] == 0) {
				HASH_TABLE[result] = HASH_TABLE[now] + 1;
				rear++; QUEUE[rear] = result;
			}
		}
	}
	return -1;
}

//主函数************************************************************************************
int main()
{
	init(0); //初始化-打表

#define NREDIR
#ifndef NREDIR
	freopen("input.txt", "r", stdin);
#endif


	int num_toy; scanf("%d", &num_toy);
	if (0 == num_toy) return 0;
	unsigned char pos[8];
	unsigned char output[1000];
	for (int i = 0; i < num_toy; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			scanf("%d", pos + j);
		}

		int num_pos = POS_TO_NUM[pos[0] - 1][pos[1] - 1][pos[2] - 1][pos[3] - 1][pos[4] - 1][pos[5] - 1][pos[6] - 1];

		output[i] = search(num_pos);
	}

	for (int i = 0; i < num_toy; i++)
	{
		printf("%d\n", output[i]);
	}

	return 0;
}

//本算法low performance的原因是，当input很多个“魔块”时，会反复调用search()，每一次BFS的过程中必定会有部分与前几次BFS重复，进行没必要的重复操作拉低效率