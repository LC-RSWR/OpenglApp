#pragma once
#include <afxwin.h>  // MFC 基本头文件
#include <cmath>     // 数学函数 abs()
#include "LineElement.h"
#include "CircleElement.h"
#include <vector>

class Draw
{
public:
	Draw();
	~Draw();

	// Bresenham 直线算法实现
	void draw(CDC* pDC);

public:
	std::vector<Element*> elements;
};

