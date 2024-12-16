#pragma once
#include "Element.h"

// LineElement 类：继承自 Element，表示一条直线 // Bresenham 直线算法实现
class LineElement : public Element {
public:
    CPoint point1_origion, point2_origion;  // 使用 CPoint 来表示两个点
    // 构造函数
    LineElement(CPoint startPoint, CPoint endPoint, COLORREF color = RGB(0, 0, 0))
        : Element(startPoint, endPoint, color) {

        point1_origion = startPoint;
        point2_origion = endPoint;
    }

    void fill(CDC* pDC) {}  // 纯虚函数，派生类需要实现

    // 平移函数
    void translate(int dx, int dy) {
        point1.Offset(dx, dy);
        point2.Offset(dx, dy);

        point1_origion = point1;
        point2_origion = point2;
    }


    void scale(double scaleX, double scaleY, CPoint origin) {
        // 缩放公式:
        // x' = (x - origin.x) * scaleX + origin.x
        // y' = (y - origin.y) * scaleY + origin.y

        int mid_x = (point1.x + point2.x) / 2;
        int mid_y = (point1.y + point2.y) / 2;
        CPoint mid(mid_x, mid_y);
        origin = mid;

        int x1_new = (point1_origion.x - origin.x) * scaleX + origin.x;
        int y1_new = (point1_origion.y - origin.y) * scaleY + origin.y;
        point1 = CPoint(x1_new, y1_new);

        int x2_new = (point2_origion.x - origin.x) * scaleX + origin.x;
        int y2_new = (point2_origion.y - origin.y) * scaleY + origin.y;
        point2 = CPoint(x2_new, y2_new);
    }

    // 旋转函数，旋转角度以弧度为单位
    void rotate(double angle, CPoint origin) {
        // 旋转矩阵:
        // x' = cos(angle) * (x - origin.x) - sin(angle) * (y - origin.y) + origin.x
        // y' = sin(angle) * (x - origin.x) + cos(angle) * (y - origin.y) + origin.y

        int mid_x = (point1.x + point2.x) / 2;
        int mid_y = (point1.y + point2.y) / 2;
        CPoint mid(mid_x, mid_y);
        origin = mid;

        // 对point1进行旋转
        double radian = angle;  // 假设传入的角度是弧度制
        double cosAngle = cos(radian);
        double sinAngle = sin(radian);

        int x1_new = cosAngle * (point1.x - origin.x) - sinAngle * (point1.y - origin.y) + origin.x;
        int y1_new = sinAngle * (point1.x - origin.x) + cosAngle * (point1.y - origin.y) + origin.y;
        point1 = CPoint(x1_new, y1_new);

        // 对point2进行旋转
        int x2_new = cosAngle * (point2.x - origin.x) - sinAngle * (point2.y - origin.y) + origin.x;
        int y2_new = sinAngle * (point2.x - origin.x) + cosAngle * (point2.y - origin.y) + origin.y;
        point2 = CPoint(x2_new, y2_new);
    }

    // Bresenham 直线算法实现
    void draw(CDC* pDC) override {
        // 使用局部变量保存原始坐标，避免修改原始坐标
        int x1_temp = point1.x;
        int y1_temp = point1.y;
        int x2_temp = point2.x;
        int y2_temp = point2.y;

        // 计算差值
        int dx = abs(x2_temp - x1_temp);
        int dy = abs(y2_temp - y1_temp);

        // 计算步进方向
        int sx = x1_temp < x2_temp ? 1 : -1;
        int sy = y1_temp < y2_temp ? 1 : -1;

        // 计算误差
        int err = dx - dy;

        // Bresenham 算法的核心循环
        while (true) {
            // 绘制当前像素点
            pDC->SetPixel(x1_temp, y1_temp, color);

            // 如果到达终点，结束循环
            if (x1_temp == x2_temp && y1_temp == y2_temp) break;

            // 计算误差的两倍值
            int e2 = err * 2;

            // 更新误差和坐标
            if (e2 > -dy) {
                err -= dy;
                x1_temp += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1_temp += sy;
            }
        }
    }
};
