// TSP.cpp : 定义控制台应用程序的入口点。
//旅行商问题

#include <cstdio>
#include <cstdlib>

enum VStatus { UNDISCOVERED, DISCOVERED };

//顶点类
struct Vertex {
	VStatus status; //节点状态
	int longestPath; //以当前节点为starting point的最长路径上的节点数

	Vertex(VStatus status = UNDISCOVERED, int longestPath = 0) :status(status), longestPath(longestPath) {}
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

	//通过拓扑排序得到旅行商问题的解（TSP）
	int tSort() {
		int startPnt = rand() % n;
		int v = startPnt; //v相当于游标的作用
		int max = 0; //用于记录TSP的解
		do {
			if (UNDISCOVERED == vertexes[v].status) {
				int temp = DFS(v);
				max = (temp > max) ? temp : max;
			}
		} while (startPnt != (v = (++v % n)));
		return max;
	}
	//单趟(单个连通域)
	int DFS(int v) {
		for (int u = nextNbr(v); -1 < u; u = nextNbr(v)) {
			switch (vertexes[u].status)
			{
			case UNDISCOVERED:
				vertexes[v].longestPath = ((DFS(u) + 1) > vertexes[v].longestPath) ? \
					(DFS(u) + 1) : vertexes[v].longestPath;
				break;
			case DISCOVERED:
				return (vertexes[u].longestPath + 1) > vertexes[v].longestPath ? \
					(vertexes[v].longestPath = vertexes[u].longestPath + 1) : vertexes[v].longestPath;
				break;
			}

		}
		return (0 == vertexes[v].longestPath) ? (vertexes[v].longestPath = 1) : vertexes[v].longestPath;
	}
};



int main()
{
	int n, m; //n为村庄数目，m为道路的数目
	scanf("%d %d", &n, &m);
	GraphList graph(n, m);
	int v1, v2; //表示从v1到v2的路
	for (int i = 0; i < m; i++)
	{
		scanf("%d %d", &v1, &v2);
		graph.insert(v1 - 1, v2 - 1);
	}
	printf("%d\n", graph.tSort());


	return 0;
}

