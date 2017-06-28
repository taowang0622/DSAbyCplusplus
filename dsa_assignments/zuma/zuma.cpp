// zuma.cpp : 定义控制台应用程序的入口点。
//

#include <cstdio>
#include <cassert>

//快速io , 用结构体struct简洁
const int BUFFER_SIZE = 1 << 20;  //把buffer设置为2^20个byte
struct FastIO {
	char inbuf[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];
	
	FastIO() {  //构造函数
		setvbuf(stdin, inbuf, _IOFBF, BUFFER_SIZE);
		setvbuf(stdout, outbuf, _IOFBF, BUFFER_SIZE);
	}
}io;

//typedef (ListNode*) ListNodePosi;
#define ListNodePosi ListNode*

//实现列表节点类
struct ListNode {
	char data;
	ListNodePosi pred;
	ListNodePosi succ;
};

//实现列表类
class List {
private:
	int _size;
	ListNodePosi header;//头哨兵节点的位置
	ListNodePosi trailer;//尾哨兵节点的位置
public:
	//构造函数
	List() {
		header = new ListNode;
		trailer = new ListNode;
		header->succ = trailer;
		header->pred = NULL;
		trailer->pred = header;
		trailer->succ = NULL;

		_size = 0;//规模_size是指对外可见的节点的规模，不包括header和trailer
	}

	//析构函数
	~List() {
		delete header, trailer;
	}

	//把数据项为data的新节点插到位置为posi的节点之前,并返回新节点的位置
	//为应对“边界情况”，当只输入一个参数的时候，默认posi为trailer，也就是说把新节点插在列表末尾
	ListNodePosi insertB(char data, ListNodePosi posi = NULL) {
		if (NULL == posi)posi = trailer;

		ListNodePosi new_node_posi = new ListNode;
		new_node_posi->data = data;
		new_node_posi->succ = posi;
		new_node_posi->pred = posi->pred;

		posi->pred->succ = new_node_posi;
		posi->pred = new_node_posi;

		_size++;

		return new_node_posi;
	}

	//remove位置为posi的节点
	char remove(ListNodePosi posi) {
		char temp = posi->data;
		posi->pred->succ = posi->succ;
		posi->succ->pred = posi->pred;

		_size--;
		delete posi;
		return temp;
	}

	//由栈到位置的转换
	ListNodePosi rankToPosi(int rank) {
		//退化情况的处理
		//assert((0 <= rank)&&(_size >= rank));
		if (rank < 0)
			rank = 0;
		else if (rank > _size)
			rank = _size;

		ListNodePosi posi = header->succ; //first node的位置
		while (0 < rank--) {
			posi = posi->succ;
		}
		return posi;
	}

	//zuma输入的bead，并把zuma后的整个beads显示出来
	void zuma(ListNodePosi new_posi, char bead) {
		//assert('A' <= bead <= 'Z');

		ListNodePosi posi = new_posi;
		int counter = 0;//计数器
		//判断包括插入的bead在内的左右连续相同的珠子有几颗
		if ((posi != trailer) && (posi->data != bead)) {
			while (((posi = posi->pred) != header) && (posi->data == bead))
				counter++;
		}
		else {
			if (posi != trailer) {
				counter++;
				ListNodePosi backward_posi = posi;
				while ((backward_posi = backward_posi->succ)->data == bead)
					counter++;
			}
			while ((posi = posi->pred)->data == bead)
				counter++;
		}
		//printf("包括新bead在内的前后相同珠子的总数为：%d\n", counter + 1); //测试

		//判断相同bead数是否已达到消除的程度,没达到就插入，达到就remove掉，然后递归调用消除后续可消除的beads
		if (2 <= counter) {
			for (int i = 0; i < counter; i++)
			{
				remove(posi->succ);
			}
			if ((posi != header) && (posi->succ != trailer)) {
				ListNodePosi succ = posi->succ;
				zuma(succ, remove(posi));
			}
		}
		else
			insertB(bead, new_posi);

	}

	//显示列表中每一个元素的data值
	void display() {
		ListNodePosi posi = header;
		if (posi->succ == trailer)
			printf("-\n");
		else {
			while ((posi = posi->succ) != trailer) {
				printf("%c", posi->data);
			}
			printf("\n");
		}

	}
};



int main()
{
	//setvbuf(stdout, NULL, _IOLBF, 2048);

	List beads;
	char bead;
	while ((scanf_s("%c", &bead), '\n' != bead))
	{
		beads.insertB(bead);
	}

	int num_insert;
	scanf_s("%d", &num_insert);

	int rank;
	char new_bead;
	for (int i = 0; i < num_insert; i++)
	{
		scanf_s("%d %c", &rank, &new_bead);
		//zuma输入的bead，并把zuma后的整个beads显示出来
		ListNodePosi posi = beads.rankToPosi(rank);
		beads.zuma(posi, new_bead);
		beads.display();
	}
	return 0;
}

