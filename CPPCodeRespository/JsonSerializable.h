#pragma once
#ifndef _JSONSERIALIZABLE_INCLUDE_
#define _JSONSERIALIZABLE_INCLUDE_

#include<string>
#include<iostream>
#include<map>
#include "Include\json\json\json.h"
/*
	//测试使用数据
	Json::Value myBody;
	myBody["uid"] = "11111111111111";
	myBody["apiVersion"] = "1.0";
	myBody["hid"] = "win7";
	myBody["osVersion"] = "2019";
	myBody["currentVersion"] = "3.0.2.9";
	myBody["macAddr"] = "1.1.1.1";
*/
//Json对象转成String
std::string  JsonObjectToString(Json::Value jsonValue)
{
	return Json::FastWriter().write(jsonValue);
}
//string转成Json对象
Json::Value JsonStringToObject(std::string strObj)
{
	Json::Reader reader;
	Json::Value jsonValue;
	if (reader.parse(strObj, jsonValue, false))
	{
		return jsonValue;
	}
	return NULL;
}

/*
	//测试使用数据
	std::map<std::string, std::string> mapTest;
	mapTest.insert(std::pair<std::string, std::string>("name","test"));
	mapTest.insert(std::pair<std::string, std::string>("address", "beijing"));
	mapTest.insert(std::pair<std::string, std::string>("age","18"));
*/
//使用map构建Json
Json::Value JsonBulid(std::map<std::string,std::string> mapValue)
{
	Json::Value value;
	if (!mapValue.empty() )
	{
		std::map<std::string, std::string>::iterator iter = mapValue.begin();
		while (iter != mapValue.end())
		{
			Json::Value obj;
			obj[iter->first] = iter->second;
			value.append(obj);
			std::cout << iter->first << ":" << iter->second << std::endl;
			iter++;
		}
		/*for (iter = mapValue.begin;iter != mapValue.end();iter++)
		{
			Json::Value obj;
			obj[iter->first] = iter->second;
			value.append(obj);
		}*/
	}
	return value;
}

#endif