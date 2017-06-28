// deduplicate.cpp : 定义控制台应用程序的入口点。
//

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>
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

const unsigned SIZE_HASHTABLE = 1200109;
//#define hash(hash_code) ((hash_code) % SIZE_HASHTABLE)
//散列函数：除余法（division method），有连续性缺陷
#define hash(hash_code) ((31 * (hash_code) + 2) % SIZE_HASHTABLE) 
//散列函数：MAD法，a和b取值恰当的话可以克服除余法的连续性缺陷
//#define NDEBUG

//词条类
struct Entry {
	int hash_code;
	char* str;
	bool duplicate; //是否重复标志位flag

	//重复标识位duplicate默认为false
	Entry(int hash_code, char* str, bool flag = false) :hash_code(hash_code), str(str), duplicate(flag) {}
};


//散列表类
class Hashtable {
private:
	Entry** ht;
	//Entry* ht[SIZE_HASHTABLE];  //除了new之外，在类体和函数体内定义的对象，都是临时的，存储在栈中，对规模有限制

	//生成字符串的循环移位散列码cyclic shift hash code
	//参数为C风格的字符串（字符数组）
	int hashCode(const char* str) {
		int h = 0;
		for (int length = strlen(str), i = 0; i < length; i++)
		{
			h = (h << 5) | (h >> 27);
			h += (int)str[i];
		}
		return h;
	}

	//多项式散列码polynomial hash code
	/*int hashCode(const char* str) {
		int temp = 33;
		int code = 0;
		for (int length = strlen(str), i = 0; i < length; i++)
		{
			code += int(str[i]) * pow(temp, length - i - 1);
		}
		return code;
	}*/

	//冲突排解策略：平方试探（quadratic probing）
	//参数为关键码（字符串）
	int probe4Hit(const char* str) {
		int hash_code = hashCode(str);
		int rank = hash(hash_code);
		int i = 0;
		//注意：此逻辑表达式起作用的前提是要对每个桶单元初始化,C++的逻辑表达式是好东西啊！！！！！！！！！！
		while (ht[rank] && (hash_code != ht[rank]->hash_code || (strcmp(str, ht[rank]->str) != 0))) {
			rank = (rank + i * i) % SIZE_HASHTABLE;
			i++;
		}
		return ht[rank] ? rank : -1;
	}

	//冲突排解策略：平方试探（quadratic probing）
	int probe4Free(const char* str) {
		int rank = hash(hashCode(str));
		int i = 0;
		while (ht[rank]) {
			rank = (rank + i * i) % SIZE_HASHTABLE; i++;
		}
		return rank; //为保证总能找到空桶，装填因子和hash table长度需要合理设置
	}

public:
	Hashtable() {
		ht = new Entry*[SIZE_HASHTABLE];
		memset(ht, 0, sizeof(Entry*) * SIZE_HASHTABLE); //初始化每个“桶单元”
	}
	//析构函数
	~Hashtable() {
		for (int i = 0; i < SIZE_HASHTABLE; i++)
		{
			if (!ht[i]) continue;
			delete[] ht[i]->str;
			delete ht[i];
		}
		delete[] ht;
	}

	//插入关键码类型为char*(C风格字符串)的词条
	bool put(char *str) {
		int hash_code = hashCode(str);
		int rank;
		if ((rank = probe4Hit(str)) != -1) {
			if (false == (ht[rank]->duplicate)) {
				ht[rank]->duplicate = true; return false;
				//第一次出现重复，就把对应的桶单元“重复标志位”置为true，并立即返回false
			}
			return true; //第一次重复之后的重复就直接返回true
		}

		rank = probe4Free(str);
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
	char** outputs = new char*[num_cases]; //用来存储duplicate的字符串的首字符的“指针”
	int rank_outputs = 0; //数组ouputs的秩，扮演cursor的作用
	for (int i = 0; i < num_cases; i++)
	{
		char* str = new char[45];
		scanf("%s", str);
		//puts(temp); //puts()输出字符串后会自动换行
		//printf("    whether succeed in putting: %s\n", hash_table.put(temp) ? "true" : "false");
		if (!hash_table.put(str)) {
			outputs[rank_outputs] = str;
			rank_outputs++;
		}
	}

	for (int i = 0; i < rank_outputs; i++)
	{
		printf("%s\n", outputs[i]);
	}

	return 0;
}

