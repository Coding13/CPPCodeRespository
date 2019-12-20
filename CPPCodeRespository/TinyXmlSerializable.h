#pragma once
#ifndef _TINYXMLSERIALIABLE_INCLUDE_
#define _TINYXMLSERIALIABLE_INCLUDE_
#include "tinyxml2.h"
using namespace tinyxml2;

#include <iostream>
using namespace std;

//�������е�Ԫ�ص�����ֵ
void GetElementValue(XMLElement * element)
{
	for (XMLElement* curElement = element->FirstChildElement(); curElement; curElement = curElement->NextSiblingElement())
	{
		XMLElement* tmpElement= curElement;
		if (curElement->GetText() != NULL)
			cout << curElement->Name() << ":" << curElement->GetText() << endl;
		if (!tmpElement->NoChildren())
			GetElementValue(tmpElement);
	}
}

void test()
{
	tinyxml2::XMLDocument doc;
	int xmlRet = doc.LoadFile("C:/Users/Administrator/Documents/YYEinvoiceClientV2/invoice/��ֵ˰��ͨ��Ʊ_1205102366541897728.xml");
	if (xmlRet != 0)
	{
		return ;
	}
	tinyxml2::XMLElement *root = doc.RootElement();

	GetElementValue(root);
}
#endif // !_TINYXMLSERIALIABLE_INCLUDE_
