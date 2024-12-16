#pragma once
#include "Element.h"

// CircleElement 类：继承自 Element，表示一个圆形
class CircleElement : public Element {
public:
    int radius;  // 圆的半径
    int radius_origion;
    CPoint center;  // 圆心

    CircleElement(CPoint center, int radius, COLORREF color = RGB(0, 0, 0))
        : Element(center, center, color), center(center), radius(radius) {

        radius_origion = radius;
    }

    void fill(CDC* pDC) {}  // 纯虚函数，派生类需要实现

    void setRadius(int r) {
        radius = r;
        radius_origion = radius;
    }

    // 平移函数
    void translate(int dx, int dy) {
        center.Offset(dx, dy);
    }

    void scale(double scaleX, double scaleY, CPoint origin) {
        radius = radius_origion * scaleX;
    }

    virtual void rotate(double angle, CPoint origin){}

    void draw(CDC* pDC) override {
        int x = 0;
        int y = radius;
        int p = 1 - radius;

        while (x <= y) {
            pDC->SetPixel(center.x + x, center.y + y, color);
            pDC->SetPixel(center.x - x, center.y + y, color);
            pDC->SetPixel(center.x + x, center.y - y, color);
            pDC->SetPixel(center.x - x, center.y - y, color);
            pDC->SetPixel(center.x + y, center.y + x, color);
            pDC->SetPixel(center.x - y, center.y + x, color);
            pDC->SetPixel(center.x + y, center.y - x, color);
            pDC->SetPixel(center.x - y, center.y - x, color);

            if (p < 0) {
                p = p + 2 * x + 3;
            }
            else {
                p = p + 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }
};
