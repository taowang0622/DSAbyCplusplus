// scheduler.cpp : 任务调度(Schedule)
//
//#define NDEBUG //必须写在最上面，否则无效

#include <ctime>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <Windows.h>
using namespace std;
typedef int Rank;

//const int PERMU_MAX = 40320;
const int PERMU_MAX = 4000000;
const char TEST_STR[9] = "abcdefgh";
//char* ALL_PERMU[PERMU_MAX];
char* ALL_PERMU[PERMU_MAX];
//bool TABLE[8];
bool TABLE[26]; //映射a~z的一维表格

////快速io , 用结构体struct简洁
//const int BUFFER_SIZE = 1 << 20;  //把buffer设置为2^20个byte
//struct FastIO {
//	char inbuf[BUFFER_SIZE];
//	char outbuf[BUFFER_SIZE];
//
//	FastIO() {  //构造函数
//		setvbuf(stdin, inbuf, _IOFBF, BUFFER_SIZE);
//		setvbuf(stdout, outbuf, _IOFBF, BUFFER_SIZE);
//	}
//}io;


//词条Task
struct Task {
	long long prio_num;
	char* task_name;

	Task(long long prio = 0, char* name = NULL) :prio_num(prio), task_name(name) {}
	~Task() {}

	//重载比较器，自定义prio_num小的优先级高，构建出来的完全二叉堆是“小顶堆”
	bool operator < (Task& task) {
		assert(0 < prio_num);
		assert(((long long)1 << 32) > prio_num);
		assert(0 < task.prio_num);
		assert(((long long)1 << 32) > task.prio_num);

		if (prio_num > task.prio_num) return true;
		if (prio_num == task.prio_num) {
			assert(task_name != NULL);
			assert(task.task_name != NULL);
			if (0 < strcmp(task_name, task.task_name)) return true;
		}
		return false;
	}

	//当前任务的prio_num翻倍，如果翻倍后的值不小于2^32，则返回false
	bool doublePrioNum() {
		assert((0 < prio_num) && (((long long)1 << 32) > prio_num));

		return ((prio_num *= 2) >= ((long long)1 << 32)) ? false : true;
	}
};

//Task类对应的测试类
struct TaskTester {
	Task* ptr_obj;

	void caseBegin() {
		ptr_obj = new Task();
	}
	void caseEnd() {
		delete ptr_obj;
	}

	void operatorLt(long long prio_num_1, long long prio_num_2, char* name_1 = NULL, char* name_2 = NULL) {
		caseBegin(); {
			ptr_obj->prio_num = prio_num_1;
			ptr_obj->task_name = name_1;
			Task task(prio_num_2, name_2);
			bool result = (*ptr_obj) < task;
			assert(result == (prio_num_1 >= prio_num_2));
			printf("%lld比%lld的优先级低：%s\n", prio_num_1, prio_num_2, result ? "true" : "false");
		}caseEnd();
	}

	void doublePrioNum(long long prio) {
		caseBegin(); {
			ptr_obj->prio_num = prio;
			bool valid = ptr_obj->doublePrioNum();
			assert(ptr_obj->prio_num == (2 * prio));
			printf("origin: %lld current: %lld valid: %s\n", prio, ptr_obj->prio_num, valid ? "true" : "false");
		}caseEnd();
	}

};

//优先级队列_使用完全二叉堆实现
class PQ_ComplHeap {
	friend struct PQ_ComplHeap_Tester; //声明测试类为友元类

private:
	int _size;
	Task** _elem;

	/*****************************************************************************************/
	//交换节点i和节点j的值
	inline void swap(int i, int j) {
		Task* temp;
		temp = _elem[j];
		_elem[j] = _elem[i];
		_elem[i] = temp;
	}
	//判断_elem[i]是否合法
	inline bool inHeap(Rank i) {
		return (-1 < i) && (_size > i);
	}
	//节点i的右孩子的秩
	inline Rank rightChild(Rank i) {
		return (1 + i) << 1;
	}
	//节点i的左孩子的秩
	inline Rank leftChild(Rank i) {
		return 1 + (i << 1);
	}
	//判断是否存在右孩子
	inline bool validRC(Rank i) {
		return inHeap((1 + i) << 1);
	}
	//判断是否存在左孩子
	inline bool validLC(Rank i) {
		return inHeap(1 + (i << 1));
	}
	//返回最后一个内部节点的秩
	inline Rank lastInternal() {
		return ((_size - 1) - 1) >> 1;
	}
	//返回秩为i的节点及其孩子(至多三个)中优先级最大的节点的秩
	Rank properParent(Rank i);
	Rank properChild(Rank i);
	/***************************************************************************************************/
	//下滤															
	Rank percolateDown(Rank i);
	//建堆-----Floyd算法
	void heapify();

public:
	PQ_ComplHeap() {}
	PQ_ComplHeap(Task** tasks, int size);
	//析构函数
	~PQ_ComplHeap();

	Task* getMax();

	Task* delMax();

	//任务调度，m为预测的任务执行序列长度
	void scheduler(int m);

};

//返回秩为i的节点及其孩子(至多三个)中优先级最大的节点的秩
Rank PQ_ComplHeap::properParent(Rank i) {
	if (validRC(i)) { //情况一：有右孩子，必然有左孩子
		Rank lc = leftChild(i);
		Rank rc = rightChild(i);
		return (*_elem[i] < *_elem[lc]) ? ((*_elem[lc] < *_elem[rc]) ? rc : lc) : ((*_elem[i] < *_elem[rc]) ? rc : i);
	}
	else {
		if (validLC(i)) { //情况二：无右孩子，但有左孩子
			Rank lc = leftChild(i);
			return (*_elem[i] < *_elem[lc]) ? lc : i;
		}
		else  //情况三：既无左孩子，也无右孩子
			return i;
	}
}
Rank PQ_ComplHeap::properChild(Rank i) {
	if (validRC(i)) { //情况一：有右孩子，必然有左孩子
		Rank lc = leftChild(i);
		Rank rc = rightChild(i);
		return (*_elem[lc] < *_elem[rc]) ? rc : lc;
	}
	else {
		if (validLC(i)) { //情况二：无右孩子，但有左孩子
			return leftChild(i);
		}
		else  //情况三：既无左孩子，也无右孩子
			return i;
	}
}
//下滤
/*Rank PQ_ComplHeap::percolateDown(Rank i) {
	assert(inHeap(i));

	Rank j;
	while (i != (j = properParent(i))) {
		swap(i, j);
		i = j;
	}

	assert(inHeap(i));
	return i;
}*/
//下滤-----优化版,减少比较操作次数和赋值操作次数
Rank PQ_ComplHeap::percolateDown(Rank i) {
	assert(-1 < i);
	assert(i < _size);

	Task* temp = _elem[i];
	Rank j;

	while (i != (j = properChild(i))) {
		if (*_elem[j] < *temp) {
			_elem[i] = temp;
			return i;
		}
		else {
			_elem[i] = _elem[j];
			i = j;
		}
	}
	_elem[i] = temp;

	assert(-1 < i);
	assert(i < _size);
	return i;
}
//建堆-----Floyd算法
void PQ_ComplHeap::heapify() {
	assert(0 != _size);

	for (Rank i = lastInternal(); inHeap(i); i--)
	{
		percolateDown(i);
	}
}
//构造函数
PQ_ComplHeap::PQ_ComplHeap(Task** tasks, int size) {
	_elem = tasks;
	_size = size;
	heapify();
}
//析构函数
PQ_ComplHeap::~PQ_ComplHeap() {
	/*for (int i = 0; i < _size; i++)
	{
		delete _elem[i];
	}*/
}
Task* PQ_ComplHeap::getMax() {
	assert(0 != _size);
	return _elem[0];
}
Task* PQ_ComplHeap::delMax() {
	Task* max = getMax();
	if (1 < _size) {
		_elem[0] = _elem[--_size];
		percolateDown(0);
	}
	else _size--;
	return max;
}
//任务调度，m为预测的任务执行序列长度
void PQ_ComplHeap::scheduler(int m) {
	Task** excuted_sequ = new Task*[m];
	int j = 0;
	for (int i = 0; i < m; i++)
	{
		excuted_sequ[j++] = getMax();
		if (getMax()->doublePrioNum()) {
			assert(0 != _size);
			percolateDown(0);
		}
		else {
			delMax();
			if (0 == _size) break;
		}
	}

	/*for (int i = 0; i < j; i++) {
		printf("%s\n", excuted_sequ[i]->task_name);
	}*/
}


//PQ_ComplHeap对应的Tester
struct PQ_ComplHeap_Tester {
	PQ_ComplHeap* ptr_obj;
	void caseBegin() {
		ptr_obj = new PQ_ComplHeap();
	}
	void caseEnd() {
		delete ptr_obj;
	}

	void heapify(Task** tasks, int size, Task* target) {
		caseBegin(); {
			ptr_obj->_elem = tasks;
			ptr_obj->_size = size;
			ptr_obj->heapify();
			//assert(ptr_obj->getMax() == target);
			printf("优先级最高：（%lld, %s）\n", (ptr_obj->getMax())->prio_num, (ptr_obj->getMax())->task_name);
		}caseEnd();
	}

	void delMax(Task** tasks, int size, Task* target) {
		caseBegin(); {
			ptr_obj->_elem = tasks;
			ptr_obj->_size = size;
			ptr_obj->heapify();
			ptr_obj->delMax();
			assert(ptr_obj->getMax() == target);
			printf("delMax()之后，优先级最高：（%lld, %s）\n", (ptr_obj->getMax())->prio_num, (ptr_obj->getMax())->task_name);
		}caseEnd();
	}

};

////全排列字符串“abcdefgh”，并把结果存到数组ALL_PERMU[]中
//void permu(int rank = 0) {
//	static int ii = 0;
//	static char temp[9]; //静态局部变量(scope：局部 active duration：直到程序结束)，临时数组
//	if (7 < rank) {
//		char* str = new char[9];
//		int i = 0;
//		for (; i < 8; i++)
//		{
//			str[i] = temp[i];
//		}
//		str[i] = '\0';
//		ALL_PERMU[ii++] = str;
//	};//递归基
//	for (int i = 0; i < 8; i++) {
//		if (!TABLE[i]) {
//			assert(nullptr != temp);
//			temp[rank] = TEST_STR[i];
//			TABLE[i] = true;
//			permu(rank + 1);
//			TABLE[i] = false;
//		}
//	}
//}
//void permuTexter() {
//	for (int i = 0; i < 10; i++)
//	{
//		for (int j = 0; j < 8; j++)
//		{
//			cout << ALL_PERMU[i][j];
//		}
//		cout << endl;
//	}
//}

//从a~z中排列8个字符组成的字符串，并把结果存到数组ALL_PERMU[]中
bool permu(int rank = 0) {
	static int ii = 0;
	static char temp[9]; //"静态局部变量"(scope：局部 active duration：直到程序结束)，临时数组
	if (7 < rank) { //递归基
		char* str = new char[9];
		int i = 0;
		for (; i < 8; i++)
		{
			str[i] = temp[i];
		}
		str[i] = '\0';
		ALL_PERMU[ii++] = str;
		if (PERMU_MAX <= ii) return false;
		else return true;
	};
	for (int i = 0; i < 26; i++) {
		if (!TABLE[i]) {
			assert(nullptr != temp);
			temp[rank] = 'a' + i;
			TABLE[i] = true;
			if (!permu(rank + 1)) return false;
			TABLE[i] = false;
		}
	}
	return true;
}
void permuTexter() {
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << ALL_PERMU[i][j];
		}
		cout << endl;
	}
}

int main()
{
	//long long x = 1;
	//printf("%lld\n", (x <<= 32));
	//printf("%lld\n", ((long long)(1) << 32));

	///************************************************************************************************/
	//TaskTester task_tester;
	////测试Task类的<运算符的“黑箱测试测例”
	//task_tester.operatorLt(10, 20);
	//task_tester.operatorLt(30, 20);
	////task_tester.operatorLt(20, ((long long)(1) << 32));
	//task_tester.operatorLt(20, 20, "world", "hello");
	////测试Task类的doublePrioNum()方法的“黑箱测试测例”
	//task_tester.doublePrioNum(100); //正常情况
	//task_tester.doublePrioNum((long long)(1) << 31); //边界情况
	////task_tester.doublePrioNum((long long)(1) << 32); //非法情况
	////task_tester.doublePrioNum(0); //非法情况
	///*****************************************************************************************/
	//PQ_ComplHeap_Tester PQ_tester;
	////测试PQ的heapify()的黑箱测试测例
	////正常情况
	//Task* tasks_1[5] = { new Task(50, "wang"), new Task(40, "tao"), \
	//	new Task(30, "is"), new Task(20, "an"), new Task(10, "idiot") };
	//PQ_tester.heapify(tasks_1, 5, tasks_1[4]);
	////退化情况
	//Task* tasks_2[5] = { new Task(20, "wang"), new Task(20, "tao"), \
	//	new Task(20, "is"), new Task(20, "an"), new Task(20, "idiot") };
	//PQ_tester.heapify(tasks_2, 5, tasks_2[3]);
	////边界情况
	////PQ_tester.heapify(NULL, 0, NULL);
	////测试PQ的delmax()的黑箱测试测例
	////正常情况
	//Task* tasks_3[5] = { new Task(50, "wang"), new Task(40, "tao"), \
	//	new Task(30, "is"), new Task(20, "an"), new Task(10, "idiot") };
	//PQ_tester.delMax(tasks_3, 5, tasks_3[3]);
	////退化情况
	//Task* tasks_4[5] = { new Task(20, "wang"), new Task(20, "tao"), \
	//	new Task(20, "is"), new Task(20, "an"), new Task(20, "idiot") };
	//PQ_tester.delMax(tasks_4, 5, tasks_4[4]);
	////边界情况
	////Task* tasks_5[1] = { new Task(10, "wang") };
	////PQ_tester.delMax(tasks_5, 1, NULL);
	///****************************************************************************************************************/
	////printf("%d", strcmp("Tao", "an"));  //注意大小写！！！！！！！！！！！！！！！！！！！！

	permu();
	//permuTexter();

	//为了精确计时，使用调用CPU主频的方案
	LARGE_INTEGER begin_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency); //获取CPU主频

	while (1) {
		cout << "请输入工作模式" << endl << "0.测试模式\t1.任务分配\t2.退出\t[ ]\b\b";
		int mode; cin >> mode;  //输入工作模式

		if (0 == mode) { //如果选择了测试模式的话
			unsigned int testcase_num;
			cout << "输入测例个数："; cin >> testcase_num;
			unsigned int task_num = INT_MAX;
			while (task_num > PERMU_MAX) { //需要调度的任务个数不超过4000000
				cout << "\n输入需要调度的任务个数："; cin >> task_num; cout << endl;
			}
			double all_time = 0, max_time = 0, min_time = 1000; //分别表示：总消耗时间，最长时间，最短时间
			int* time_table = nullptr;
			while (nullptr == time_table) time_table = new int[10000]; //建立时间统计数组
			for (int i = 0; i < 1000; i++) time_table[i] = 0;//初始化time_table
			int max_table = 0; //用于记录time_table中最大的元素，以方便演示

			for (int i = 0; i < testcase_num; i++)
			{
				srand(unsigned(clock()));
				int n = task_num, m = rand();
				Task** tasks = new Task*[n];  //delete!
				for (int j = 0; j < n; j++)
				{
					long long prio_num;
					while ((prio_num = rand()) <= 0);
					tasks[j] = new Task(prio_num, ALL_PERMU[j]); //delete!
				}
				QueryPerformanceCounter(&begin_time);
				if (0 == m) return 0;
				PQ_ComplHeap PQ(tasks, n);
				PQ.scheduler(m);
				QueryPerformanceCounter(&end_time);
				all_time += (double)(end_time.QuadPart - begin_time.QuadPart) / frequency.QuadPart * 1000; //更新总消耗时间
				time_table[int((double)(end_time.QuadPart - begin_time.QuadPart) / frequency.QuadPart * 1000)]++; //将当前耗时值加入统计表
				if ((double)(end_time.QuadPart - begin_time.QuadPart) / frequency.QuadPart * 1000> max_time)
				{//刷新最长时间值
					max_time = (double)(end_time.QuadPart - begin_time.QuadPart) / frequency.QuadPart * 1000;
				}
				if ((double)(end_time.QuadPart - begin_time.QuadPart) / frequency.QuadPart * 1000 < min_time)
				{//刷新最小时间值
					min_time = (double)(end_time.QuadPart - begin_time.QuadPart) / frequency.QuadPart * 1000;
				}
				//输出已解的情况，暂时不妨忽略%左右的系统偏差
				cout << "已测试" << i + 1 << "个\t平均" << all_time / (i + 1) << "ms\t最长耗时" << max_time << "ms" << endl;
				
				//释放测例
				for (int i = 0; i < n; i++)
				{
					assert(nullptr != tasks[i]);
					delete tasks[i];
				}
				delete[] tasks;

			}

		}
	}

	/*int n, m;
	scanf("%d %d", &n, &m);
	if (0 == n) return 0;

	Task** tasks = new Task*[n];
	for (Rank i = 0; i < n; i++)
	{
		long long prio_num;
		char* task_name = new char[10];
		scanf("%lld %s", &prio_num, task_name);
		tasks[i] = new Task(prio_num, task_name);
	}

	if (0 == m) return 0;
	PQ_ComplHeap PQ(tasks, n);
	PQ.scheduler(m);*/

	return 0;
}

