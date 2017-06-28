// Binary Rebuild.cpp : Defines the entry point for the console application.
//
#define NDEBUG
#define NREDIR

#include <cstdio>
#include <cassert>
#include <cstdlib>

//快速io
const int BUFFER_SIZE = 1 << 20;
struct FastIO {
	char buffer_in[BUFFER_SIZE];
	char buffer_out[BUFFER_SIZE];

	FastIO(){
		setvbuf(stdin, buffer_in, _IOFBF, BUFFER_SIZE);
		setvbuf(stdout, buffer_out, _IOFBF, BUFFER_SIZE);
	}
} io;

//切分先序遍历序列
//返回右子树对应的子序列的首元素的指针，并且更新左子树对应的子序列的规模（节点数目）
int* preDivide(int* pre_seq, int pre_size, int r_child, int& left_size) {
	assert(NULL != pre_seq);
	assert(0 != pre_size);
	assert(0 != r_child);
	assert(0 == left_size);

	int i = 0;
	for (; i < pre_size; i++) {
		if (r_child == pre_seq[i])
			break;
	}
	left_size = i - 1; //左子树的节点数量
	return pre_seq + i;
}

//通过输入的先序遍历序列和后序遍历序列来实现“真二叉树重构”
//此题的目的是确定中序遍历序列，所以不必建构出一个完整的真二叉树
//只需“定位”各个节点在中序遍历序列中的位置即可
void properRebuild(int* pre_seq, int* post_seq, int* in_seq, int size) {
	//因为三个序列是“同步切分”切分的，所以当先序遍历序列的规模为1时
	//其他两个序列的规模也为1
	if (1 == size) {
		in_seq[0] = pre_seq[0]; return;
	}//递归基，“安全绳”

	int root = pre_seq[0]; //根节点的编号
	int left_size = 0; //根节点root的左子树的节点数量
	int right_size = 0; //根节点root的右子树的节点数量
	int* left_pre_seq = pre_seq + 1;
	int* left_post_seq = post_seq;
	int* left_in_seq = in_seq;

	int* right_pre_seq = preDivide(pre_seq, size, post_seq[size - 2], left_size);
	right_size = size - left_size - 1;
#ifndef NDEBUG
	printf("先序遍历序列的左子序列的规模为：%d, 首元素为：%d\n", left_size, *left_pre_seq);
	printf("先序遍历序列的右子序列的规模为：%d, 首元素为：%d\n", right_size, *right_pre_seq);
#endif

	int* right_post_seq = post_seq + left_size;
#ifndef NDEBUG
	printf("后序遍历序列的右子序列的首元素为：%d\n", *right_post_seq);
#endif

	int* right_in_seq = in_seq + left_size + 1;
#ifndef NDEBUG
	printf("中序遍历序列的右子序列的首元素是否正确：%d\n", *right_in_seq == in_seq[left_size + 1]);
	printf("中序遍历序列的定位是否正确：%d\n\n", in_seq[left_size] == pre_seq[0]);
#endif
	in_seq[left_size] = pre_seq[0];//对中序遍历序列“定位赋值”

	properRebuild(left_pre_seq, left_post_seq, left_in_seq, left_size);
	properRebuild(right_pre_seq, right_post_seq, right_in_seq, right_size);
}

int main()
{
	//测试
#ifndef NREDIR
	freopen("test1.txt", "r", stdin);
#endif
	/*int case_size = 9;
	int case_num = 2;

	for (int i = 0; i < case_num; i++)
	{
		int preorder[9];
		int postorder[9];
		int true_inorder[9];
		for (int i = 0; i < case_size; i++) scanf("%d", &preorder[i]);
		for (int i = 0; i < case_size; i++) scanf("%d", &postorder[i]);
		for (int i = 0; i < case_size; i++) scanf("%d", &true_inorder[i]);
		int inorder[9];
		properRebuild(preorder, postorder, inorder, case_size);
		for (int i = 0; i < case_size; i++)
		{
			if (true_inorder[i] != inorder[i]) {
				printf("Error\n");
				break;
			}
		}
		printf("Correct\n");
	}*/

	int num_node;
	scanf("%d", &num_node);
	int* preorder = new int[num_node];
	int* postorder = new int[num_node];
	int* inorder = new int[num_node];
	
	for (int i = 0; i < num_node; i++) scanf("%d", &preorder[i]);
	for (int i = 0; i < num_node; i++) scanf("%d", &postorder[i]);

	properRebuild(preorder, postorder, inorder, num_node);

	for (int i = 0; i < num_node; i++)
	{
		printf("%d ", inorder[i]);
	}
	printf("\n");

	delete[] preorder;
	delete[] postorder;
	delete[] inorder;
	return 0;
}