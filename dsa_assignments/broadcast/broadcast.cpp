//使用BFS框架来解决“无线电波”问题

#include <cstdio>
#include <cstdlib>
#include <cassert>
//#define NDEBUG

enum VStatus { UNDISCOVERED, DISCOVERED, VISITED };  
//VISITED表示“访问结束”，表示该节点不能被再次访问，是很重要的一个状态，不能没有

//顶点类
struct Vertex {
	VStatus status; //节点状态
	int wave_band; //当前节点的波段

	Vertex(VStatus status = UNDISCOVERED, int spectrum = -1) :status(status), wave_band(wave_band) {}
};

//边节点类
struct EdgeNode {
	int data;
	EdgeNode* pred;
	EdgeNode* succ;

	EdgeNode(int data = -1, EdgeNode* pred = NULL, EdgeNode* succ = NULL) :data(data), pred(pred), succ(succ) {}
};

//边列表类（邻接表）
class EdgeList {
private:
	int _size;
	EdgeNode(*_header), (*_trailer);
	EdgeNode* _cursor; //游标
protected:
	void init() {
		_size = 0;
		_header = new EdgeNode();
		_trailer = new EdgeNode();
		_header->succ = _trailer;
		_trailer->pred = _header;
		_cursor = _header; //将cursor指向头哨兵
	}
public:
	EdgeList() { init(); }
	~EdgeList() {
		for (EdgeNode* v = _header->succ; v != _trailer;) {
			_header->succ->succ->pred = _header;
			_header->succ = _header->succ->succ;
			delete v;
			v = _header->succ;
		}
		delete _header;
		delete _trailer;
	}

	void insertAsLast(int data) {
		EdgeNode* newNode = new EdgeNode(data, NULL, _trailer);
		newNode->pred = _trailer->pred;
		_trailer->pred->succ = newNode;
		_trailer->pred = newNode;
		_size++; //别忘记维护更新_size
	}
	int removeFirst() {
		assert(0 != _size); //调试
		_size--;
		EdgeNode* v = _header->succ;
		_header->succ->succ->pred = _header;
		_header->succ = _header->succ->succ;
		int temp = v->data;
		delete v;
		return temp;
	}

	int size() {
		return _size;
	}

	//由第一个节点开始遍历整个列表,下一个节点的position由现在_cursor的position决定
	int nextNode() {
		if ((0 == _size) || (_cursor == _trailer)) return -1;
		return ((_cursor = _cursor->succ) == _trailer) ? -1 : (*_cursor).data;
	}

	//debug
	void display() {
		if (0 == _size) {
			printf("-1\n");
			return;
		}
		for (EdgeNode* v = _header->succ; v != _trailer; v = v->succ) {
			printf("%d ", v->data);
		}
		printf("\n");
	}

};

//队列类
class Queue : public EdgeList {
public:
	Queue() {}
	~Queue() {}
	void enqueue(int data) {
		insertAsLast(data);
	}
	int dequeue() {
		assert(0 != size()); //调试
		return removeFirst();
	}
	bool empty() {
		return 0 == size();
	}

};

//使用adjacency list实现graph
struct GraphList {
	int n;// 顶点的数目
	int e;//边的数目
	Vertex* vertexes;
	EdgeList** edges; //使用邻接表

					  //n是顶点数目，m是边的数目
	GraphList(int n, int m) :n(n), e(m) {
		vertexes = new Vertex[n];
		edges = new EdgeList*[n];
		//把里的所有元素都初始化为0
		for (int i = 0; i < n; i++)
		{
			edges[i] = new EdgeList();
		}
	}
	~GraphList() {
		delete[] vertexes;
		for (int i = 0; i < n; i++)
		{
			delete edges[i];
		}
	}

	//插入从i到j的边
	void insert(int i, int j) {
		(*edges[i]).insertAsLast(j);
	}

	//获得顶点i的下一个邻居
	int nextNbr(int i) {
		return (*edges[i]).nextNode();
	}

	//debug
	void displayEdges() {
		for (int i = 0; i < n; i++)
		{
			(*edges[i]).display();
		}
	}

	//利用BFS框架解决无线电波问题（全图）
	int bfs() {
		int s = rand() % n;
		int v = s;
		do
		{
			if (UNDISCOVERED == vertexes[v].status) {
				if (!BFS(v)) return -1;
			}
		} while (s != (v = (++v % n)));
		return 1;
	}

	//单连通域
	//s表示起始基点
	bool BFS(int s) {
		Queue Q;
		vertexes[s].status = DISCOVERED; //节点s“被发现”
		vertexes[s].wave_band = 0; //将节点s的“波段”设为0
		Q.enqueue(s);
		while (!Q.empty()) {
			int v = Q.dequeue();
			for (int u = nextNbr(v); -1 < u; u = nextNbr(v))
			{
				switch (vertexes[u].status)
				{
				case UNDISCOVERED:
					vertexes[u].status = DISCOVERED;
					vertexes[u].wave_band = (vertexes[v].wave_band == 0) ? 1 : 0;
					Q.enqueue(u); break;
				case DISCOVERED:
					if (vertexes[u].wave_band == vertexes[v].wave_band) return false; break;
				case VISITED:
					break;
				}
			}
			vertexes[v].status = VISITED;
		}
		return true;
	}
};



int main()
{
	/*Queue queu;
	queu.enqueue(0);
	queu.enqueue(1);
	queu.enqueue(2);
	queu.enqueue(3);
	queu.enqueue(4);
	queu.enqueue(5);

	printf("dequeue: %d\n", queu.dequeue());
	printf("dequeue: %d\n", queu.dequeue());
	printf("dequeue: %d\n", queu.dequeue());
	printf("isEmpty: %s\n", queu.empty() ? "true" : "false");
	printf("dequeue: %d\n", queu.dequeue());
	printf("dequeue: %d\n", queu.dequeue());
	printf("dequeue: %d\n", queu.dequeue());
	printf("isEmpty: %s\n", queu.empty() ? "true" : "false");*/

	//通过自己设计的“测例”来测试，测试通过输出Passed，测试失败输出Failure
////#define NREDI
//#ifndef NREDI
//	freopen("test_cases.txt", "r", stdin);
//#endif
//	int num_test;
//	scanf("%d", &num_test);
//
//	for (int i = 0; i < num_test; i++)
//	{
//		int n, m; //两个整数n，m，分别为小镇的个数以及接下来小于20km的小镇对的数目
//		scanf("%d %d", &n, &m);
//		GraphList graph(n, m);
//		int v1, v2; //表示从v1到v2，两个小镇的距离小于20km（编号从1开始）
//		for (int i = 0; i < m; i++)
//		{
//			scanf("%d %d", &v1, &v2);
//			graph.insert(v1 - 1, v2 - 1);
//			graph.insert(v2 - 1, v1 - 1);
//		}
//
//		int answer;
//		scanf("%d", &answer);
//
//		//printf("%d\n", graph.bfs());
//		printf("%s\n", (graph.bfs() == answer) ? "Passed" : "Failure");
//	}

	int n, m; //两个整数n，m，分别为小镇的个数以及接下来小于20km的小镇对的数目
	scanf("%d %d", &n, &m);
	GraphList graph(n, m);
	int v1, v2; //表示从v1到v2，两个小镇的距离小于20km（编号从1开始）
	for (int i = 0; i < m; i++)
	{
		scanf("%d %d", &v1, &v2);
		graph.insert(v1 - 1, v2 - 1);
		graph.insert(v2 - 1, v1 - 1);
	}

	printf("%d\n", graph.bfs());

	return 0;
}

