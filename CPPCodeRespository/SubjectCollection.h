#include<cstdlib>
#include"assert.h"
#include<iostream>
#include"process.h"
class Singleton {
private:
	Singleton() {
		std::cout << std::endl;
		std::cout << "Singleton()..." << std::endl; }
	Singleton(const Singleton &) {}
	Singleton & operator=(const Singleton& ) {}
	
public:
	~Singleton() {
		std::cout << std::endl;
		std::cout << "~Singleton()..." << std::endl; }
	static Singleton &getInstance()
	{
		static Singleton instance;
		return instance;
	}
};

unsigned int __stdcall threadFun(void *ptr)
{
	for (int num = 0; num < 10; num++)
	{
		std::cout << "Singleton::getInstance():" << &(Singleton::getInstance()) << std::endl;
	}
	return 0;
}
void SingleTest()
{
	for (int num = 0; num < 100; num++)
	{
		std::cout << "_beginthreadex: " << num << std::endl;
		_beginthreadex(nullptr, 0, threadFun, nullptr, 0, nullptr);
	}

}
struct LinkNode {
	int num;
	LinkNode * next=nullptr;
};
LinkNode * SingleLinkReverse(LinkNode * head)
{
	LinkNode * pNode = head;
	LinkNode * pPrev = nullptr;
	LinkNode * pNext = nullptr;
	LinkNode * pReverseHerad = nullptr;
	while (pNode != nullptr)
	{
		pNext = pNode->next;
		if (pNext == nullptr)
			pReverseHerad = pNode;
		pNode->next = pPrev;
		pPrev = pNode;
		pNode = pNext;
	}
	return pReverseHerad;
}
LinkNode *SingleLinkCreate(int arr[],int length)
{
	LinkNode *link = new LinkNode;
	LinkNode *pHead = link;
	for(int i=0;i<length;i++)
	{
		LinkNode * node = new LinkNode;
		node->num = arr[i];

		link->next = node;
		link = link->next;
	}
	return pHead;
}
LinkNode * SingleLinkInsert(LinkNode * head,int num)
{
	LinkNode *pNode = head;
	LinkNode * inNode = new LinkNode;
	inNode->num = num;
	LinkNode * prvNode = nullptr;
	while (pNode != nullptr && inNode->num > pNode->num)
	{
		prvNode = pNode;
		pNode = pNode->next;
	}
	if (pNode == nullptr)
	{
		prvNode->next = inNode;
	}
	else
	{
		prvNode->next = inNode;
		inNode->next = pNode;
		
	}
	return head;
}
LinkNode * SingleLinkInsertNode(LinkNode * head, int num)
{
	LinkNode * pNode = head;

	LinkNode * inNode = new LinkNode;
	inNode->num = num;
	inNode->next = nullptr;

	if (pNode == nullptr)
	{
		head = inNode;
	}
	else
	{
		while (pNode->next != nullptr)
		{
			pNode = pNode->next;
		}
		pNode->next = inNode;
	}
	return head;
}
LinkNode * SingleLinkDelete(LinkNode *head,int num)
{
	LinkNode * node = head;
	LinkNode * pPrev = nullptr;
	if (head == nullptr)
		return nullptr;
	if (node->num == num)
	{
		head = node->next;
		delete node;
		node = nullptr;
		return head;
	}
	while (node != nullptr && node->num != num)
	{
		pPrev = node;
		node = node->next;
	}
	if (node != nullptr)
	{
		LinkNode *delNode = node;
		if (node->next != nullptr)
		{
			pPrev->next = node->next;
			delete delNode;
		}
		else
		{
			pPrev->next = nullptr;
		}
	}
	return head;
}
void TestLinkNode()
{
	LinkNode * node = SingleLinkInsertNode(nullptr, 100);
	node = SingleLinkInsertNode(node, 101);
	int arr[] = {0,4,8,12,16,20,24,28};
	LinkNode * link = SingleLinkCreate(arr,sizeof(arr)/sizeof(int));
	link = SingleLinkInsert(link, -1);
	link = SingleLinkInsert(link, 1);
	link = SingleLinkInsert(link, 1);
	link = SingleLinkInsert(link, 3);
	link = SingleLinkInsert(link, 30);
	link = SingleLinkDelete(node, 101);
	LinkNode * temp= SingleLinkReverse(link);
	std::cout << std::endl;
}
char* mystrcpy(char* dst, const char* src)
{
	assert(dst != NULL);
	assert(src != NULL);

	if (dst == src)
		return dst;

	int size = strlen(src) + 1;

	if ((dst  < src) || (src + size < dst))
	{
		char* d = dst;
		const char* s = src;

		while (size--)
			*d++ = *s++;
	}
	else
	{
		char* d = dst + size - 1;
		const char* s = src + size - 1;

		while (size--)
			*d-- = *s--;
	}
	return dst;
}