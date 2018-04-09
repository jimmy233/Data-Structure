#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
using namespace std;
char *filePath = "D:\\crawler_2"; //链接文档路径
typedef struct
{
	string str1;  //文件路径名
	string str2; //文件名
}Files;
typedef struct  //入度及出度
{
	string str;  //文件名
	int in; //入度
	int out; //出度
	double value;
}Files1;
#define size 152
#define sum 1

typedef struct doc_node
{
	int  id;  //文档ID
	char*name; //文档名
	char *address;//地址
	double PR;//pagerank值
	int in;//入度值
	struct doc_node *next;
}DOCNODE, *doc_list;
void getFiles(string path, Files *files)
{
	//文件句柄  
	long   hFile = 0;
	int i = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files[i].str1 = p.assign(path).append("\\").append(fileinfo.name);
				files[i].str2 = fileinfo.name;
				i++;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
void Pagerank(Files *files, Files1 *page)
{
	//获取该路径下的所有文件  
	getFiles(filePath, files); //得到每个文档的路径，以及文档名字
	double matrix[size][size] = { 0 }; //根据出入度建立的邻接矩阵
	/*统计每个文档的出入度数*/
	for (int i = 0; i < size; i++)
	{
		ifstream fin;
		int j = 0;
		string temp[100];
		fin.open(files[i].str1, ios::in);
		while (fin.good() && !fin.eof())
		{
			getline(fin, temp[j]);
			//cout << temp[j];
			string temp1 = temp[j];
			j++;
			if (temp1 == "")
			{
				j = j - 1;
			}
			if (temp1 == "inlink:")
			{
				page[i].out = j - 2;
				j = 0;
			}
		}
		if (j - 1 < 0)
			page[i].in = 0;
		else
			page[i].in = j;
		page[i].str = files[i].str2;
	}
	/*建立邻接矩阵*/
	for (int i = 0; i < size; i++)
	{
		ifstream fin;
		string temp;
		fin.open(files[i].str1, ios::in);
		while (fin.good() && !fin.eof())
		{
			getline(fin, temp);
			//cout << temp[j];
			string temp1 = temp;
			if (temp1 == "outlink:")continue;
			if (temp1 == "inlink:")break;
			temp1 = temp1.append(".txt");
			int m = 0;
			for (; m < size; m++)
			if (files[m].str2 == temp1)break;
			if (page[i].out == 0)
				matrix[m][i] = 0;
			else
				matrix[m][i] = (double)sum / (double)page[i].out;
		}
	}
	//page[0].str = files[0].str2;
	double q = 0.85;//误差修正因子
	//double result[size];//结果特征向量
	double e = double(sum) / double(size);
	//double e[size];  //单位量
	for (int i = 0; i < size; i++)
	{
		page[i].value = e;
		//e[i] = double(sum) / double(size);
	}
	int s = 0;
	double r[size];
	/*以上邻接矩阵建立完毕，以下实现pagerank计算 */
	do
	{
		for (int p = 0; p < size; p++)
		{
			r[p] = page[p].value;
		}
		for (int i = 0; i < size; i++)
		{
			double sumr = 0;
			for (int j = 0; j < size; j++)
			{
				sumr = sumr + matrix[i][j] * page[j].value;
			}
			page[i].value = sumr*q + (1 - q)*e;
		}
		int f;
		for (f = 0; f < size; f++)
		{
			if (fabs(r[f] - page[f].value)>10e-6)break;
		}
		if (f == size)
			s = 1;
	} while (s != 1);
}
/*-----------------------------------*/
void Insort(doc_list &hc, Files1 *page)
{
	DOCNODE *pa = hc;
	pa = pa->next;
	int f = 0;
	while (pa != NULL)
	{
		int i = 0;
		for (; i < size; i++)
		{
			string s = pa->name;
			if (s == page[i].str)break;
		}
		pa->in = page[i].in;
		pa->PR = page[i].value;
		pa = pa->next;
	}
	DOCNODE* tail = NULL;

	while (tail != hc->next)
	{
		DOCNODE* pre = hc;
		DOCNODE* cur = pre->next;
		while (cur != tail && cur->next != tail)
		{
			if (cur->in < cur->next->in)
			{
				//交换当前节点和后一个节点  
				pre->next = cur->next;
				cur->next = cur->next->next;
				pre->next->next = cur;
			}
			pre = pre->next;
			cur = pre->next;
		}

		tail = cur;
	}
}

void Pagesort(doc_list hc, Files1 *page)
{
	DOCNODE *pa = hc;
	pa = pa->next;
	while (pa != NULL)
	{
		int i = 0;
		for (; i < size; i++)
		{
			string s = pa->name;
			if (s == page[i].str)break;
		}
		pa->in = page[i].in;
		pa->PR = page[i].value;
		pa = pa->next;
	}
	DOCNODE* tail = NULL;

	while (tail != hc->next)
	{
		DOCNODE* pre = hc;
		DOCNODE* cur = pre->next;
		while (cur != tail && cur->next != tail)
		{
			if (cur->PR < cur->next->PR)
			{
				//交换当前节点和后一个节点  
				pre->next = cur->next;
				cur->next = cur->next->next;
				pre->next->next = cur;
			}
			pre = pre->next;
			cur = pre->next;
		}

		tail = cur;
	}
}
