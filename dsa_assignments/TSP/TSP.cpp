// TSP.cpp : 定义控制台应用程序的入口点。
//旅行商问题

#include <cstdio>
#include <cstdlib>

enum VStatus { UNDISCOVERED, DISCOVERED };
const int MAX = 1000000;

//顶点类
struct Vertex {
	VStatus status; //节点状态
	int longestPath; //以当前节点为starting point的最长路径上的节点数

	Vertex(VStatus status = UNDISCOVERED, int longestPath = 0):status(status), longestPath(longestPath){}
};

//边类
struct Edge {
	int data;
	int* next;
};

//使用adjacency matrix实现graph
struct GraphMatrix {
	int n;// 顶点的数目
	int e;//边的数目
	Vertex* vertexes;
	int** edges;

	//n是顶点数目，m是边的数目
	GraphMatrix(int n, int m) :n(n), e(m) {
		vertexes = new Vertex[n];
		edges = new int*[n];
		//把矩阵里的所有元素都初始化为0
		for (int i = 0; i < n; i++)
		{
			edges[i] = new int[n];
			for (int j = 0; j < n; j++)
			{
				edges[i][j] = 0;
			}
		}
	}
	~GraphMatrix() {
		delete vertexes;
		for (int i = 0; i < n; i++)
		{
			delete[] edges[i];
		}
	}

	//插入从i到j的边
	void insert(int i, int j) {
		edges[i][j] = 1;
	}

	//查找顶点i的相对于顶点j的下一个邻居（逆向查找）
	int nextNbr(int i, int j) {
		while ((-1 < j) && (1 != edges[i][--j]));
		return j;
	}
	int firstNbr(int i) {
		return nextNbr(i, n);
	}

	//debug
	void displayEdges() {
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				printf("%d ", edges[i][j]);
			}
			printf("\n");
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
	//单趟
	int DFS(int v) {
		for (int u = firstNbr(v); -1 < u; u = nextNbr(v, u)) {
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
		GraphMatrix graph(4, 3);
		graph.insert(1 - 1, 4 - 1);
		graph.insert(2 - 1, 4 - 1);
		graph.insert(4 - 1, 3 - 1);
		graph.displayEdges();

	/*	GraphMatrix graph(12, 10);
		graph.insert(2 - 1, 1 - 1);
		graph.insert(3 - 1, 4 - 1);
		graph.insert(5 - 1, 4 - 1);
		graph.insert(6 - 1, 8 - 1);
		graph.insert(7 - 1, 8 - 1);
		graph.insert(8 - 1, 9 - 1);
		graph.insert(8 - 1, 10 - 1);
		graph.insert(5 - 1, 11 - 1);
		graph.insert(5 - 1, 8 - 1);
		graph.insert(9 - 1, 12 - 1);
		graph.displayEdges();*/

		int temp;
		printf("first: %d\n", temp = graph.firstNbr(1 - 1));
		printf("%d\n", temp = graph.nextNbr(1 - 1, temp));
		printf("%d\n", temp = graph.nextNbr(1 - 1, temp));
		printf("%d\n", temp = graph.nextNbr(1 - 1, temp));
		printf("%d\n", temp = graph.nextNbr(1, temp));

		printf("first: %d\n", temp = graph.firstNbr(2 - 1));

		printf("output: %d\n", graph.tSort());

		//int n, m; //n为村庄数目，m为道路的数目
		//scanf("%d %d", &n, &m);
		//GraphMatrix graph(n, m);
		//int v1, v2; //表示从v1到v2的路
		//for (int i = 0; i < m; i++)
		//{
		//	scanf("%d %d", &v1, &v2);
		//	graph.insert(v1 - 1, v2 - 1);
		//}
		//printf("%d\n", graph.tSort());


		return 0;
	}

