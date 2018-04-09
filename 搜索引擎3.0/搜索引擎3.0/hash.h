#define _CRT_SECURE_NO_WARNINGS
#include "string.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include  <stdlib.h>
#define  TABLE_SIZE 0x1000000  //����hash������
#define WORD_MAX_LEN 1024
using namespace std;
/*pagerank�����һЩ�ṹ�� �� ����*/
typedef struct
{
	string str1;  //�ļ�·����
	string str2; //�ļ���
}Files;
typedef struct  //��ȼ�����
{
	string str;  //�ļ���
	int in; //���
	int out; //����
	double value;
}Files1;
#define size 152  //�ļ���
#define sum 1    //��������
/*------------------------------------*/
typedef struct
{
	int nHashA;
	int nHashB;
	char bExists;
}SOMESTRUCTRUE; //nHashA��nHashB����У��
typedef struct key_node
{
	char *pkey;     // �ؼ���ʵ��
	int count;      // �ؼ��ʳ��ִ���
	int pos;        // �ؼ�����hash����λ��
	struct doc_node *next;  // ָ���ĵ����
}KEYNODE, *key_list;
key_list key_array[TABLE_SIZE];
typedef struct doc_node
{
	int  id;  //�ĵ�ID
	char*name; //�ĵ���
	char *address;//�ĵ���ҳ��ַ
	double PR;//pagerankֵ
	int in;//���ֵ
	struct doc_node *next;
}DOCNODE, *doc_list;
unsigned long cryptTable[0x1000000];
void preparecrypttable() //��ʼ��hash��
{
	unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
	for (index1 = 0; index1 < 0x100; index1++)
	{
		for (index2 = index1, i = 0; i < 10000; i++, index2 += 0x100)
		{
			unsigned long temp1, temp2;
			seed = (seed * 125 + 3) % 0x2aaaab;
			temp1 = (seed & 0xffff) << 0x10;
			seed = (seed * 125 + 3) % 0x2aaaab;
			temp2 = seed & 0xffff;
			cryptTable[index2] = (temp1 | temp2);
		}
	}

}

unsigned long HashString(char *lpszFileName, unsigned long dwHashType)
{
	unsigned char*key = (unsigned char*)lpszFileName;
	unsigned long seed1 = 0x7fed7fed;
	unsigned long seed2 = 0xeeeeeeee;

	int ch;
	while (*key != 0)
	{
		ch = *key++;
		seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
	}
	return seed1;
}
SOMESTRUCTRUE HashTable[TABLE_SIZE];
key_list SearchByString(char *string_in)
{
	int HASH_OFFSET = 0, HASH_C = 1, HASH_D = 2;
	unsigned int nHash = HashString(string_in, HASH_OFFSET);
	unsigned int nHashC = HashString(string_in, HASH_C);
	unsigned int nHashD = HashString(string_in, HASH_D);
	unsigned int nHashStart = nHash % TABLE_SIZE;
	unsigned int nHashPos = nHashStart;

	while (HashTable[nHashPos].bExists)
	{
		if (HashTable[nHashPos].nHashA == (int)nHashC && HashTable[nHashPos].nHashB == (int)nHashD)
		{
			break;
			//��ѯ����벻ͬ���˴������޸�
		}
		else
		{
			nHashPos = (nHashPos + 1) % TABLE_SIZE;
		}

		if (nHashPos == nHashStart)
		{
			break;
		}
	}

	if (key_array[nHashPos] && strlen(key_array[nHashPos]->pkey))
	{
		return key_array[nHashPos];
	}

	return NULL;
}

//��������ѯ������ɹ����عؼ��֣��˺����ڱ�����û�б��õ������Ժ��ԣ�
key_list SearchByIndex(unsigned int nIndex)
{
	unsigned int nHashPos = nIndex;
	if (nIndex < TABLE_SIZE)
	{
		if (key_array[nHashPos] && strlen(key_array[nHashPos]->pkey))
		{
			return key_array[nHashPos];
		}
	}

	return NULL;
}

//����ؼ��֣�����ɹ�����hashֵ
int InsertString(char *str)
{
	int HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
	unsigned int nHash = HashString(str, HASH_OFFSET);
	unsigned int nHashA = HashString(str, HASH_A);
	unsigned int nHashB = HashString(str, HASH_B);
	unsigned int nHashStart = nHash % TABLE_SIZE;
	unsigned int nHashPos = nHashStart;
	int len;

	while (HashTable[nHashPos].bExists)
	{
		nHashPos = (nHashPos + 1) % TABLE_SIZE;

		if (nHashPos == nHashStart)
			break;
	}

	len = strlen(str);
	if (!HashTable[nHashPos].bExists && (len < WORD_MAX_LEN))
	{
		HashTable[nHashPos].nHashA = nHashA;
		HashTable[nHashPos].nHashB = nHashB;

		key_array[nHashPos] = (KEYNODE *)malloc(sizeof(KEYNODE)* 1);
		if (key_array[nHashPos] == NULL)
		{
			printf("10000 EMS ERROR !!!!\n");
			return 0;
		}

		key_array[nHashPos]->pkey = (char *)malloc(len + 1);
		if (key_array[nHashPos]->pkey == NULL)
		{
			printf("10000 EMS ERROR !!!!\n");
			return 0;
		}

		memset(key_array[nHashPos]->pkey, 0, len + 1);
		strncpy(key_array[nHashPos]->pkey, str, len);
		*((key_array[nHashPos]->pkey) + len) = 0;
		key_array[nHashPos]->pos = nHashPos;
		key_array[nHashPos]->count = 1;
		key_array[nHashPos]->next = NULL;
		HashTable[nHashPos].bExists = 1;
		return nHashPos;
	}

	if (HashTable[nHashPos].bExists)
		printf("30000 in the hash table %s !!!\n", str);
	else
		printf("90000 strkey error !!!\n");
	return -1;
}
