#define _CRT_SECURE_NO_WARNINGS
#include "io.h"
#include "string.h"
#include <tchar.h>
#include "stdlib.h"
#include <windows.h>
#include "hash.h"
#include <fstream>
#include <cmath>
using namespace std;

#define BUF_MAX_LEN 100  //分词的词语最大长度
#define FILENAME_MAX_LEN    100 //文件名的最大长度
void UNION(key_list m, key_list n, doc_list &hc); //布尔检索的求并
void InterSect(key_list m, key_list n, doc_list &hc);//布尔检索的求交
void Gui();//简单界面
void Pagerank(Files *files, Files1 *page);//计算每个文档PR值以及出入度 记录到page结构数组中
void Insort(doc_list &hc, Files1 *page);//入度值排序
void Pagesort(doc_list hc, Files1 *page);//PR值排序
void getFiles(string path, Files *files);//获取链接文档名
doc_list SaveItems(int i, char filename[][FILENAME_MAX_LEN], char*address);  //保存每一个关键词后的文档信息
FILE* OpenReadFile(int index, char filename[][FILENAME_MAX_LEN]); //打开可读文件
FILE* OpenWriteFile(const char *in_file_path); //打开可写文件
int GetFileName(char filename[][FILENAME_MAX_LEN]); //获得目录下的文本个数
bool trans_real_exp(char exp[], char real_exp[], doc_list exp_array[], int &count_key); //将用户输入的表达式转化为用字母代替关键词方便运算
void translate(char str[], char exp[]);//将中缀表达式转化为后缀表达式
void cal_value(char exp[], doc_list exp_array[], int &count_key); //通过栈计算后缀表达式

int  main()
{
	Gui();
	key_list keylist;
	char *pbuf;
	char filename[160][FILENAME_MAX_LEN];
	char**words = new char*[1000000];
	int u;
	for (u = 0; u < 10000; u++)
	{
		words[u] = new char[50];
	}
	int filenum = GetFileName(filename);  //返回读取文档的数目
	FILE *fr;
	pbuf = (char *)malloc(BUF_MAX_LEN);
	memset(pbuf, 0, BUF_MAX_LEN);

	preparecrypttable();    //初始化Hash表
	int wordnum = 0;
	for (int i = 0; i < filenum; i++)  //开始读取每一个文档的分词。
	{
		fr = OpenReadFile(i, filename);
		if (fr == NULL)
		{
			break;
		}
		bool is_web = false;
		wchar_t*wc = (wchar_t*)malloc(200 * sizeof(wchar_t));
		char*address = NULL;
		while (!feof(fr))
		{
			fgetws(wc, 200, fr);
			int m = wcslen(wc);
			if (wc[m - 1] == '\n')
				wc[m - 1] = 0;
			char*pbuf = (char*)malloc(sizeof(char)*(2 * wcslen(wc) + 1));
			memset(pbuf, 0, 2 * wcslen(wc) + 1);
			setlocale(LC_ALL, "");
			wcstombs(pbuf, wc, 2 * wcslen(wc) + 1);
			if (is_web == true)
			{
				if (keylist = SearchByString(pbuf))    //到hash表内查询
				{
					bool flag = false;
					doc_list infolist = SaveItems(i, filename, address);
					doc_list com = keylist->next;
					while (com != NULL)
					{
						if (com->id == i)
						{
							flag = true;
							break;
						}
						else
							com = com->next;
					}

					if (flag == false)
					{
						infolist->next = keylist->next;
						keylist->count++;
						keylist->next = infolist;
					}
					else
					{
						keylist->count++;
					}
				}
				else
				{
					// 如果关键字第一次出现，则将其加入hash表
					int pos = InsertString(pbuf);       //插入hash表
					keylist = key_array[pos];
					doc_list infolist = SaveItems(i, filename, address);
					infolist->next = NULL;
					keylist->next = infolist;
					if (pos != -1)
					{
						strcpy_s(words[wordnum], 50, pbuf);

						wordnum++;
					}
				}
			}
			else
			{
				int addr_len = strlen(pbuf);
				address = (char *)malloc(addr_len + 1);
				memset(address, 0, addr_len + 1);
				strncpy(address, pbuf, addr_len);
				is_web = true;
			}

		}
		fclose(fr);
	}
	FILE *fw = fopen("D:\\new\\Desktop\\index.txt", "wt+");
	if (fw == NULL)
	{
		return 0;
	};
	// 遍历关键字数组，将关键字及其对应的文档内容写入文件中
	for (int i = 0; i < wordnum; i++)
	{

		keylist = SearchByString(words[i]);
		if (keylist != NULL)
		{
			fprintf(fw, "%s %d  ", words[i], keylist->count);
			doc_list infolist = keylist->next;
			while (infolist != NULL)
			{

				fprintf(fw, " %d %s ", infolist->id, infolist->name);
				infolist = infolist->next;
			}
			fputc('\n', fw);
		}

	}
	free(pbuf);
	fclose(fw);
	Files files[size];
	Files1 page[size];
	Pagerank(files, page);
	char judge;//判断是否需要继续查询
	do
	{
		//以下是bool检索
		cout << "请输入需要查找的关键字个数" << endl;
		int num;
		cin >> num;
		cout << "请输入关键字" << endl;
		char exp[100];
		cin >> exp;
		if (num == 1)
		{
			char*real_exp = new char[strlen(exp) + 1];
			memset(real_exp, 0, strlen(exp) + 1);
			strncpy(real_exp, exp, strlen(exp));
			key_list t1 = SearchByString(real_exp);
			if (t1 == NULL)
			{
				cout << "搜索失败，无您需要搜索的内容" << endl;
			}
			else
			{
				doc_list hc = t1->next;
				DOCNODE *pa = new DOCNODE;
				pa->next = hc;
				hc = pa;
				cout << "请选择查看哪种排序方式得到的结果：" << endl;
				cout << "   1.Pagerank排序     " << endl;
				cout << "   2.入度值排序          " << endl;
				int ch;  //选择排序方式
				cin >> ch;
				if (ch == 1)
					Pagesort(hc, page);
				else
					Insort(hc, page);
				//int h1 = 0;
				hc = hc->next;
				while (hc != NULL)
				{
					printf("%s  ", hc->name);
					cout << "  入度值：" << hc->in << "  PR值：" << hc->PR << endl;
					hc = hc->next;
				}
			}
		}
		else
		{
			//需要计算表达式
			char real_exp[100];
			doc_list exp_array[20];
			int count_key = 0;
			if (trans_real_exp(exp, real_exp, exp_array, count_key) == false)
			{
				cout << "搜索失败，无您需要搜索的内容" << endl;
			}
			else
			{
				char exp1[100];
				translate(real_exp, exp1);
				cal_value(exp1, exp_array, count_key);
				doc_list result = exp_array[count_key - 1];

				cout << "请选择查看哪种排序方式得到的结果：" << endl;
				cout << "   1.Pagerank排序     " << endl;
				cout << "   2.入度值排序          " << endl;
				int ch;  //选择排序方式
				cin >> ch;
				if (ch == 1)
					Pagesort(result, page);
				else
					Insort(result, page);
				//int h1 = 0;
				result = result->next;
				while (result != NULL)
				{
					printf("%s  ", result->name);
					cout << "  入度值：" << result->in << "  PR值：" << result->PR << endl;
					cout << "该文档地址:" << result->address << endl;
					result = result->next;
				}
			}
		}
		cout << "是否需要继续查询？（输入 Y or N）" << endl;
		cin >> judge;
	} while (judge != 'N');
	delete[]words;
	return 0;
}

void UNION(doc_list m, doc_list n, doc_list &hc)
{

	doc_list pa = m->next;
	doc_list pb = n->next;
	doc_list s, tc;
	hc = (doc_list)malloc(sizeof(DOCNODE));
	tc = hc;
	while (pa != NULL&&pb != NULL)
	{
		if (pa->id > pb->id)
		{
			s = (doc_list)malloc(sizeof(DOCNODE));
			s->id = pa->id;
			int len = strlen(pa->name);
			s->name = (char *)malloc(len + 1);
			memset(s->name, 0, len + 1);
			strncpy(s->name, pa->name, len);
			int addr_len = strlen(pa->address);
			s->address = (char *)malloc(addr_len + 1);
			memset(s->address, 0, addr_len + 1);
			strncpy(s->address, pa->address, addr_len);
			tc->next = s;
			tc = s;
			pa = pa->next;
		}
		else if (pa->id < pb->id)
		{
			s = (doc_list)malloc(sizeof(DOCNODE));
			s->id = pb->id;
			int len = strlen(pb->name);
			s->name = (char *)malloc(len + 1);
			memset(s->name, 0, len + 1);
			strncpy(s->name, pb->name, len);
			int addr_len = strlen(pb->address);
			s->address = (char *)malloc(addr_len + 1);
			memset(s->address, 0, addr_len + 1);
			strncpy(s->address, pb->address, addr_len);
			tc->next = s;
			tc = s;
			pb = pb->next;
		}
		else
		{
			s = (doc_list)malloc(sizeof(DOCNODE));
			s->id = pa->id;
			int len = strlen(pa->name);
			s->name = (char *)malloc(len + 1);
			memset(s->name, 0, len + 1);
			strncpy(s->name, pa->name, len);
			int addr_len = strlen(pa->address);
			s->address = (char *)malloc(addr_len + 1);
			memset(s->address, 0, addr_len + 1);
			strncpy(s->address, pa->address, addr_len);
			tc->next = s;
			tc = s;
			pa = pa->next;
			pb = pb->next;
		}
	}
	if (pb != NULL)
	{
		pa = pb;
	}
	while (pa != NULL)
	{
		s = (doc_list)malloc(sizeof(DOCNODE));
		s->id = pa->id;
		int len = strlen(pa->name);
		s->name = (char *)malloc(len + 1);
		memset(s->name, 0, len + 1);
		strncpy(s->name, pa->name, len);
		int addr_len = strlen(pa->address);
		s->address = (char *)malloc(addr_len + 1);
		memset(s->address, 0, addr_len + 1);
		strncpy(s->address, pa->address, addr_len);
		tc->next = s;
		tc = s;
		pa = pa->next;
	}
	tc->next = NULL;
}
void InterSect(doc_list m, doc_list n, doc_list &hc)
{
	doc_list pa = m->next;
	doc_list pb;
	doc_list s, tc;
	hc = (doc_list)malloc(sizeof(DOCNODE));
	tc = hc;
	while (pa != NULL)
	{
		pb = n->next;
		while (pb != NULL&&pb->id > pa->id)
		{
			pb = pb->next;
		}
		if (pb != NULL&& pb->id == pa->id)
		{

			s = (doc_list)malloc(sizeof(DOCNODE));
			s->id = pa->id;
			int len = strlen(pa->name);
			s->name = (char *)malloc(len + 1);
			memset(s->name, 0, len + 1);
			strncpy(s->name, pa->name, len);
			int addr_len = strlen(pa->address);
			s->address = (char *)malloc(addr_len + 1);
			memset(s->address, 0, addr_len + 1);
			strncpy(s->address, pa->address, addr_len);
			tc->next = s;
			tc = s;
		}
		pa = pa->next;
	}
	tc->next = NULL;
}

void Gui()
{
	cout << " --------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << "                                                         搜索引擎Demo                                                                         " << endl;
	cout << " --------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << "                                                 制作人：蒋雨霖 马泽坤 支原                                                               " << endl;
}

doc_list SaveItems(int i, char filename[][FILENAME_MAX_LEN], char*address)
{
	doc_list infolist = (doc_list)malloc(sizeof(DOCNODE));
	infolist->id = i;
	int len = strlen(filename[i]);
	infolist->name = (char *)malloc(len + 1);
	memset(infolist->name, 0, len + 1);
	strncpy(infolist->name, filename[i], len);
	int addr_len = strlen(address);
	infolist->address = (char *)malloc(addr_len + 1);
	memset(infolist->address, 0, addr_len + 1);
	strncpy(infolist->address, address, addr_len);
	return infolist;
}

FILE* OpenReadFile(int index, char filename[][FILENAME_MAX_LEN])
{
	locale loc("");
	wcout.imbue(loc);
	FILE * fp;
	char t[100] = "D:\\new\\Desktop\\search\\";
	strcat_s(t, 100, filename[index]);
	int len1 = strlen(t);
	char*CStr = (char*)malloc(len1 + 1);
	strcpy(CStr, t);
	fp = fopen(CStr, "rt,ccs=UTF-8");
	return fp;


}

FILE* OpenWriteFile(const char *in_file_path)
{
	if (in_file_path == NULL)
	{
		printf("output file path error!\n");
		return NULL;
	}

	FILE *fp = fopen(in_file_path, "w+");
	if (fp == NULL)
	{
		printf("open write file error!\n");
	}
	return fp;
}
int GetFileName(char filename[][FILENAME_MAX_LEN])
{
	_finddata_t file;
	long handle;
	int filenum = 0;
	if ((handle = _findfirst("D:\\new\\Desktop\\search\\*.txt", &file)) == -1)
	{
		printf("Not Found\n");
	}
	else
	{
		do
		{
			strcpy_s(filename[filenum++], file.name);
		} while (!_findnext(handle, &file));

	}
	_findclose(handle);
	return filenum;
}

void translate(char str[], char exp[])
{
	struct
	{
		char data[50];
		int top;
	}op;
	char ch;
	int i = 0, t = 0;
	op.top = -1;
	ch = str[i];
	i++;
	while (ch != '\0')
	{
		switch (ch)
		{
		case '(':
			op.top++;
			op.data[op.top] = ch;
			break;
		case')':
			while (op.data[op.top] != '(')
			{
				exp[t] = op.data[op.top];
				op.top--;
				t++;
			}
			op.top--;
			break;
		case '|':
		case '&':
			while (op.top != -1 && op.data[op.top] != '(')
			{
				exp[t] = op.data[op.top];
				op.top--;
				t++;
			}
			op.top++;
			op.data[op.top] = ch;
			break;
		default:
			while (ch >= 'A'&&ch <= 'Z')
			{
				exp[t] = ch;
				t++;
				ch = str[i];
				i++;
			}
			i--;
		}
		ch = str[i];
		i++;
	}
	while (op.top != -1)
	{
		exp[t] = op.data[op.top];
		t++;
		op.top--;
	}
	exp[t] = '\0';
}
void cal_value(char exp[], doc_list exp_array[], int &count_key)
{
	struct
	{
		char data[50];
		int top;
	}st;
	char ch;
	int t = 0;
	st.top = -1;
	ch = exp[t];
	t++;
	while (ch != '\0')
	{
		switch (ch)
		{
		case '|':
			doc_list t2;
			int num1, num2;
			for (int m = 0; m < count_key; m++)
			{
				if (exp_array[m]->id + 'A' == st.data[st.top - 1])
					num1 = m;
				if (exp_array[m]->id + 'A' == st.data[st.top])
					num2 = m;
			}
			UNION(exp_array[num1], exp_array[num2], t2);
			exp_array[count_key] = (DOCNODE *)malloc(sizeof(DOCNODE)* 1);
			exp_array[count_key]->id = count_key;
			doc_list tail;
			tail = exp_array[count_key];
			t2 = t2->next;
			while (t2 != NULL)
			{
				doc_list temp = (DOCNODE *)malloc(sizeof(DOCNODE)* 1);
				temp->id = t2->id;
				int key_len = strlen(t2->name);
				temp->name = (char *)malloc(key_len + 1);
				memset(temp->name, 0, key_len + 1);
				strncpy(temp->name, t2->name, key_len);
				int key_addr_len = strlen(t2->address);
				temp->address = (char *)malloc(key_addr_len + 1);
				memset(temp->address, 0, key_addr_len + 1);
				strncpy(temp->address, t2->address, key_addr_len);
				tail->next = temp;
				tail = temp;
				t2 = t2->next;
			}
			tail->next = NULL;

			st.data[st.top - 1] = exp_array[count_key]->id + 'A';
			count_key++;
			st.top--;
			break;
		case '&':
			doc_list t1;
			int n1, n2;
			for (int m = 0; m < count_key; m++)
			{
				if (exp_array[m]->id + 'A' == st.data[st.top - 1])
					n1 = m;
				if (exp_array[m]->id + 'A' == st.data[st.top])
					n2 = m;
			}
			InterSect(exp_array[n1], exp_array[n2], t1);
			exp_array[count_key] = (DOCNODE *)malloc(sizeof(DOCNODE)* 1);
			exp_array[count_key]->id = count_key;
			doc_list tail1;
			tail1 = exp_array[count_key];
			t1 = t1->next;
			while (t1 != NULL)
			{
				doc_list temp1 = (DOCNODE *)malloc(sizeof(DOCNODE)* 1);
				temp1->id = t1->id;
				int key_len = strlen(t1->name);
				temp1->name = (char *)malloc(key_len + 1);
				memset(temp1->name, 0, key_len + 1);
				strncpy(temp1->name, t1->name, key_len);
				int key_addr_len = strlen(t1->address);
				temp1->address = (char *)malloc(key_addr_len + 1);
				memset(temp1->address, 0, key_addr_len + 1);
				strncpy(temp1->address, t1->address, key_addr_len);
				tail1->next = temp1;
				tail1 = temp1;
				t1 = t1->next;
			}
			tail1->next = NULL;
			st.data[st.top - 1] = exp_array[count_key]->id + 'A';
			count_key++;
			st.top--;
			break;
		default:
			st.top++;
			st.data[st.top] = ch;

		}
		ch = exp[t];
		t++;
	}
}

bool trans_real_exp(char exp[], char real_exp[], doc_list exp_array[], int &count_key)
{
	int j = 0, k = 0;
	int len_exp = strlen(exp);
	int end;
	int start = 0;
	while (j < len_exp)
	{
		if (exp[j] == '(' || exp[j] == ')' || exp[j] == '|' || exp[j] == '&')
		{
			real_exp[k] = exp[j];
			k++;
			j++;
		}
		else
		{
			start = j;
			while (exp[j] != '('&&exp[j] != ')'&&exp[j] != '|'&&exp[j] != '&')
			{
				j++;
			}
			end = j;
			exp_array[count_key] = (DOCNODE *)malloc(sizeof(DOCNODE)* 1);
			exp_array[count_key]->id = count_key;
			real_exp[k] = count_key + 'A';
			exp_array[count_key]->name = new char[end - start + 1];
			int u = 0;
			for (; u < end - start; u++)
			{
				exp_array[count_key]->name[u] = exp[u + start];
			}
			exp_array[count_key]->name[u] = '\0';
			doc_list tail;
			tail = exp_array[count_key];
			key_list search_key = SearchByString(exp_array[count_key]->name);
			if (search_key == NULL)
			{
				cout << "关键字" << exp_array[count_key]->name << "搜索不到！" << endl;
				return false;
			}
			doc_list search_doc = search_key->next;
			while (search_doc != NULL)
			{
				doc_list temp = (DOCNODE *)malloc(sizeof(DOCNODE)* 1);
				temp->id = search_doc->id;
				int key_len = strlen(search_doc->name);
				temp->name = (char *)malloc(key_len + 1);
				memset(temp->name, 0, key_len + 1);
				strncpy(temp->name, search_doc->name, key_len);
				int key_addr_len = strlen(search_doc->address);
				temp->address = (char *)malloc(key_addr_len + 1);
				memset(temp->address, 0, key_addr_len + 1);
				strncpy(temp->address, search_doc->address, key_addr_len);
				tail->next = temp;
				tail = temp;
				search_doc = search_doc->next;
			}
			tail->next = NULL;
			k++;
			count_key++;

		}
	}
	real_exp[k] = '\0';
	return true;
}
