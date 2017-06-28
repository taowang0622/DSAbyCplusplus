// Train Dispatching.cpp : 定义控制台应用程序的入口点。
//
#define NDEBUG

#include<cstdio>
#include<cassert>
#include <cstdlib>

//快速io , 用结构体struct简洁
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

enum Operation { PUSH, POP };

class Node {
public:
	int _data;
	Node* _pre;
	Node* _succ;

	Node() {}
	Node(int data) :_data(data), _pre(NULL), _succ(NULL) {}
};

class Queue {
private:
	Node *_header, *_trailer;
public:
	Queue() {
		_header = new Node();
		_trailer = new Node();
		_header->_succ = _trailer;
		_trailer->_pre = _header;
	}
	~Queue() {
		delete _header, _trailer;
	}

	//从队尾入队,然后返回新入队的元素的位置
	Node* enqueue(int data) {
		Node* new_node = new Node(data);
		new_node->_succ = _trailer;
		new_node->_pre = _trailer->_pre;
		_trailer->_pre->_succ = new_node;
		_trailer->_pre = new_node;

		return new_node;
	}

	//逆向入队,从对头入队
	Node* reverEnqueue(int data) {
		Node* new_node = new Node(data);
		new_node->_succ = _header->_succ;
		new_node->_pre = _header;
		_header->_succ->_pre = new_node;
		_header->_succ = new_node;

		return new_node;

	}

	//从队头出队，然后返回出队的元素的数据项的值
	int dequeue() {
		int front_data = _header->_succ->_data;
		Node* front_posi = _header->_succ;

		_header->_succ->_succ->_pre = _header;
		_header->_succ = _header->_succ->_succ;

		delete front_posi;
		return front_data;
	}

	//返回队头的位置,如果队列为空，则返回NULL
	Node* front() {
		return (_header->_succ == _trailer) ? NULL : _header->_succ;
	}

	//从front到rear，依次显示各个节点的数据项，用于调试
	void display() {
		printf("front: ");
		for (Node* posi = _header->_succ; posi != _trailer; posi = posi->_succ)
		{
			printf("%d ", posi->_data);
		}
		printf(" :rear\n");
	}
};

template<typename T>
class Stack {
private:
	int _size; //规模，需要“动态更新”
	int _capacity; //容量
	T* _elem;
public:
	Stack(int capacity) {
		_capacity = capacity;
		_size = 0;
		_elem = new T[_capacity];
	}
	Stack() :Stack(1000) {}
	~Stack() {
		delete[] _elem;
	}

	//压栈，如果栈溢出就返回false
	bool push(T data) {
		if (_size >= _capacity)
			return false;
		else {
			_elem[_size] = data;
			++_size;
			return true;
		}
	}

	//弹栈，返回弹出的元素
	T pop() {
		return _elem[--_size];
	}

	//判断栈是否为空
	bool empty() {
		return 0 == _size;
	}

	//判断栈是否已满
	bool full() {
		return _size == _capacity;
	}

	T top() {
		return _elem[_size - 1];
	}

	int size() {
		return _size;
	}

	//用于调试
	void display() {
		printf("top: ");
		for (int i = _size; i > 0; i--)
		{
			printf("%d ", _elem[i - 1]);
		}
		printf(" :bottom\n");
	}
};

void remove(Stack<int>& transfer_end, int& ordinal, Stack<Operation>& operations, Queue& entrance) {
	for (int i = transfer_end.size(); i > 0; i--)
	{
		if (ordinal == transfer_end.top()) {
			int temp = transfer_end.pop();
			operations.push(POP);
			ordinal--;
			entrance.reverEnqueue(temp); //便于调试
		}
		else
			break;
	}
}

bool feasible(Queue& exit, Queue& entrance, Stack<int>& transfer_end, int num_compart, Stack<Operation>& operations) {
	if (transfer_end.full()) return false; //退化情况
	
	int ordinal = num_compart;//车厢的最大序号为车厢的总数
	Node* cursor = exit.front();

	//while (num_compart--) {//从队头到队尾遍历一遍整个exit队列
	//	if (ordinal == cursor->_data) {
	//		if (transfer_end.full()) return false;
	//		entrance.reverEnqueue(ordinal); //找到ordinal就入entrance队列
	//										//entrance.display();
	//		operations.push(PUSH); operations.push(POP);
	//		--ordinal;//search下一个车厢
	//		remove(transfer_end, ordinal, operations, entrance);
	//	}
	//	else {
	//		//transfer_end.display();
	//		if (!transfer_end.empty()) {
	//			remove(transfer_end, ordinal, operations, entrance);
	//		}
	//		if (!transfer_end.empty()) {  //如果栈弹出后，栈还不为空的话
	//			if (cursor->_data < transfer_end.top())//如果压栈之前，要压栈的元素比栈顶元素小的话，肯定不可行
	//				return false;
	//		}
	//		if (!(operations.push(PUSH), transfer_end.push(cursor->_data))) {//如果栈已满，就输出No，结束程序
	//			printf("栈溢出\n");
	//			return false;
	//		}
	//	}
	//	cursor = cursor->_succ;//cursor往rear进一步
	//}

	for (int i = 0; i < num_compart; i++) {//从队头到队尾遍历一遍整个exit队列
		if (ordinal == cursor->_data) {
			if (transfer_end.full()) return false;
			entrance.reverEnqueue(ordinal); //找到ordinal就入entrance队列
			//entrance.display();
			operations.push(PUSH); operations.push(POP);
			--ordinal;//search下一个车厢
			remove(transfer_end, ordinal, operations, entrance);
		}
		else {
			//transfer_end.display();
			if (!transfer_end.empty()) {
				remove(transfer_end, ordinal, operations, entrance);
			}
			if (!transfer_end.empty()) {  //如果栈弹出后，栈还不为空的话
				if (cursor->_data < transfer_end.top())//如果压栈之前，要压栈的元素比栈顶元素小的话，肯定不可行
					return false;
			}
			if (!(operations.push(PUSH), transfer_end.push(cursor->_data))) {//如果栈已满，就输出No，结束程序
				printf("栈溢出\n");
				return false;
			}
		}
		cursor = cursor->_succ;//cursor往rear进一步
	}

	//printf("ordinal: %d\n", ordinal); 用于调试
	//transfer_end.display();
	while (ordinal) {//把exit遍历完之后，得到的栈应该“自顶向下”呈递减次序，如果不是，就不可行
		if ((operations.push(POP), ordinal != transfer_end.pop())) {
			return false;
		}
		else {
			entrance.reverEnqueue(ordinal);
		}
		ordinal--;
	}

	//entrance.display();
	return true;
}

//交换器
void swap(int& a, int& b) {
	int temp;
	temp = a;
	a = b;
	b = temp;
}

//数组置乱器
void permute(int arr[], int size) {
	for (int i = size; i > 0; i--) //自后向前
	{
		swap(arr[i - 1], arr[rand() % i]);
	}
}


int main()
{


	//生成测例
#define _REDIR_
#ifndef _REDIR_
	//freopen("test_cases.txt", "w", stdout);
	freopen("test_cases.txt", "r", stdin);
#endif
	int n = 5;
	/*int* cases = new int[n];
	for (int i = 0; i < n; i++) {
	cases[i] = i + 1;
	}
	for (int i = 0; i < 5*n; i++) {
	printf("%d ", n);
	printf("%d\n", (rand() % n) + 1);
	permute(cases, n);
	for (int i = 0; i < n; i++)
	{
	printf("%d ", cases[i]);
	}
	printf("\n");
	printf("\n");

	}*/

	/*for (int i = 0; i < 5*n; i++)
	{*/
	int num_compart, limit_tansfer;
	scanf("%d %d", &num_compart, &limit_tansfer);

	Queue exit, entrance;
	Stack<int> transfer_end(limit_tansfer);
	Stack<Operation> operations(2 * num_compart);
	int ordinal;//车厢的序号

				//输入最后在exit中车厢的序列
	for (int i = 0; i < num_compart; i++)
	{
		scanf("%d", &ordinal);
		exit.reverEnqueue(ordinal); //把最后输入的元素作为front，第一个输入的作为rear
	}

	if (feasible(exit, entrance, transfer_end, num_compart, operations)) {
		int size = operations.size();
		for (int i = 0; i < size; i++)
		{
			switch (operations.pop())
			{
			case PUSH:printf("pop\n"); break;
			case POP:printf("push\n"); break;
			}
		}
	}
	else
		printf("No");

	//调试
	/*printf("\n");
	transfer_end.display();
	entrance.display();
	printf("\n");*/
	/*}*/
	return 0;
}

