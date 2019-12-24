#pragma once
#include <iostream>
#include<functional>
using namespace std;
function<int(int n1, int n2)> funAddnum;
int addnum(int n1, int n2)
{
	return n1 + n2;
}

function<int(int n1, int n2)> sumnum = [](int n1, int n2)->int {return n1 - n2; };
