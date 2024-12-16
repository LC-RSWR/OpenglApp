#pragma once
#include "Element.h"
#include "LineElement.h"


// RectangleElement 类：继承自 Element，表示一个矩形
class RectangleElement : public Element {
public:
    CPoint topLeft, bottomRight;  // 左上角和右下角的点
    
    // 构造函数：根据两个点构造矩形
    RectangleElement(CPoint point1, CPoint point2, COLORREF color = RGB(0, 0, 0))
        : Element(point1, point2, color) {

        // 计算左上角和右下角的点
      
        int minX = (point1.x < point2.x) ? point1.x : point2.x;  // 选择较小的 x
        int minY = (point1.y < point2.y) ? point1.y : point2.y;  // 选择较小的 y
        int maxX = (point1.x > point2.x) ? point1.x : point2.x;  // 选择较大的 x
        int maxY = (point1.y > point2.y) ? point1.y : point2.y;  // 选择较大的 y

        // 构造 CPoint 对象
        topLeft = CPoint(minX, minY);      // 左上角点
        bottomRight = CPoint(maxX, maxY);  // 右下角点


        point1 = topLeft;
        point2 = bottomRight;
    }

    void fill(CDC* pDC) {}  // 纯虚函数，派生类需要实现

    // 获取矩形的四个顶点
    std::vector<CPoint> getVertices() const {
        std::vector<CPoint> vertices;
        vertices.push_back(topLeft); // 左上角
        vertices.push_back(CPoint(bottomRight.x, topLeft.y)); // 右上角
        vertices.push_back(bottomRight); // 右下角
        vertices.push_back(CPoint(topLeft.x, bottomRight.y)); // 左下角
        return vertices;
    }

    // 绘制矩形的方法（使用四条边表示矩形）
    void draw(CDC* pDC) override {
        // 获取矩形的四个顶点
        std::vector<CPoint> vertices = getVertices();

        // 使用 Bresenham 直线算法绘制四条边
        LineElement topEdge(vertices[0], vertices[1], color);  // 上边
        LineElement rightEdge(vertices[1], vertices[2], color); // 右边
        LineElement bottomEdge(vertices[2], vertices[3], color); // 下边
        LineElement leftEdge(vertices[3], vertices[0], color);  // 左边

        // 绘制矩形四个边
        topEdge.draw(pDC);
        rightEdge.draw(pDC);
        bottomEdge.draw(pDC);
        leftEdge.draw(pDC);
    }

    // 平移函数
    void translate(int dx, int dy) {

        topLeft.Offset(dx, dy);
        bottomRight.Offset(dx, dy);

        point1 = topLeft;
        point2 = bottomRight;
    }

    // 缩放函数
    void scale(double scaleX, double scaleY, CPoint origin) {


        // 缩放公式:
        // x' = (x - origin.x) * scaleX + origin.x
        // y' = (y - origin.y) * scaleY + origin.y

        int mid_x = (topLeft.x + bottomRight.x) / 2;
        int mid_y = (topLeft.y + bottomRight.y) / 2;
        CPoint mid(mid_x, mid_y);
        origin = mid;

        int x1_new = (point1.x - origin.x) * scaleX + origin.x;
        int y1_new = (point1.y - origin.y) * scaleY + origin.y;
        topLeft = CPoint(x1_new, y1_new);

        int x2_new = (point2.x - origin.x) * scaleX + origin.x;
        int y2_new = (point2.y - origin.y) * scaleY + origin.y;
        bottomRight = CPoint(x2_new, y2_new);
    }

    // 旋转函数
    void rotate(double angle, CPoint origin) {
        // 旋转矩阵:
        // x' = cos(angle) * (x - origin.x) - sin(angle) * (y - origin.y) + origin.x
        // y' = sin(angle) * (x - origin.x) + cos(angle) * (y - origin.y) + origin.y

        int mid_x = (topLeft.x + bottomRight.x) / 2;
        int mid_y = (topLeft.y + bottomRight.y) / 2;
        CPoint mid(mid_x, mid_y);
        origin = mid;

        // 对point1进行旋转
        double radian = angle;  // 假设传入的角度是弧度制
        double cosAngle = cos(radian);
        double sinAngle = sin(radian);

        int x1_new = cosAngle * (topLeft.x - origin.x) - sinAngle * (topLeft.y - origin.y) + origin.x;
        int y1_new = sinAngle * (topLeft.x - origin.x) + cosAngle * (topLeft.y - origin.y) + origin.y;
        topLeft = CPoint(x1_new, y1_new);

        // 对point2进行旋转
        int x2_new = cosAngle * (bottomRight.x - origin.x) - sinAngle * (bottomRight.y - origin.y) + origin.x;
        int y2_new = sinAngle * (bottomRight.x - origin.x) + cosAngle * (bottomRight.y - origin.y) + origin.y;
        bottomRight = CPoint(x2_new, y2_new);
    }


};
