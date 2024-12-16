
// 231040100322View.cpp: COpenglView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "OpenglApp.h"
#endif

#include "OpenglDoc.h"
#include "OpenglView.h"
#include "LineElement.h"
#include "CircleElement.h"
#include "PolygonElement.h"
#include "RectangleElement.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 定义定时器标识符
#define TIMER_ID 1

// COpenglView

IMPLEMENT_DYNCREATE(COpenglView, CView)

BEGIN_MESSAGE_MAP(COpenglView, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()  // 绑定左键按下消息
	ON_WM_RBUTTONDOWN()  // 绑定右键按下消息
	ON_WM_MOUSEMOVE()    // 绑定鼠标滑动消息
	ON_WM_TIMER()  // 定时器事件
	// 其它消息映射...
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_32771, &COpenglView::OnDrawLine)
	ON_COMMAND(ID_32772, &COpenglView::OnCircle)
	ON_COMMAND(ID_32773, &COpenglView::OnTranslate)
	ON_COMMAND(ID_32774, &COpenglView::OnRotate)
	ON_COMMAND(ID_32775, &COpenglView::OnScale)
	ON_COMMAND(ID_32776, &COpenglView::OnStopTimer)
	ON_COMMAND(ID_32778, &COpenglView::OnPolygon)
	ON_COMMAND(ID_32779, &COpenglView::OnClear)
	ON_COMMAND(ID_32780, &COpenglView::OnFill)
	ON_COMMAND(ID_Menu32781, &COpenglView::OnMenuFill)
	ON_COMMAND(ID_32783, &COpenglView::OnRect)
	ON_COMMAND(ID_32784, &COpenglView::OnClip)
	ON_COMMAND(ID_OPENGL32785, &COpenglView::OnOpenglRender)
END_MESSAGE_MAP()

// COpenglView 构造/析构

COpenglView::COpenglView() noexcept:m_timerID(0), m_moveDirection(1), m_moveDistance(0)
{
	// TODO: 在此处添加构造代码

}

COpenglView::~COpenglView()
{
	// TODO: 在此添加命令处理程序代码
	for (int i = 0; i < draw_system.elements.size(); ++i)
	{
		auto pElement = draw_system.elements[i];
		if (pElement) {
			delete pElement;
			pElement = nullptr;
		}
	}
	draw_system.elements.clear();
}

BOOL COpenglView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	if (current_type == OpenglRender)
	{
		if (!CView::PreCreateWindow(cs))
			return FALSE;

		cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;  // 确保 OpenGL 绘制区域不被其他控件干扰
		return TRUE;
	}
	else
	{
		return CView::PreCreateWindow(cs);
	}

}

void COpenglView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	m_renderer.ResizeOpenGL(cx, cy);  // 调整 OpenGL 渲染区域
}


// 鼠标双击事件
void COpenglView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (current_type == OpenglRender)
	{
		return;
	}
	current_type = None;
	mouse_points.clear();
	if (item)
	{
		if (!item->vertices.empty())
		{
			item->vertices.pop_back();
		}
		
		item = nullptr;
	}
	Invalidate();  // 触发重绘
	
}

void COpenglView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (current_type == OpenglRender)
	{
		return;
	}
	mouse_points.push_back(point);

	switch (current_type)
	{
	case None:
		break;
	case Line:
		if (mouse_points.size() == 2)
		{
			// 创建并绘制一条直线
			LineElement* line = new LineElement(mouse_points[0], mouse_points[1], m_selectedColor);
			draw_system.elements.push_back(line);
			mouse_points.clear();
		}
		break;
	case Circle:
		if (mouse_points.size() == 2)
		{
			// 计算两点之间的距离作为圆的半径
			int dx = mouse_points[1].x - mouse_points[0].x;  // x2 - x1
			int dy = mouse_points[1].y - mouse_points[0].y;  // y2 - y1

			double distance = sqrt(dx * dx + dy * dy);  // 欧几里得距离公式

			// 创建并绘制一个绿色圆形
			CircleElement* circle = new CircleElement(mouse_points[0], distance, m_selectedColor);
			draw_system.elements.push_back(circle);
			mouse_points.clear();
		}
		break;

	case ElementType::RectangleSqure :
		if (mouse_points.size() == 2)
		{
			// 创建并绘制矩形
			RectangleElement* rect = new RectangleElement(mouse_points[0], mouse_points[1], m_selectedColor);
			draw_system.elements.push_back(rect);
			mouse_points.clear();
		}
		break;

	case PolygonSqure:
		// 创建并绘制一个绿色圆形
		if (!item)
		{
			item = new PolygonElement(mouse_points, m_selectedColor);
			draw_system.elements.push_back(item);
		}
		else
		{
			item->vertices = mouse_points;
		}
	
		break;

	default:
		break;
	}
	
	
	Invalidate();  // 触发重绘

	// 获取鼠标点击的坐标作为直线的起点
	if (m_startPoint == CPoint(0, 0)) {
		m_startPoint = point;
	}
	else {
		m_endPoint = point;
		
	}
	CView::OnLButtonDown(nFlags, point);
}

void COpenglView::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnSelectColor();
}

void COpenglView::OnMouseMove(UINT nFlags, CPoint point)
{

}

// COpenglView 绘图

void COpenglView::OnDraw(CDC* pDC)
{
	COpenglDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (current_type == OpenglRender)
	{
		// 清空窗口并渲染场景
		m_renderer.RenderScene();
		return;
	}
	

	if (clearing)
	{
		// TODO: 在此添加命令处理程序代码
		for (int i = 0; i < draw_system.elements.size(); ++i)
		{
			auto pElement = draw_system.elements[i];
			if (pElement) {
				delete pElement;
				pElement = nullptr;
			}
		}
		draw_system.elements.clear();
		Invalidate();  // 触发重绘
		clearing = false;
	}

	

	// TODO: 在此处为本机数据添加绘制代码
	draw_system.draw(pDC);

	for (auto it : draw_system.elements)
	{
		if (auto polygon = dynamic_cast<PolygonElement*>(it)) {

			if (polygon->fillmode)
			{
				polygon->fill(pDC);
			}
			
		}
	}


	// 视图的绘制代码
	// 在这里你可以画你的图形，下面只是示范绘制文本

	CString message;
	switch (current_type)
	{
	case Line:
		message = L"当前操作: 绘制直线 鼠标左键分别按下确认两点";
		break;
	case Circle:
		message = L"当前操作: 绘制圆形 鼠标左键首次是确认圆心 再次按下将计算两点距离作为半径R";
		break;
	case ElementType::RectangleSqure:
		message = L"当前操作: 绘制矩形 鼠标左键分别按下确认两点";
		break;
	case PolygonSqure:
		message = L"当前操作: 绘制多边形 鼠标左键依次按下 双击终止";
		break;
	default:
		message = L"当前操作: 无操作 右键单击弹出颜色对话框，选择当前待操作的图形颜色";
		break;
	}

	// 设置文本颜色
	pDC->SetTextColor(RGB(0, 0, 255));  // 设置蓝色文字
	pDC->TextOutW(10, 10, message);     // 在屏幕左上角显示文本

}


void COpenglView::OnSelectColor()
{
	// 创建颜色对话框
	CColorDialog colorDlg(m_selectedColor);  // 初始化为当前选中的颜色

	// 显示对话框并检查用户是否点击了“确定”
	if (colorDlg.DoModal() == IDOK)
	{
		// 获取用户选择的颜色
		m_selectedColor = colorDlg.GetColor();

		// 你可以在这里使用这个颜色，比如更新界面，或做其他处理
		// 例如，调用一个重绘函数以展示新颜色
		Invalidate(); // 强制重绘视图
	}
}

// COpenglView 诊断

#ifdef _DEBUG
void COpenglView::AssertValid() const
{
	CView::AssertValid();
}

void COpenglView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenglDoc* COpenglView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenglDoc)));
	return (COpenglDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenglView 消息处理程序


void COpenglView::OnDrawLine()
{
	// TODO: 在此添加命令处理程序代码
	current_type = Line;
	Invalidate();  // 触发重绘
	mouse_points.clear();
}


void COpenglView::OnCircle()
{
	// TODO: 在此添加命令处理程序代码
	current_type = Circle;
	Invalidate();  // 触发重绘
	mouse_points.clear();
}



// 定时器事件处理函数
void COpenglView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID) {

		if (current_type == OpenglRender)
		{
			m_renderer.m_rotation[0] += 1;
		}
		else 
		{
			// 处理平移
			if (m_moveInProgress) {
				// 计算新的平移距离
				m_moveDistance += m_moveDirection * 10;  // 每次平移 10 个单位
				for (auto& it : draw_system.elements) {
					it->translate(m_moveDirection * 10, 0);  // 水平方向平移
				}

				// 如果平移达到500，则反转方向
				if (m_moveDistance >= 500) {
					m_moveDirection = -1;  // 向左平移
				}
				else if (m_moveDistance <= 0) {
					m_moveDirection = 1;   // 向右平移
				}
			}

			// 处理缩放
			if (m_scaleInProgress) {
				// 计算新的缩放比例
				m_scaleFactor += m_scaleDirection * 0.05f;  // 每次增加0.05的比例
				for (auto& it : draw_system.elements) {
					it->scale(m_scaleFactor, m_scaleFactor, CPoint(0, 0));  // 对元素应用缩放
				}

				// 如果缩放达到最大值或最小值，反转方向
				if (m_scaleFactor >= 2.0f) {
					m_scaleDirection = -1;  // 反向缩小
				}
				else if (m_scaleFactor <= 0.4f) {
					m_scaleDirection = 1;   // 反向放大
				}
			}

			// 处理旋转
			if (m_rotateInProgress) {
				// 计算新的旋转角度
				m_rotateAngle += m_rotateDirection * 5;  // 每次旋转5度
				for (auto& it : draw_system.elements) {
					it->rotate(m_rotateAngle, CPoint(0, 0));  // 对元素应用旋转
				}

				// 如果旋转角度达到360度，反转方向
				if (m_rotateAngle >= 360) {
					m_rotateAngle = 0;  // 重置角度
				}
			}
		}

		// 重绘视图，更新位置
		Invalidate();
	}
}

void COpenglView::OnTranslate()
{
	// TODO: 在此添加命令处理程序代码

	// 设置定时器，每100毫秒触发一次
	m_timerID = SetTimer(TIMER_ID, 100, nullptr);  // 100ms 的定时器
	m_moveDirection = 1;  // 向右平移
	m_moveDistance = 0;   // 初始平移距离为0
	m_moveInProgress = true; // 启动平移操作
	m_scaleInProgress = false; // 停止缩放操作
	m_rotateInProgress = false; // 停止旋转操作

}


void COpenglView::OnRotate()
{
	// TODO: 在此添加命令处理程序代码
	// 设置定时器，每100毫秒触发一次
	m_timerID = SetTimer(TIMER_ID, 100, nullptr);  // 100ms 的定时器
	m_rotateAngle = 0;  // 初始旋转角度为0
	m_rotateDirection = 1;  // 顺时针旋转
	m_rotateInProgress = true; // 启动旋转操作
	m_moveInProgress = false; // 停止平移操作
	m_scaleInProgress = false; // 停止缩放操作
}


void COpenglView::OnScale()
{
	// TODO: 在此添加命令处理程序代码

	// 设置定时器，每100毫秒触发一次
	m_timerID = SetTimer(TIMER_ID, 100, nullptr);  // 100ms 的定时器
	m_scaleFactor = 1.0f;  // 初始缩放因子为1
	m_scaleDirection = 1;  // 设定为放大
	m_scaleInProgress = true;  // 启动缩放操作
	m_moveInProgress = false; // 停止平移操作
	m_rotateInProgress = false; // 停止旋转操作
}


void COpenglView::OnStopTimer()
{
	// TODO: 在此添加命令处理程序代码
	KillTimer(m_timerID);  // 停止定时器
}


void COpenglView::OnPolygon()
{
	// TODO: 在此添加命令处理程序代码
	current_type = PolygonSqure;
	Invalidate();  // 触发重绘
	mouse_points.clear();

}


void COpenglView::OnClear()
{
	clearing = true;
	item = nullptr;
	mouse_points.clear();
	Invalidate();  // 触发重绘
}


void COpenglView::OnFill()
{
	
}


void COpenglView::OnMenuFill()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
	for (auto it : draw_system.elements)
	{
		it->fillmode = true;
	}

	Invalidate();  // 触发重绘
}


void COpenglView::OnRect()
{
	// TODO: 在此添加命令处理程序代码
	current_type = ElementType::RectangleSqure;
	Invalidate();  // 触发重绘
	mouse_points.clear();
}


void COpenglView::OnClip()
{
	// TODO: 在此添加命令处理程序代码

	Element* dst_elem = nullptr;
	for (auto it : draw_system.elements)
	{
		if (auto polygon = dynamic_cast<RectangleElement*>(it)) {
			dst_elem = polygon;
			break;
		}
	}

	if (!dst_elem)
	{
		// 在某个函数中弹出消息框
		AfxMessageBox(_T("你还未画出一个裁剪区域矩形框"), MB_OK | MB_ICONINFORMATION);

		return;
	}

	for (auto it : draw_system.elements)
	{
		it->fillmode = false;
	}

	bool cliped = false;
	for (auto it : draw_system.elements)
	{
		if (auto polygon = dynamic_cast<PolygonElement*>(it)) {
			PolygonElement* clip = new PolygonElement(polygon->clipPolygon(dst_elem));
			cliped = true;

			COLORREF redColor = RGB(255, 0, 0);


			clip->color = redColor;
			clip->fillmode = true;
			draw_system.elements.push_back(clip);
			break;
		}
	}

	if (!cliped)
	{
		AfxMessageBox(_T("多边形裁剪需要多边形和矩形元素，你还未画出一个待裁剪多边形"), MB_OK | MB_ICONINFORMATION);
	}

	Invalidate();  // 触发重绘
}


void COpenglView::OnOpenglRender()
{
	// TODO: 在此添加命令处理程序代码
	current_type = ElementType::OpenglRender;
	// 设置定时器，每100毫秒触发一次
	m_timerID = SetTimer(TIMER_ID, 100, nullptr);  // 100ms 的定时器
}

void COpenglView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_renderer.InitOpenGL(m_hWnd);  // 初始化 OpenGL 渲染
}


void COpenglView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_renderer.ProcessKeyboardInput(nChar);

	// 重绘视图
	Invalidate(); // 触发视图更新
}

void COpenglView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果需要停止移动或者其他操作，可以在这里处理
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL COpenglView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (current_type == OpenglRender)
	{
		// 例如，定义缩放比例
		const double zoomFactor = 1.1; // 每次滚动放大或缩小10%

		// 根据滚轮的方向（zDelta），调整缩放比例
		if (zDelta > 0)
		{
			// 放大

			m_renderer.m_scaling[0] *= zoomFactor;
			m_renderer.m_scaling[1] = m_renderer.m_scaling[0];
			m_renderer.m_scaling[2] = m_renderer.m_scaling[0];

		}
		else
		{
			// 缩小
			m_renderer.m_scaling[0] /= zoomFactor;
			m_renderer.m_scaling[1] = m_renderer.m_scaling[0];
			m_renderer.m_scaling[2] = m_renderer.m_scaling[0];
		}

		// 触发重绘
		Invalidate(); // 请求重绘视图
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
