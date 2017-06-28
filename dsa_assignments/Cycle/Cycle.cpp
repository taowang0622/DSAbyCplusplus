// 循环移位Cycle:determine if either of two strings can match another that is transform by "cycle" 
//

#define NDEBUG
const int MAX_LEN = 1000000;


#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

//matchKMP()'s auxiliary function, generating next table
const int* next(const char* pattern, int m) {
	int j = 0, t = -1;
	int* next_table = new int[m];  //一定要是new出来的next表，否则return之后就消亡了
	next_table[j] = t;
	while (j < m - 1) {
		if (0 > t || pattern[j] == pattern[t]) {
			j++; t++;
			next_table[j] = (pattern[j] != pattern[t]) ? t : next_table[t];
		}
		else
			t = next_table[t];
	}
	return next_table;
}

bool matchKMP(const char* pattern, const char* text) {
	int n = strlen(text), m = strlen(pattern);
	if (n != m) return false; //退化情况
	const int* next_table = next(pattern, m);
	int i = 0, j = 0; //i,j分别为text和pattern的cursor
	while (j < m && i < (2 * n - 1)) { //针对cycle进行modify appropriately
		if (j < 0 || pattern[j] == text[(i % n)])
		{
			i++; j++;
		}
		else
			j = next_table[j];
	}
	delete[] next_table;  //next是new出来的，所以一定要delete，否则会造成内存泄露

	int rank = i - j;
	assert(0 <= rank && rank < (2 * n - 1)); //debug:rank要合法

	if (rank < (2 * n - m)) return true;
	else return false;
}


int main(int argc, char* argv[])
{
	char* str1 = NULL;
	char* str2 = NULL;
	while (scanf("%s %s", (str1 = new char[MAX_LEN]), (str2 = new char[MAX_LEN])) != EOF)
	{
		printf("%s\n", matchKMP(str1, str2) ? "YES" : "NO");
		delete[] str1; delete[] str2;
	}

	return 0;
}

