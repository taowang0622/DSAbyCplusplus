// tunnel.cpp : 定义控制台应用程序的入口点。
//

//#define NDEBUG

#include <cstdio>
#include <cassert>
#include <cstdlib>

//快速io
//const int BUFFER_SIZE = (1 << 20);
//struct fastIO {
//	char buffer_in[BUFFER_SIZE];
//	char buffer_out[BUFFER_SIZE];
//
//	fastIO() { //构造函数
//		setvbuf(stdout, buffer_out, _IOFBF, BUFFER_SIZE);
//		setvbuf(stdin, buffer_in, _IOFBF, BUFFER_SIZE);
//	}
//} io;

class Stack {
private:
	struct Elem { int data; int counter; }; //镜像栈中每个元素所属的类型

	int _size; //原栈规模，需要“动态更新”
	int _mirror_size; //镜像规模，，需要“动态更新”
	int _capacity; //容量
	int* _elem;
	Elem* _mirror; //当前栈的“镜像”，与当前栈的元素一一对应
public:
	Stack(int capacity) {
		_capacity = capacity;
		_size = 0;
		_mirror_size = 0;
		_elem = new int[_capacity];
		_mirror = new Elem[_capacity];
	}
	Stack():Stack(10000){}
	~Stack() {
		delete[] _elem;
		delete[] _mirror;
	}

	//压栈，如果栈溢出就返回false
	bool push(int data) {
		if (full())
			return false;
		else {
			//原栈压栈
			assert(_size < _capacity);
			_elem[_size] = data;
			++_size; //动态更新原栈的_size

					 //镜像压栈
			if (0 == _mirror_size) {
				Elem a_elem = { data, 1 };
				_mirror[_mirror_size] = a_elem;//边界情况
				_mirror_size++;
			}
			else if (data <= _mirror[_mirror_size - 1].data) {
				assert((_size - 1) >= 0); //调试
				_mirror[_mirror_size - 1].counter++;
			}
			else {
				Elem a_elem = { data, 1 };
				_mirror[_mirror_size] = a_elem;
				_mirror_size++;
			}

			return true;
		}
	}

	//弹栈，返回弹出的元素,如果栈为空的情况下弹栈，就返回-1
	int pop() {
		if (!empty()) {
			if (!(--_mirror[_mirror_size - 1].counter)) {
				_mirror_size--;
			}
			return _elem[--_size];
		}
		else
			return -1;
	}

	//判断栈是否为空
	bool empty() {
		return 0 == _size;
	}

	//判断栈是否已满
	bool full() {
		return _size == _capacity;
	}

	//引用栈顶元素
	int top() {
		return _elem[_size - 1];
	}

	int size() {
		return _size;
	}

	//高效获取栈中的最大元素，渐进时间复杂度为O(1)
	//约定：如果镜像栈的话，返回-1
	int getMax() {
		//assert((_mirror_size - 1) >= 0);
		if (0 == _mirror_size) return -1;
		return _mirror[_mirror_size - 1].data;
	}

	//把当前栈“倾倒”入栈target中
	void dumpTo(Stack& target) {
		int size = _size; //当前栈的size，以防pop和push时_size动态更新，影响迭代循环次数
		for (int i = 0; i < size; i++)
		{
			target.push(pop());
		}
	}

	//用于调试
	void display() {
		printf("top: ");
		for (int i = _size; i > 0; i--)
		{
			printf("%d ", _elem[i - 1]);
		}
		printf(" :bottom\n");

		printf("mirror -> top: ");
		for (int i = _mirror_size; i > 0; i--)
		{
			printf("(data:%d counter:%d) ", _mirror[i - 1].data, _mirror[i - 1].counter);
		}
		printf(" :bottom\n");
	}
};


//用来个栈来实现一个队列
class Queue {
private:
	Stack* tail;
	Stack* head;
public:
	Queue(int capacity) {
		tail = new Stack(capacity);
		head = new Stack(capacity);
	}
	Queue():Queue(10000){}
	~Queue() {
		delete tail, head;
	}

	void enqueue(int data) {
		assert(!tail->full());//栈tail不能满
		tail->push(data);
	}

	int dequeue() {
		if (head->empty()) {//如果栈head中为空，就把tail中的元素“倾倒”入其中，在从head中弹出
			tail->dumpTo(*head);
			return head->pop();
		}
		else
			return head->pop();
	}

	//高效获取最大值，O(1)
	//因为事先在Stack类中约定好了，如果栈为空的话，返回-1
	int getMax() {
		return (tail->getMax() < head->getMax()) ? head->getMax() : tail->getMax();
	}

	//调试
	void display() {
		printf("tail栈：\n"); tail->display();
		printf("head栈：\n"); head->display();
	}
};


int main()
{
	//	//生成测例
	////#define _REDIR_
	//#ifndef _REDIR_
	//	//freopen("push_test_cases.txt", "w", stdout);
	//	freopen("push_test_cases.txt", "r", stdin);
	//#endif
	//	int num_cases = 50;
	//	int size = 5;
	//	/*for (int i = 0; i < num_cases; i++)
	//	{
	//		for (int j = 0; j < size; j++) {
	//			printf("%d ", rand() % 20);
	//		}
	//		printf("\n");
	//		printf("\n");
	//	}*/
	//
	//	//test
	//	for (int i = 0; i < num_cases; i++)
	//	{
	//		//Stack test;
	//		Queue test(size);
	//		int temp;
	//		/*for (int i = 0; i < size; i++)
	//		{
	//			scanf("%d", &temp);
	//			test.push(temp);
	//		}*/
	//
	//		for (int i = 0; i < size; i++)
	//		{
	//			scanf("%d", &temp);
	//			test.enqueue(temp);
	//		}
	//		printf("入队：\n");
	//		test.display();
	//		printf("队列中各元素的最大值为：%d\n", test.getMax());
	//		printf("\n");
	//
	//		printf("出队：\n");
	//		for (int i = 0; i < size; i++)
	//		{
	//			printf("元素%d出队\n", test.dequeue());
	//			test.display();
	//			printf("队列中各元素的最大值为：%d\n", test.getMax());
	//			printf("\n");
	//		}
	//
	//		/*printf("压栈：\n");
	//		test.display();
	//		printf("栈中各元素的最大值为：%d\n", test.getMax());
	//		printf("\n");
	//
	//		Stack target;
	//		printf("倾倒入栈target中：\n");
	//		test.dumpTo(target);
	//		target.display();
	//		printf("栈中各元素的最大值为：%d\n", target.getMax());
	//		printf("\n");
	//
	//		printf("原栈：\n");
	//		test.display();*/
	//
	//		/*printf("弹栈：\n");
	//		for (int i = 0; i < size; i++) {
	//			printf("弹出一个元素：%d\n", test.pop());
	//			test.display();
	//			printf("栈中各元素的最大值为：%d\n", test.getMax());
	//			printf("\n");
	//		}*/
	//		printf("****************************************\n");
	//	}

	int num_oper;
	scanf("%d", &num_oper);
	Queue tunnel(num_oper);

	for (int i = 0; i < num_oper; i++)
	{
		char code[10];
		int vehi_height;
		scanf("%s", &code);  //技巧：会自动过滤空白符(回车，空格等）
		//if ('\n' == code)scanf("%c", &code);
		switch (code[0])
		{
		case 'E':
			scanf("%d", &vehi_height);
			tunnel.enqueue(vehi_height);
			break;
		case 'D':
			printf("%d\n", tunnel.dequeue());
			break;
		case 'M':
			printf("%d\n", tunnel.getMax());
			break;
		default:
			printf("Invalid Input\n");
			break;
		}
	}

	return 0;
}

