#pragma once
#include "Element.h"
#include <algorithm>


// PolygonElement 类：继承自 Element，表示一个多边形
class PolygonElement : public Element {
public:
    std::vector<CPoint> vertices;  // 存储多边形的顶点列表
    std::vector<CPoint> vertices_origion;  // 存储多边形的顶点列表

    PolygonElement() {}

    // 构造函数
    PolygonElement(const std::vector<CPoint>& points, COLORREF color = RGB(0, 0, 0))
        : Element(points[0], points[0], color) {
        vertices = points;  // 初始化顶点列表
        vertices_origion = vertices;
    }

    // 平移函数
    void translate(int dx, int dy) {
        for (auto& point : vertices) {
            point.Offset(dx, dy);
        }
        vertices_origion = vertices;
    }

    // 计算多边形的中点
    CPoint calculateCentroid() const {
        int sumX = 0, sumY = 0;

        for (const auto& point : vertices_origion) {
            sumX += point.x;
            sumY += point.y;
        }
        int n = vertices_origion.size();
        return CPoint(sumX / n, sumY / n);
    }

    // 缩放函数
    void scale(double scaleX, double scaleY, CPoint origin) {
        vertices = vertices_origion;
        origin = calculateCentroid();
        for (auto& point : vertices) {
            int x_new = (point.x - origin.x) * scaleX + origin.x;
            int y_new = (point.y - origin.y) * scaleY + origin.y;
            point = CPoint(x_new, y_new);
        }
    }

    // 旋转函数，旋转角度以弧度为单位
    void rotate(double angle, CPoint origin) {

        vertices = vertices_origion;
        origin = calculateCentroid();
        for (auto& point : vertices) {
            double radian = angle;  // 假设传入的角度是弧度制
            double cosAngle = cos(radian);
            double sinAngle = sin(radian);

            int x_new = cosAngle * (point.x - origin.x) - sinAngle * (point.y - origin.y) + origin.x;
            int y_new = sinAngle * (point.x - origin.x) + cosAngle * (point.y - origin.y) + origin.y;
            point = CPoint(x_new, y_new);
        }
    }

    // 绘制多边形
    void draw(CDC* pDC) override {
        for (size_t i = 0; i < vertices.size(); ++i) {
            // 绘制每一条边，连接当前顶点和下一个顶点
            CPoint start = vertices[i];
            CPoint end = vertices[(i + 1) % vertices.size()];  // 最后一个顶点和第一个顶点连接
            drawLine(pDC, start, end);
        }
    }

    // 填充多边形区域
    void fill(CDC* pDC) {

        if (vertices.size() < 3)
            return;

        // 计算多边形的上下边界
        int minY = INT_MAX, maxY = INT_MIN;
        for (const auto& point : vertices) {
            if (point.y < minY) {
                minY = point.y;
            }
            if (point.y > maxY) {
                maxY = point.y;
            }
        }


        // 对每一条扫描线进行处理
        for (int y = minY; y <= maxY; ++y) {
            std::vector<int> intersections;
            for (size_t i = 0; i < vertices.size(); ++i) {
                CPoint p1 = vertices[i];
                CPoint p2 = vertices[(i + 1) % vertices.size()];

                // 判断扫描线y是否与边相交
                if ((p1.y <= y && p2.y > y) || (p1.y > y && p2.y <= y)) {
                    // 计算交点的x坐标
                    int x_intersection = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                    intersections.push_back(x_intersection);
                }
            }

            // 对交点进行排序
            std::sort(intersections.begin(), intersections.end());

            // 填充交点之间的区域
            for (size_t i = 0; i < intersections.size(); i += 2) {
                if (i + 1 < intersections.size()) {
                    // 填充扫描线之间的区域
                    for (int x = intersections[i]; x <= intersections[i + 1]; ++x) {
                        pDC->SetPixel(x, y, color);  // 填充当前像素
                    }
                }
            }
        }
    }


    // 判断点是否在裁剪边内
    bool isInside(const CPoint& p, double edge, int axis, int sign) {
        if (axis == 0) {  // 水平裁剪边 (x = edge)
            return (sign == 1) ? p.x >= edge : p.x <= edge;
        }
        else {  // 垂直裁剪边 (y = edge)
            return (sign == 1) ? p.y >= edge : p.y <= edge;
        }
    }

    // 计算两条线段的交点
    CPoint intersect(const CPoint& p1, const CPoint& p2, double edge, int axis, int sign) {
        double x, y;
        if (axis == 0) {  // 水平裁剪边 (x = edge)
            y = p1.y + (edge - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
            return CPoint(edge, y);
        }
        else {  // 垂直裁剪边 (y = edge)
            x = p1.x + (edge - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
            return CPoint(x, edge);
        }
    }

    // 裁剪一个多边形与某个裁剪边的交集
    std::vector<CPoint> clipAgainstEdge(const std::vector<CPoint>& polygon, double edge, int axis, int sign) {
        std::vector<CPoint> output;
        CPoint prevPoint = polygon.back();  // 上一个点

        for (const auto& currentPoint : polygon) {
            bool prevInside = isInside(prevPoint, edge, axis, sign);
            bool currInside = isInside(currentPoint, edge, axis, sign);

            if (currInside) {
                if (!prevInside) {
                    // 计算交点
                    output.push_back(intersect(prevPoint, currentPoint, edge, axis, sign));
                }
                output.push_back(currentPoint);
            }
            else if (prevInside) {
                // 前一个点在裁剪区域内，计算交点
                output.push_back(intersect(prevPoint, currentPoint, edge, axis, sign));
            }
            prevPoint = currentPoint;
        }

        return output;
    }

    // Sutherland-Hodgman 算法，逐边裁剪多边形
    std::vector<CPoint> sutherlandHodgmanClip(const std::vector<CPoint>& polygon, const CPoint& min, const CPoint& max) {
        std::vector<CPoint> clipped = polygon;

        // 左边裁剪
        clipped = clipAgainstEdge(clipped, min.x, 0, 1);
        if (clipped.empty()) return clipped;

        // 右边裁剪
        clipped = clipAgainstEdge(clipped, max.x, 0, -1);
        if (clipped.empty()) return clipped;

        // 上边裁剪
        clipped = clipAgainstEdge(clipped, min.y, 1, 1);
        if (clipped.empty()) return clipped;

        // 下边裁剪
        clipped = clipAgainstEdge(clipped, max.y, 1, -1);

        return clipped;
    }

   



    // Sutherland-Hodgman矩形裁剪算法
    PolygonElement clipPolygon(Element* cliprect) {
        std::vector<CPoint> clipped = vertices;  // 初始化裁剪前的多边形顶点

        CPoint clipRectMin = cliprect->point1;  // 获取裁剪矩形的左下角
        CPoint clipRectMax = cliprect->point2;  // 获取裁剪矩形的右上角

     

        // 例子：定义一个多边形和裁剪矩形
        //std::vector<CPoint> polygon = {
        //    CPoint(1, 1),
        //    CPoint(5, 1),
        //    CPoint(5, 5),
        //    CPoint(1, 5)
        //};

        //CPoint clipMin(2, 2);  // 裁剪矩形的左下角
        //CPoint clipMax(4, 4);  // 裁剪矩形的右上角

        //// 执行 Sutherland-Hodgman 裁剪
        //std::vector<CPoint> clippedPolygon = sutherlandHodgmanClip(polygon, clipMin, clipMax);

       

        // 执行 Sutherland-Hodgman 裁剪
       std::vector<CPoint> clippedPolygon = sutherlandHodgmanClip(clipped, clipRectMin, clipRectMax);

        

        // 返回裁剪后的多边形
        return PolygonElement(clippedPolygon, color);
    }


private:
    // 辅助函数：绘制两点之间的直线
    void drawLine(CDC* pDC, CPoint start, CPoint end) {
        // Bresenham 直线算法的实现
        int x1 = start.x, y1 = start.y;
        int x2 = end.x, y2 = end.y;

        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy;

        while (true) {
            pDC->SetPixel(x1, y1, color);  // 绘制当前像素点

            if (x1 == x2 && y1 == y2) break;  // 如果到达终点，结束循环

            int e2 = err * 2;

            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }
};
