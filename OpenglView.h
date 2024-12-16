
// 231040100322View.h: COpenglView 类的接口
//

#pragma once
#include "Draw.h"
#include "PolygonElement.h"
#include "OpenGLRenderer.h"

enum ElementType
{
	None,
	Line,
	Circle,
	RectangleSqure,
	PolygonSqure,
	OpenglRender // Opengl场景漫游环境绘制
};

class COpenglView : public CView
{
protected: // 仅从序列化创建
	COpenglView() noexcept;
	DECLARE_DYNCREATE(COpenglView)

// 特性
public:
	COpenglDoc* GetDocument() const;

private:
	Draw draw_system;

	ElementType current_type = OpenglRender;

	// 在你的视图类中或对话框类中
	COLORREF m_selectedColor; // 用于保存选择的颜色
	void OnSelectColor();
// 操作

	PolygonElement* item = nullptr;
	std::vector<CPoint> mouse_points;
	CPoint m_startPoint;  // 起始点
	CPoint m_endPoint;    // 终止点
	int m_radius;         // 圆的半径

	// 定时器 ID 和平移相关变量
	UINT_PTR m_timerID;     // 定时器 ID
	// 平移相关变量
	int m_moveDistance;      // 当前平移的距离
	int m_moveDirection;     // 平移方向 (1: 右, -1: 左)
	bool m_moveInProgress;   // 是否进行平移操作

	// 缩放相关变量
	float m_scaleFactor;     // 当前缩放因子
	int m_scaleDirection;    // 缩放方向 (1: 放大, -1: 缩小)
	bool m_scaleInProgress;  // 是否进行缩放操作

	// 旋转相关变量
	float m_rotateAngle;     // 当前旋转角度
	int m_rotateDirection;   // 旋转方向 (1: 顺时针, -1: 逆时针)
	bool m_rotateInProgress; // 是否进行旋转操作

	bool clearing = false;
	OpenGLRenderer m_renderer;  // OpenGL 渲染对象

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnSize(UINT nType, int cx, int cy);
	// 双击事件处理
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	// 通过鼠标事件获取直线和圆的坐标信息
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	// 通过鼠标事件获取直线和圆的坐标信息
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);    // 处理鼠标移动


protected:



// 实现
public:
	virtual ~COpenglView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDrawLine();
	afx_msg void OnCircle();
	afx_msg void OnTranslate();

	afx_msg void OnTimer(UINT_PTR nIDEvent);  // 处理定时器事件
	afx_msg void OnRotate();
	afx_msg void OnScale();
	afx_msg void OnStopTimer();
	afx_msg void OnPolygon();
	afx_msg void OnClear();
	afx_msg void OnFill();
	afx_msg void OnMenuFill();
	afx_msg void OnRect();
	afx_msg void OnClip();
	afx_msg void OnOpenglRender();
	afx_msg void OnInitialUpdate();
	// 处理键盘事件
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // 231040100322View.cpp 中的调试版本
inline COpenglDoc* COpenglView::GetDocument() const
   { return reinterpret_cast<COpenglDoc*>(m_pDocument); }
#endif

