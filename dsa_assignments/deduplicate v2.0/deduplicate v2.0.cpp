// deduplicate.cpp : 定义控制台应用程序的入口点。
//

#include <cstdio>
#include <cstring>
#include <cstdlib>
//
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

const unsigned SIZE_HASHTABLE = 1200007;
#define hash(hash_code) ((hash_code) % SIZE_HASHTABLE) //散列函数：除余法（division method）
//#define NDEBUG

//词条类
struct Entry {
	int hash_code;
	char* str;
	bool duplicate; //是否重复标志位flag

	Entry(int hash_code, char* str, bool flag = false) :hash_code(hash_code), str(str), duplicate(flag) {}
};


//散列表类
class Hashtable {
private:
	Entry** ht;
	//Entry* ht[SIZE_HASHTABLE];

	//冲突排解策略：线性试探（linear probing）
	//hit：返回命中的秩 miss：返回-1
	int probe4Hit(int hash_code) {
		int rank = hash(hash_code);
		while (ht[rank] && hash_code != ht[rank]->hash_code) { //注意：此逻辑表达式起作用的前提是要对每个桶单元初始化
			rank = (rank + 1) % SIZE_HASHTABLE;
		}
		return ht[rank] ? rank : -1;
	}

	int probe4Free(int hash_code) {
		int rank = hash(hash_code);
		while (ht[rank]) rank = (rank + 1) % SIZE_HASHTABLE;
		return rank; //为保证总能找到空桶，装填因子和hash table长度需要合理设置
	}

public:
	Hashtable() {
		ht = new Entry*[SIZE_HASHTABLE];
		memset(ht, 0, sizeof(Entry*) * SIZE_HASHTABLE); //初始化每个“桶单元”
	}
	//~Hashtable(){}

	//生成字符串的循环移位散列码cyclic shift hash code
	//参数为C风格的字符串（字符数组）
	int hashCode(char* str) {
		int h = 0;
		for (int length = strlen(str), i = 0; i < length; i++)
		{
			h = (h << 5) | (h >> 27);
			h += (int)str[i];
		}
		return h;
	}

	//插入关键码类型为char*(C风格字符串)的词条
	bool put(char *str) {
		int hash_code = hashCode(str);
		int rank;
		if ((rank = probe4Hit(hash_code)) != -1) {
			if (strcmp(ht[rank]->str, str) == 0) {
				if (false == (ht[rank]->duplicate)) {
					ht[rank]->duplicate = true; printf("%s\n", str); return false;
					//第一次出现重复，就把对应的桶单元“重复标志位”置为true，并立即返回false
				}
				return true; //第一次重复之后的重复就直接返回true
			}
		}

		rank = probe4Free(hash_code);
		//Entry new_entry = { hash_code, str };
		ht[rank] = new Entry(hash_code, str);
		return true;
	}

};



int main()
{
	//#define NREDIR
#ifndef NREDIR
	freopen("test_cases_1.txt", "r", stdin);
#endif

	Hashtable hash_table;
	int num_cases;
	scanf("%d\n", &num_cases);
	//char** outputs = new char*[num_cases]; //用来存储duplicate的字符串的首字符的“指针”
	//int rank_outputs = 0; //数组ouputs的秩，扮演cursor的作用
	for (int i = 0; i < num_cases; i++)
	{
		char* str = new char[45];
		scanf("%s", str);
		hash_table.put(str);
		//puts(temp); //puts()输出字符串后会自动换行
		//printf("    whether succeed in putting: %s\n", hash_table.put(temp) ? "true" : "false");
		/*if (!hash_table.put(str)) {
			outputs[rank_outputs] = str;
			rank_outputs++;*/
		//}
	}

	/*for (int i = 0; i < rank_outputs; i++)
	{
		printf("%s\n", outputs[i]);
	}*/

	return 0;
}

