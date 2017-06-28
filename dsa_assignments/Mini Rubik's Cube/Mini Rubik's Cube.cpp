#include<iostream>
#include<ctime>
#include<windows.h>
using namespace std;

//函数声明-------------------------------------------------------------------------------------
void turn(int i);
//对now0数组执行某个转动后赋给now
void uzip(int i);
//将状态数字i解压缩到now0数组
int zip();
//将now数组压缩，返回状态数字
void pret();
//now0赋给now
void pre();
//初始化，调用prePos，然后生成TurnPos与TurnOri数组
void prePos(int);
//初始化全排列，生成排列与位置码的对应数组
void output(int, int);
//生成并输出结果
int input();
//输入并判断是否合法、可解
int search(int i, char mode);
//搜索状态数字为i的状态，如mode=0，不输出结果
int searchall();
//搜索二阶所有状态
void count();
//统计所有二阶状态

//常量声明---------------------------------------------------------------------------------------
const int T = 5;
//树状图采样密度
const int MAX_CUBE = 3674160;
//总状态数
const int MAX_POS = 5040;
//总位置状态数
const int MAX_ORI = 729;
//总方向状态数

//全局变量声明------------------------------------------------------------------------------------
unsigned char ht[MAX_CUBE];
//3588K|分为部分：
//ht[i]/128部分：为表示从目标生成的状态，否则表示从还原态生成的状态
//ht[i]%128%13部分：该状态距离还原态的距离，即还原态，表示无限远即还未搜索到
//ht[i]%128/13部分：表示达到该状态前所转动的那一步代码
int Queue[MAX_CUBE];
//14352K|搜索队列
int now[7 + 7] = { 0,1,2,3,4,5,6,0,0,0,0,0,0,0 }, now0[7 + 7];
//前位表示某位置上的块原来是哪个，后位表示在某个位置的块的方向
//now0表示当前正在扩展的节点，now表示当前搜索到等待判重的节点
int z, z0;
//now数组的压缩编码,now0数组的压缩编码
bool PosNumUsed[7];
//prePos过程中表示某个数是否被使用过
unsigned char Possearch[7];
//枚举某个排列
unsigned char Num_to_Pos[MAX_POS][7];
//34.5K|Num_to_Pos[i]数组的个元素表示第i个排列的相应数字
short int Pos_to_Num[7][7][7][7][7][7];
//115K|某个排列在所有排列中排的位置，排列
short int TurnPos[MAX_POS][9];
//88.6K|返回将位置状态i转动j后的位置状态
short int TurnOri[MAX_ORI][9];
//12.8K|返回将方向状态i转动j后的方向状态
int p1 = 0, p2 = 0;
//指针，分别指向队首、队尾
int ziptot = 0;
//统计当前一共枚举几个全排列
int main()
{
	//为了精确计时，本程序中使用了调用CPU主频数的方案
	LARGE_INTEGER BegainTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);//获取CPU主频
	QueryPerformanceCounter(&BegainTime);//设定计时开始
	pre();//初始化
	QueryPerformanceCounter(&EndTime);//设定计时结束
	cout << "初始化时间：" << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000 << "ms" << endl;
	//输出初始化时间
	while (1)
	{
		cout << "请输入工作模式：" << endl << "0.测试模式\t1.解二阶魔方\t2.退出\t[ ]\b\b";
		char mode;
		cin >> mode;//输入工作模式
		cout << endl;
		if (mode == '0')
		{//如果选择了测试模式
			char testmode;
			while (1)
			{
				cout << "请指定测试项目：" << endl << "0.搜索二阶所有状态\t1.随机解二阶魔方计 \
					时\t2.返回\t[]\b\b";
				cin >> testmode;//输入测试项目
				cout << endl;
				if (testmode == '0')
				{//测试项目=搜索二阶所有状态
					searchall();//执行搜索并统计结果
					system("pause");//暂停等待
					continue;//继续执行到输入测试项目步骤
				}
				else if (testmode == '1')
				{//测试项目=随机解二阶魔方计时
					unsigned int num, i;//num记录测试个数
					int p20, p10;//记录耗时最长的时候的搜索量
					cout << "测试个数："; cin >> num;//输入测试个数
					double alltime, maxtime, mintime;//分别表示：总消耗时间，最长时间，最短时间
					alltime = 0; maxtime = 0; mintime = 1000;//初始化时间
					int *timetable = NULL;
					while (timetable == NULL)timetable = new int[1000];
					//建立时间统计数组
					int maxtable; maxtable = 0;//用于记录timetable中最大的元素以方便演示
					for (i = 0; i < 1000; i++)timetable[i] = 0;//清空timetable
					for (i = 0; i<num; i++)
					{
						srand(unsigned(clock()));//初始化随机数表
						QueryPerformanceCounter(&BegainTime);//记录开始时间

						search(unsigned int(rand() % MAX_CUBE), mode);//随机搜索某个
						QueryPerformanceCounter(&EndTime);//记录结束时间
						alltime += (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000; //更新总消耗时间
						timetable[int((double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000 * T)]++; //将当前耗时值加入统计表
						if ((double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000>maxtime)
						{//如果本次耗时超过了原最长时间值，记录本次搜索量，同时刷新最长时间值
							p10 = p1;
							p20 = p2;
							maxtime = (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000;
						}
						if ((double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000 < mintime)
						{//刷新最小时间值
							mintime = (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000;
						}
						if (i % 100 == 99)
						{//输出已解魔方的情况，暂时不妨忽略%左右的系统偏差
							cout << "已测试" << i + 1 << "个\t平均" << alltime / (i + 1) << "ms\
								长" << maxtime << "ms(" << p10 << " / " << p20 << ")\t" << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b \
								\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b \
								\b\b\b\b\b";
						}
					}
					//以下输出最终统计结果及时间分布
					cout << "已测试：" << i << "个平均每个" << alltime / i << "ms 平均每 \
						秒：" << 1000 * i / alltime << "个。最长耗时" << maxtime << "ms" << endl;
					cout << "时间分布：" << endl;
					for (i = (int)(mintime*T); i < maxtime*T; i++)
					{
						if (timetable[i] > maxtable)maxtable = timetable[i];
					}
					int j;
					if (maxtime > 10)maxtime = 10;//默认最长时间不超过ms，以防止因CPU中断导致的时间过长的无效输出
					for (i = (int)(mintime*T); i < maxtime*T; i++)
					{
						cout << (double(i)) / T << "ms~" << ((double(i + 1)) / T) << "ms:\t";
						for (j = 1; j < (int)(timetable[i] / (double(maxtable)) * 50); j++)
						{
							cout << '|';
						}
						cout << endl;
					}
					//以上输出最终统计结果及时间分布
					delete[] timetable; timetable = NULL;//释放内存
					cout << endl;
				}
				else
				{//测试项目等于其他值
					break;//返回
				}
			}
		}
		else if (mode == '1')
		{//如果选择了还原魔方模式
			while (1)
			{
				int f;//用于保证输入状态合法性并传递数据，还原后用来作为是否继续求解的表示参数
				while ((f = input()) == -1);//获取输入状态并保证其合法
				if (f == -2)break;
				QueryPerformanceCounter(&BegainTime);//开始计时
				search(f, mode);
				QueryPerformanceCounter(&EndTime);//结束计时
				cout << "此次求解共搜索了" << p2 << "个状态，总共耗时：" << (double)
					(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000 << "ms";
				//输出此次还原相关信息
				cout << "继续求解?(0.Yes\t1.No)\t[ ]\b\b";
				cin >> f;//是否继续求解？
				cout << endl;
				if (f != 0)break;//如果不继续，直接返回模式选择界面
			}
		}
		else break;//如果是其他模式，直接调出循环后将程序
	}
	return 0;
}

//I\O----------------------------------------------------------------------------------------------------------
int input()
{
	int colour, i;
	int cubecolour[6];//配色，RUFLDB的顺序
	bool checkcolour[6];//检查配色是否摆满
	for (i = 0; i < 6; i++)checkcolour[i] = false;
	cout << "请按照以下提示输入魔方" << endl;
	cout << "是否使用默认配色？.是;1.否\t[ ]\b\b";
	cin >> colour;
	if (colour != 0)
	{//是否需要重新定义配色？是：
		cout << "请按RUFLDB面的顺序输入您魔方还原后的六面颜色：" << endl;
		cout << "0.橘色;1.黄色;2.绿色;3.红色;4.白色;5.蓝色;" << endl;
		cout << "R面颜色：\t[ ]\b\b"; cin >> cubecolour[0];
		cout << "U面颜色：\t[ ]\b\b"; cin >> cubecolour[1];
		cout << "F面颜色：\t[ ]\b\b"; cin >> cubecolour[2];
		cout << "L面颜色：\t[ ]\b\b"; cin >> cubecolour[3];
		cout << "D面颜色：\t[ ]\b\b"; cin >> cubecolour[4];
		cout << "B面颜色：\t[ ]\b\b"; cin >> cubecolour[5];
		for (i = 0; i < 6; i++)checkcolour[cubecolour[i]] = true;
		for (i = 0; i < 6; i++)if (checkcolour[i] == false)
		{//判断输入是否合法，如果非法
			int ret;
			cout << "输入错误！是否重新输入？.重新输入;1.返回" << endl;
			cin >> ret;//输入是否继续
			if (ret == 0)return -1;//是：返回-1后将重新执行input
			else return -2;//否：返回-2后跳出循环到上一层
		}
	}
	else
	{//是否需要重新定义配色？否：
		for (i = 0; i < 6; i++)cubecolour[i] = i;//初始化默认配色
	}
	char *(c[6]) = { "橘色","黄色","绿色","红色","白色","蓝色" };
	int position;
	bool checkPos[7];//用于检查是否每一块都被输入
	for (i = 0; i < 7; i++)checkPos[i] = false;
	cout << "请找出同时包 \
		含" << c[cubecolour[5]] << ", " << c[cubecolour[4]] << ", " << c[cubecolour[3]] << "的块，并将它放于BDL位 \
		置，同时使得" << c[cubecolour[4]] << "位于D面上" << endl;
	cout << "下面进行位置信息的输入：" << endl; \
		cout << "请找出同时包 \
		含" << c[cubecolour[1]] << ", " << c[cubecolour[2]] << ", " << c[cubecolour[3]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 0; checkPos[position] = true;
	cout << "请找出同时包 \
		含" << c[cubecolour[1]] << ", " << c[cubecolour[3]] << ", " << c[cubecolour[5]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 1; checkPos[position] = true;
	cout << "请找出同时包 \
		含" << c[cubecolour[0]] << ", " << c[cubecolour[1]] << ", " << c[cubecolour[5]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 2; checkPos[position] = true;
	cout << "请找出同时包 \
		含" << c[cubecolour[0]] << ", " << c[cubecolour[1]] << ", " << c[cubecolour[2]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 3; checkPos[position] = true;
	cout << "请找出同时包 \
		含" << c[cubecolour[2]] << ", " << c[cubecolour[3]] << ", " << c[cubecolour[4]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 4; checkPos[position] = true;
	cout << "请找出同时包 \
		含" << c[cubecolour[0]] << ", " << c[cubecolour[4]] << ", " << c[cubecolour[5]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 5; checkPos[position] = true;
	cout << "请找出同时包 \
		含" << c[cubecolour[0]] << ", " << c[cubecolour[2]] << ", " << c[cubecolour[4]] << "的块，并输入它的位 \
		置 : " << endl;
	cout << "该块的位置：.ULF;1.ULB;2.URB;3.URF;4.DLF;5.DRB;6.DRF\t[ ]\b\b";
	cin >> position; now[position] = 6; checkPos[position] = true;
	//位置信息输入完毕，下面开始检查输入是否合法
	for (i = 0; i < 7; i++)if (!checkPos[i])
	{//如果有某一个块没有被输入
		int ret;
		cout << "输入错误！是否重新输入？.重新输入;1.返回" << endl;
		cin >> ret;
		if (ret == 0)return -1;
		else return -2;
	}
	cout << "下面进行方向信息的输入：" << endl;
	cout << "请输入ULF块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
		或R; 2.F或B\t[]\b\b";
	cin >> now[7];
	cout << "请输入ULB块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
		或R; 2.F或B\t[]\b\b";
	cin >> now[8]; now[8] = (3 - now[8]) % 3;
	cout << "请输入URB块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
			或R; 2.F或B\t[]\b\b";
	cin >> now[9];
	cout << "请输入URF块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
			或R; 2.F或B\t[]\b\b";
	cin >> now[10]; now[8] = (3 - now[8]) % 3;
	cout << "请输入DLF块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
			或R; 2.F或B\t[]\b\b";
	cin >> now[11]; now[8] = (3 - now[8]) % 3;
	cout << "请输入DRB块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
			或R; 2.F或B\t[]\b\b";
	cin >> now[12]; now[8] = (3 - now[8]) % 3;
	cout << "请输入DRF块上" << c[cubecolour[1]] << "或" << c[cubecolour[4]] << "所处在的面：.U或D;1.L \
			或R; 2.F或B\t[]\b\b";
	cin >> now[13];
	//方向信息输入完毕，下面检查方向信息是否合法，即方向码总和被三整除
	int checkOri = 0;
	for (i = 7; i < 14; i++)
	{
		checkOri += now[i];
	}
	if (checkOri % 3 != 0)
	{//如果方向信息输入非法
		int ret;
		cout << "输入错误！是否重新输入？.重新输入;1.返回" << endl;
		cin >> ret;
		if (ret == 0)return -1;
		else return -2;
	}
	//如果以上输入全部合法，返回状态压缩后的值，即状态编码
	return zip();
}
void output(int i, int cube0)
{//输出解法，输入为：最后一步搜索结果i，还原状态编码cube0。由于ht表中只储存某个状态的之前一步，故需要在输出过程中一步步从双向广搜重合的状态分别往目标状态与还原态进行转动
	int j;
	
	if (ht[z0] / 128 == 0)
	{//如果最后一步是从还原状态出发进行的节点扩展，就交换，使得从output过程看来最后一步是从目标节点的扩展以统一之后的运算
		j = z0;
		z0 = z;
		z = j;
		i = (i / 3) * 3 + (2 - i % 3);
	}
	int solve[10];//由于从当前节点到目标节点的转动步骤与最后的输出步骤相反，故需要借助栈结构来储存
	int p = 0;
	char *(ansT[9]) = { "R","R2","R'","U","U2","U'","F","F2","F'" };
	cout << "该魔方解法如下：" << endl;
	while (z0 != cube0)
	{//转动魔方直到到达目标节点，并每次将转动过程压入栈
		solve[p++] = ht[z0] % 128 / 13;
		z0 = TurnPos[z0 / MAX_ORI][((ht[z0] % 128 / 13) / 3) * 3 + 2 -
			(ht[z0] % 128 / 13) % 3] * MAX_ORI + TurnOri[z0%MAX_ORI][((ht[z0] % 128 / 13) / 3) * 3 + 2 -
			(ht[z0] % 128 / 13) % 3];
	}
	for (j = p - 1; j >= 0; j--)cout << ansT[solve[j]] << ' ';//输出上面的转动
	cout << ansT[i] << ' ';//输出最后一次搜索的转动步骤
	while (z != 0)
	{//转动魔方直到还原，并每次输出
		cout << ansT[((ht[z] % 128 / 13) / 3) * 3 + 2 - (ht[z] % 128 / 13) % 3] << ' ';
		z = TurnPos[z / MAX_ORI][((ht[z] % 128 / 13) / 3) * 3 + 2 - (ht[z]
			% 128 / 13) % 3] * MAX_ORI + TurnOri[z%MAX_ORI][((ht[z] % 128 / 13) / 3) * 3 + 2 - (ht[z] % 128 / 13) % 3];
	}
	cout << endl;
}

//预定义函数-----------------------------------------------------------------------------------------------------------------
void pret()
{//now0赋给now
	for (int i = 0; i < 14; i++)now[i] = now0[i];
}
void turn(int i)
{//对now0数组执行某个转动后赋给now
	if (i == 0)
	{//R
		now[2] = now0[3];
		now[3] = now0[6];
		now[6] = now0[5];
		now[5] = now0[2];
		//以上位置变换，以下方向变换
		now[9] = (now0[10] + 2) % 3;
		now[10] = (now0[13] + 1) % 3;
		now[13] = (now0[12] + 2) % 3;
		now[12] = (now0[9] + 1) % 3;
	}
	else if (i == 1)
	{//R2
		now[2] = now0[6];
		now[6] = now0[2];
		now[3] = now0[5];
		now[5] = now0[3];
		//以上位置变换，以下方向变换
		now[9] = now0[13];
		now[13] = now0[9];
		now[10] = now0[12];
		now[12] = now0[10];
	}
	else if (i == 2)
	{//R'
		now[2] = now0[5];
		now[3] = now0[2];
		now[6] = now0[3];
		now[5] = now0[6];
		//以上位置变换，以下方向变换
		now[9] = (now0[12] + 2) % 3;
		now[10] = (now0[9] + 1) % 3;
		now[13] = (now0[10] + 2) % 3;
		now[12] = (now0[13] + 1) % 3;
	}
	else if (i == 3)
	{//U
		now[0] = now0[3];
		now[1] = now0[0];
		now[2] = now0[1];
		now[3] = now0[2];
		//以上位置变换，以下方向变换
		now[7] = now0[10];
		now[8] = now0[7];
		now[9] = now0[8];
		now[10] = now0[9];
	}
	else if (i == 4)
	{//U2
		now[0] = now0[2];
		now[2] = now0[0];
		now[3] = now0[1];
		now[1] = now0[3];
		//以上位置变换，以下方向变换
		now[7] = now0[9];
		now[9] = now0[7];
		now[10] = now0[8];
		now[8] = now0[10];
	}
	else if (i == 5)
	{//U'
		now[0] = now0[1];
		now[1] = now0[2];
		now[2] = now0[3];
		now[3] = now0[0];
		//以上位置变换，以下方向变换
		now[7] = now0[8];
		now[8] = now0[9];
		now[9] = now0[10];
		now[10] = now0[7];
	}
	else if (i == 6)
	{//F
		now[0] = now0[4];
		now[3] = now0[0];
		now[6] = now0[3];
		now[4] = now0[6];
		//以上位置变换，以下方向变换
		now[7] = (now0[11] + 1) % 3;
		now[10] = (now0[7] + 2) % 3;
		now[13] = (now0[10] + 1) % 3;
		now[11] = (now0[13] + 2) % 3;
	}
	else if (i == 7)
	{//F2
		now[0] = now0[6];
		now[6] = now0[0];
		now[3] = now0[4];
		now[4] = now0[3];
		//以上位置变换，以下方向变换
		now[7] = now0[13];
		now[13] = now0[7];
		now[10] = now0[11];
		now[11] = now0[10];
	}
	else if (i == 8)
	{//F'
		now[0] = now0[3];
		now[3] = now0[6];
		now[6] = now0[4];
		now[4] = now0[0];
		//以上位置变换，以下方向变换
		now[7] = (now0[10] + 1) % 3;
		now[10] = (now0[13] + 2) % 3;
		now[13] = (now0[11] + 1) % 3;
		now[11] = (now0[7] + 2) % 3;
	}
}
int zip()
{//将now数组压缩成数字
	int n = 0, m = 0, i;//n:位置编码;m:方向编码
	n = Pos_to_Num[now[0]][now[1]][now[2]][now[3]][now[4]][now[5]];//位置状态的编码直接读取表中的结果
	for (i = 7; i<13; i++)
	{//方向状态的编码进行三进制变换
		m *= 3;
		m += now[i];
	}
	return (n*MAX_ORI + m);
}
void uzip(int i)
{//将i解压缩到now0数组
	int m = i%MAX_ORI, n = i / MAX_ORI;//n:位置编码;m:方向编码
	now0[13] = 12;//初始化状态压缩时未被直接存储的方向状态点
	for (i = 12; i >= 7; i--)
	{//重现三进制每一位数，还原方向状态点
		now0[i] = m % 3;
		m /= 3;
		now0[13] -= now0[i];
	}
	now0[13] %= 3;//还原未被直接存储的方向状态点
	for (i = 6; i >= 0; i--)
	{//位置状态的还原直接调用表中结果
		now0[i] = Num_to_Pos[n][i];
	}
}

//初始化------------------------------------------------------------------------------------------------------------------
void prePos(int i)
{//构造位置码表（枚举排列中的第i+1位）
	if (i == 7)
	{//如果排列的前位都枚举完了，将当前排列输入位置码表
		for (int j = 0; j<7; j++)Num_to_Pos[ziptot][j] = Possearch[j];
		Pos_to_Num[Possearch[0]][Possearch[1]][Possearch[2]][Possearch[3]][Possearch[4]]
			[Possearch[5]] = ziptot;
		ziptot++;//相应的位置码+1
	}
	else
	{//如果当前正在枚举排列中的前位
		for (Possearch[i] = 0; Possearch[i]<7; Possearch[i]++)if (!PosNumUsed[Possearch[i]])
		{//用~6对第i位进行尝试，如果该数字没有被用到
			PosNumUsed[Possearch[i]] = true;//标明该数字第i+1位已经用了
			prePos(i + 1);//枚举下一位（i+2位）
			PosNumUsed[Possearch[i]] = false;//标明该数字已尝试完全
		}//否则进行下一次尝试
	}
}
void pre()
{
	prePos(0);
	int i, j, k;
	for (i = 0; i<MAX_ORI; i++)
	{//由于方向状态比位置状态少，为了减少运算量，在计算方向状态的时候不妨就计算掉一部分的位置状态
		uzip(i*MAX_ORI + i);//解压方向与位置编码
		for (j = 0; j<9; j++)
		{
			pret();//初始化当前状态
			turn(j);//进行某个转动
			k = zip();//得到转动后的状态数字
			TurnPos[i][j] = k / MAX_ORI;
			TurnOri[i][j] = k%MAX_ORI;
			//分别储存状态数字的方向部分与位置部分
		}
	}
	for (i = MAX_ORI; i<MAX_POS; i++)
	{//计算位置状态
		uzip(i*MAX_ORI);
		for (j = 0; j<9; j++)
		{
			pret();//初始化当前状态
			turn(j);//进行某个转动
			k = zip();//得到转动后的状态数字
			TurnPos[i][j] = k / MAX_ORI;
			//仅需储存状态数字的位置部分
		}
	}
}

//以上初始化----------------------------------------------------------------------------------------------------
int searchall()
{
	LARGE_INTEGER BegainTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BegainTime);
	//设定计时开始
	int i, k, maxlen = 0;
	memset(ht, 0, MAX_CUBE*sizeof(unsigned char));
	p1 = 0; p2 = 0;
	Queue[0] = 0; ht[0] = 1 + 9 * 13;
	//初始化哈希表、广搜队列头元素
	while (p1 <= p2)
	{//判断广搜队列指针
		k = ht[Queue[p1]] % 128 % 13;
		z0 = Queue[p1]; p1++;//提取z0节点信息
		for (i = 0; i<9; i++)
		{//尝试各种转动方案
			z = TurnPos[z0 / MAX_ORI][i] * MAX_ORI + TurnOri[z0%MAX_ORI][i];//对魔方进行代码为i的转动
			if (ht[z] != 0); else
			{//如果发现的节点已经在先前被发现，就不做任何操作，否则储存该节点信息并调整队尾指针
				ht[z] = k + 1 + 13 * i + 128 * (ht[z0] / 128);
				p2++;
				Queue[p2] = z;
			}
		}
		if (maxlen<k)
		{//每次扫描都记录当前搜索到的最远距离，如果有更新
			maxlen = k;
			QueryPerformanceCounter(&EndTime);
			cout << "当前搜索的状态数目为（已扩展完/总数）：" << p1 << '/' << p2 << "，总消耗时 \
				间：" << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart * 1000 << "ms，当前长 \
				度 : " << k - 1 << endl;
		}
	}
	QueryPerformanceCounter(&EndTime);
	cout << "完成搜索，总耗时：" << (double)(EndTime.QuadPart -
		BegainTime.QuadPart) / Frequency.QuadPart * 1000 << "ms" << endl;
	count();
	return 0;
}
int search(int t, char mode)
{//搜索状态数字为t的二阶状态
	int i, k;
	if (t == 0) {
		cout << "已还原" << endl;
		return 0;
	}
	memset(ht, 0, MAX_CUBE*sizeof(unsigned char));
	p1 = 0; p2 = 0;
	Queue[0] = 0; ht[0] = 1 + 9 * 13;
	p2++; Queue[p2] = t; ht[t] = 1 + 9 * 13 + 128;
	//初始化哈希表、广搜队列头元素
	while (p1 <= p2)
	{//判断广搜队列
		k = ht[Queue[p1]] % 128 % 13; //提取z0状态
		z0 = Queue[p1]; p1++;
		for (i = 0; i<9; i++)
		{
			z = TurnPos[z0 / MAX_ORI][i] * MAX_ORI + TurnOri[z0%MAX_ORI][i];//对魔方进行代码为i的转动
			if (((ht[z] % 128 % 13) != 0) && (ht[z] / 128 + ht[z0] / 128 == 1))
			{//如果发现的新的节点与当前扩展的节点分别来自目标节点与还原节点，即找到解法
				if (mode == '1') {//如果是解魔方模式就输出解法，否则返回直接统计时间
					output(i, t);
				}
				return 0;
			}
			else if (ht[z] != 0); else
			{//如果没有找到解法但发现了一个新的节点，则储存该节点，并调整队尾指针
				ht[z] = k + 1 + 13 * i + 128 * (ht[z0] / 128);
				p2++;
				Queue[p2] = z;
			}
		}
	}
	return 0;
}
void count()
{
	int maxi = 0;
	int i;
	int len[200];
	for (i = 0; i<200; i++)len[i] = 0;
	for (i = 0; i<MAX_CUBE; i++)
	{//扫描每一个节点
		if ((ht[i] % 128 % 13) != 0)
		{//如果该节点被访问过，记录该节点的长度信息
			maxi = i;
			len[ht[i] % 128 % 13]++;
		}
		else;
	}
	for (i = 1; i<13; i++)
	{//输出统计结果
		cout << "最短还原步数恰为" << i - 1 << "步的情况数为：" << len[i] << "个" << endl;
	}
}