//Toy"魔块" 状态转换图+BFS

////#define NDEBUG
#include "stdafx.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//定义常量和全局变量**********************************************************************
struct Node {  //10byte 代表一个节点，用结构体struct“绑定”状态数组及其对应的状态数字
	unsigned char poses[8];
	short unsigned int pos_to_num; //注意：此处不能声明为short int(范围：[0, 32767])，而short unsigned int(范围[0， 65535])
	//而pos_to_num的范围为[0, 40319]，声明为short int的话，绝对会溢出
};
const int MAX_TOY = 40320; //8！= 40320，即该玩具的最大状态数
const int fac[8] = { 1, 1, 2, 6, 24, 120, 720, 5040 }; //Cantor expansion中用到的0~7的阶乘
unsigned char HASH_TABLE[MAX_TOY]; //39KB 散列表
Node QUEUE[MAX_TOY]; //394KB 广搜队列
int ANSWERS[MAX_TOY];  //158KB 预处理--打表，本算法的精髓所在！！！！！！！！！！！！！！！！！！！！！！！


//辅助函数*************************************************************************
int cantor(Node &node) {  //康托展开 数据规模：n = 8 时间复杂度：O(n^2) = O(64)，假设CPU为1GHz，则时间可忽略不计，即O(1) 空间复杂度：所需额外空间为O(1)
	int temp, num = 0;
	for (int i = 0; i < 8; i++)
	{
		temp = 0;
		for (int j = i + 1; j < 8; j++)
		{
			if (node.poses[i] > node.poses[j]) temp++;
		}
		num += temp * fac[7 - i];
	}

	return num;
}

void swap(unsigned char& a, unsigned char& b) {
	a = a^b;
	b = b^a;
	a = a^b;
}
void swapTester(unsigned char a, unsigned char b) {
	printf("%d %d => ", a, b);
	swap(a, b);
	printf("%d %d", a, b);
}

void turn(Node &node, int i) {

	if (0 == i)//上下翻转
	{
		swap(node.poses[0], node.poses[7]); swap(node.poses[1], node.poses[6]);
		swap(node.poses[2], node.poses[5]); swap(node.poses[3], node.poses[4]);
	}
	else if (1 == i)//循环右移
	{
		unsigned char old_position3 = node.poses[3];
		node.poses[3] = node.poses[2]; node.poses[2] = node.poses[1]; node.poses[1] = node.poses[0]; node.poses[0] = old_position3;
		unsigned char old_position4 = node.poses[4];
		node.poses[4] = node.poses[5]; node.poses[5] = node.poses[6]; node.poses[6] = node.poses[7]; node.poses[7] = old_position4;
	}
	else if (2 == i)//中心顺时针旋转
	{
		unsigned char old_position6 = node.poses[6];
		node.poses[6] = node.poses[5]; node.poses[5] = node.poses[2]; node.poses[2] = node.poses[1]; node.poses[1] = old_position6;
	}
	else assert(false);

}


//初始化**************************************************************************************
void init() {
	memset(HASH_TABLE, 0, MAX_TOY*sizeof(unsigned char));//初始化散列表HASH_TABLE[]
	int front = 0, rear = 0; //队列的队头指针和队尾指针

	Node node = { { 1, 2, 3, 4, 5, 6, 7, 8 }, 0 }; //表征“魔块”的还原态的节点node

	QUEUE[front] = node; HASH_TABLE[node.pos_to_num] = 1; //初始化散列表和广搜队列的首元素
	Node new_node; //即将要进行turn的魔块的副本

	while (front <= rear) {
		Node now = QUEUE[front]; front++; //出队，对now进行三种模式的turn操作

		for (int mode = 0; mode < 3; mode++)
		{
			new_node = now; //复制构造
			turn(new_node, mode);
			new_node.pos_to_num = cantor(new_node);
			////调试
			//for (int i = 0; i < 7; i++)
			//{
			//	printf("%d ", NUM_TO_POS[result][i]);
			//}
			//printf("\n");

			if (HASH_TABLE[new_node.pos_to_num] == 0) {
				ANSWERS[new_node.pos_to_num] = HASH_TABLE[now.pos_to_num]; //精髓所在!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

				HASH_TABLE[new_node.pos_to_num] = HASH_TABLE[now.pos_to_num] + 1;
				rear++; QUEUE[rear] = new_node;
			}
		}
	}
}



//主函数************************************************************************************
int main()
{
	init(); //初始化-打表

//#define NREDIR
#ifndef NREDIR
	freopen("input.txt", "r", stdin);
#endif


	int num_toy; scanf("%d", &num_toy);
	if (0 == num_toy) return 0;
	unsigned char pos[8];
	unsigned char output[1000];
	for (int i = 0; i < num_toy; i++)
	{
		unsigned char code_table[10];
		
		for (int j = 0; j < 8; j++)
		{
			scanf("%d", pos + j);
		}

		//利用组合数学的“置换”，把初始状态置换为12345678，然后按照相同的code_table[]置换目标状态12345678
		//（其实置换出来和初始状态是一样的，但是为了使该算法可以"扩展"为指定初始状态和目标状态，然后求解的功能，所以写了两个循环）
		for (int i = 0; i < 8; i++)
		{
			code_table[pos[i]] = i + 1;
		}
		Node node;
		for (int i = 0; i < 8; i++)
		{
			node.poses[i] = code_table[i + 1];
		}

		output[i] = ANSWERS[cantor(node)];
	}

	for (int i = 0; i < num_toy; i++)
	{
		printf("%d\n", output[i]);
	}

	return 0;
}

