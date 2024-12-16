#pragma once
#include <afxwin.h>  // MFC 基本头文件
#include <cmath>     // 数学函数 abs()
#include <vector>
// Element 类：通用元素
class Element {
public:
    CPoint point1, point2;  // 使用 CPoint 来表示两个点
    COLORREF color;         // 颜色属性

    // 构造函数
    Element(CPoint startPoint, CPoint endPoint, COLORREF color = RGB(0, 0, 0))
        : point1(startPoint), point2(endPoint), color(color) {
    }

    bool fillmode = false;
    virtual void fill(CDC* pDC) = 0;  // 纯虚函数，派生类需要实现

    Element(){}

    virtual void draw(CDC* pDC) = 0;  // 纯虚函数，派生类需要实现

    virtual void  translate(int dx, int dy) = 0;

    // 旋转函数，旋转角度以弧度为单位
    virtual void rotate(double angle, CPoint origin) = 0; 

    // 缩放函数，围绕原点进行缩放
    virtual void scale(double scaleX, double scaleY, CPoint origin) = 0;
    
};
