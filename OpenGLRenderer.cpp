#include "pch.h"

// OpenGLRenderer.cpp

#include "OpenGLRenderer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <windows.h>
#include <iostream>
#include <vector>
// 在某个 .cpp 文件的顶部定义 STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "gdi32.lib")

#define M_PI 3.1415



GLuint textureID;

void OpenGLRenderer::SetupTexture(const char* textureFilePath)
{
    // 生成纹理
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 设置纹理的过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 设置纹理的环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 加载纹理图像
    int width, height, nrChannels;
    unsigned char* data = stbi_load(textureFilePath, &width, &height, &nrChannels, 0);
    if (data) {
        // 将纹理图像上传到 GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);  // 释放图像数据
}

OpenGLRenderer::OpenGLRenderer()
    : m_hDC(NULL), m_hRC(NULL), m_wireframe(false)
{
    m_rotation[0] = m_rotation[1] = m_rotation[2] = 0.0f;
    m_translation[0] = m_translation[1] = m_translation[2] = 0.0f;
    m_scaling[0] = m_scaling[1] = m_scaling[2] = 1.0f;


   
}

OpenGLRenderer::~OpenGLRenderer()
{
    CleanupOpenGL();
}

void OpenGLRenderer::InitOpenGL(HWND hWnd)
{
    m_hDC = GetDC(hWnd);
    if (!m_hDC)
    {
        MessageBox(NULL, L"无法获取设备上下文", L"错误", MB_OK);
        return;
    }

    // 设置像素格式
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;

    int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
    if (nPixelFormat == 0)
    {
        MessageBox(NULL, L"无法选择像素格式", L"错误", MB_OK);
        return;
    }

    if (!SetPixelFormat(m_hDC, nPixelFormat, &pfd))
    {
        MessageBox(NULL, L"无法设置像素格式", L"错误", MB_OK);
        return;
    }

    m_hRC = wglCreateContext(m_hDC);
    if (!m_hRC)
    {
        MessageBox(NULL, L"无法创建 OpenGL 渲染上下文", L"错误", MB_OK);
        return;
    }

    // 设置 OpenGL 上下文到当前线程
    if (!wglMakeCurrent(m_hDC, m_hRC))
    {
        MessageBox(NULL, L"无法激活 OpenGL 渲染上下文", L"错误", MB_OK);
        return;
    }

    glEnable(GL_DEPTH_TEST);  // 启用深度测试
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // 背景为黑色

    SetupLighting();
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << err << std::endl;
    }

    // 这里加载并设置纹理
    SetupTexture("texture.jpg");
}

void OpenGLRenderer::CleanupOpenGL()
{
    if (m_hRC)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
    }
    if (m_hDC)
    {
        ReleaseDC(NULL, m_hDC);
        m_hDC = NULL;
    }
}

void OpenGLRenderer::ResizeOpenGL(int width, int height)
{

    this->width = width;
    this->height = height;
    if (width <= 0 || height <= 0)
    {
        return;
    }

    setView();

}

void OpenGLRenderer::RenderScene()
{

    setView();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清空颜色和深度缓冲区
    glLoadIdentity();  // 载入单位矩阵
    // 在你的 OpenGL 初始化中启用颜色材质
    glEnable(GL_COLOR_MATERIAL);
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
        cameraPosX + cameraFrontX, cameraPosY + cameraFrontY, cameraPosZ + cameraFrontZ,
        cameraUpX, cameraUpY, cameraUpZ);

    glTranslatef(m_translation[0], m_translation[1], m_translation[2]);
    glRotatef(m_rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(m_rotation[2], 0.0f, 0.0f, 1.0f);
    glScalef(m_scaling[0], m_scaling[1], m_scaling[2]);

   
    // 绘制阴影
    //RenderShadow();  // 绘制立方体的阴影
     // 禁用光照
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    // 绘制立方体（正常渲染）
    glTranslatef(-5.0, 0, 0);
    DrawCube(false); // 绘制立方体

    glTranslatef(5, 0, 0);
    RenderWithLighting();

    glTranslatef(5, 0, 0);
    DrawCubeWithTransparency();

    glTranslatef(5.0, 0, 0);
    DrawWireframeCube();

    SwapBuffers(m_hDC);  // 交换缓冲区
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
}

void OpenGLRenderer::setView()
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
}

void OpenGLRenderer::ToggleWireframe(bool wireframe)
{
    m_wireframe = wireframe;
    if (m_wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void OpenGLRenderer::SetRotation(float xAngle, float yAngle, float zAngle)
{
    m_rotation[0] = xAngle;
    m_rotation[1] = yAngle;
    m_rotation[2] = zAngle;
}

void OpenGLRenderer::SetTranslation(float x, float y, float z)
{
    m_translation[0] = x;
    m_translation[1] = y;
    m_translation[2] = z;
}

void OpenGLRenderer::SetScaling(float scaleX, float scaleY, float scaleZ)
{
    m_scaling[0] = scaleX;
    m_scaling[1] = scaleY;
    m_scaling[2] = scaleZ;
}

void OpenGLRenderer::ProcessKeyboardInput(UINT nChar)
{
    // 控制相机的前后左右移动（WASD）
    if (nChar == 'W' || nChar == 'w') {
        cameraPosX += moveSpeed * cameraFrontX;
        cameraPosY += moveSpeed * cameraFrontY;
        cameraPosZ += moveSpeed * cameraFrontZ;
    }
    if (nChar == 'S' || nChar == 's') {
        cameraPosX -= moveSpeed * cameraFrontX;
        cameraPosY -= moveSpeed * cameraFrontY;
        cameraPosZ -= moveSpeed * cameraFrontZ;
    }
    if (nChar == 'A' || nChar == 'a') {
        // 计算相机的右向
        float rightX = cameraFrontY * cameraUpZ - cameraFrontZ * cameraUpY;
        float rightY = cameraFrontZ * cameraUpX - cameraFrontX * cameraUpZ;
        float rightZ = cameraFrontX * cameraUpY - cameraFrontY * cameraUpX;
        cameraPosX -= moveSpeed * rightX;
        cameraPosY -= moveSpeed * rightY;
        cameraPosZ -= moveSpeed * rightZ;
    }
    if (nChar == 'D' || nChar == 'd') {
        // 计算相机的右向
        float rightX = cameraFrontY * cameraUpZ - cameraFrontZ * cameraUpY;
        float rightY = cameraFrontZ * cameraUpX - cameraFrontX * cameraUpZ;
        float rightZ = cameraFrontX * cameraUpY - cameraFrontY * cameraUpX;
        cameraPosX += moveSpeed * rightX;
        cameraPosY += moveSpeed * rightY;
        cameraPosZ += moveSpeed * rightZ;
    }

    // 旋转控制（箭头键）
    if (nChar == VK_LEFT) {
        yaw -= rotateSpeed;
    }
    if (nChar == VK_RIGHT) {
        yaw += rotateSpeed;
    }
    if (nChar == VK_UP) {
        pitch += rotateSpeed;
        if (pitch > 89.0f) pitch = 89.0f;  // 限制俯仰角度
    }
    if (nChar == VK_DOWN) {
        pitch -= rotateSpeed;
        if (pitch < -89.0f) pitch = -89.0f;  // 限制俯仰角度
    }

    // 更新相机的前方向向量
    // 角度转换为弧度
    float radYaw = yaw * (M_PI / 180.0f);  // Yaw 转换为弧度
    float radPitch = pitch * (M_PI / 180.0f);  // Pitch 转换为弧度
    cameraFrontX = cos(radYaw) * cos(radPitch);
    cameraFrontY = sin(radPitch);
    cameraFrontZ = sin(radYaw) * cos(radPitch);
}

void OpenGLRenderer::SetupLighting()
{
    GLfloat lightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

// 绘制一个立方体
void OpenGLRenderer::DrawCube(bool texture)
{

    if (texture)
    {
        // 开启纹理映射
        glEnable(GL_TEXTURE_2D);
    }
    else
    {

    }
    glBindTexture(GL_TEXTURE_2D, textureID);  // 绑定纹理对象到当前纹理单元
   
    glBegin(GL_QUADS);

    // 前面 (设置颜色为红色)
    
    if (!texture)
    {
        glColor3f(1.0f, 0.0f, 0.0f);  // 红色
    }
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);

    // 后面 (设置颜色为绿色)
   
    if (!texture)
    {
        glColor3f(0.0f, 1.0f, 0.0f);  // 绿色
    }
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);

    // 顶面 (设置颜色为蓝色)
    glColor3f(0.0f, 0.0f, 1.0f);  // 蓝色
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);

    // 底面 (设置颜色为黄色)
    glColor3f(1.0f, 1.0f, 0.0f);  // 黄色
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);

    // 左面 (设置颜色为紫色)
    glColor3f(1.0f, 0.0f, 1.0f);  // 紫色
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);

    // 右面 (设置颜色为青色)
    glColor3f(0.0f, 1.0f, 1.0f);  // 青色
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);

}

void OpenGLRenderer::DrawCubeWithTransparency()
{
    // 启用混合模式，允许透明度效果
    glEnable(GL_BLEND);

    // 设置混合函数，源颜色的透明度（SRC_ALPHA）与目标颜色的透明度反转（ONE_MINUS_SRC_ALPHA）相结合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 开始绘制立方体的每个面
    glBegin(GL_QUADS);

    // 绘制前面
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // 半透明红色（alpha = 0.5）
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // 左下角
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // 右下角
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);   // 右上角
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // 左上角

    // 绘制后面
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f); // 半透明绿色（alpha = 0.5）
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // 左下角
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // 左上角
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);   // 右上角
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // 右下角

    // 绘制顶面
    glColor4f(0.0f, 0.0f, 1.0f, 0.5f); // 半透明蓝色（alpha = 0.5）
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // 左后角
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);   // 左前角
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);    // 右前角
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);   // 右后角

    // 绘制底面
    glColor4f(1.0f, 1.0f, 0.0f, 0.5f); // 半透明黄色（alpha = 0.5）
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // 左后角
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // 右后角
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);   // 右前角
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // 左前角

    // 绘制左面
    glColor4f(1.0f, 0.0f, 1.0f, 0.5f); // 半透明紫色（alpha = 0.5）
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // 左下角
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // 左前角
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);   // 左上角
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // 左后角

    // 绘制右面
    glColor4f(0.0f, 1.0f, 1.0f, 0.5f); // 半透明青色（alpha = 0.5）
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // 右下角
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);   // 右上角
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);    // 右前角
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);   // 右前角

    glEnd(); // 结束绘制

    // 禁用混合模式，防止后续渲染受到影响
    glDisable(GL_BLEND);
}


// 绘制线框立方体
void OpenGLRenderer::DrawWireframeCube()
{
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    glEnd();
}


// 真实感渲染（光照、纹理等）
void OpenGLRenderer::RenderWithLighting()
{
    // 渲染时开启光照
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 在这里可以做更多的光照、纹理、阴影等效果的渲染
  
    DrawCube(true); // 绘制立方体

    // 禁用光照
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}

void OpenGLRenderer::RenderShadow()
{
    // 1. 设置光源的相关参数
    GLfloat lightPos[] = { 0.0f, 5.0f, 0.0f, 1.0f };  // 设置光源位置（例如，置于立方体上方）

    // 2. 绘制投影矩阵（假设光源照射到地面）
    glPushMatrix();

    // 设置投影矩阵：根据光源和地面的法线计算阴影的投影
    GLfloat projectionMatrix[16] = {
        // 投影矩阵的计算，假设光源在上方，地面法线为 (0, 1, 0)
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 1.0f
    };

    glMultMatrixf(projectionMatrix); // 应用投影矩阵

    // 3. 绘制阴影（通常是一个扁平的矩形）
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // 阴影的颜色通常为黑色或深灰色
    DrawCube(false);  // 绘制物体的投影，通常为一个立方体的投影

    glPopMatrix();

    // 4. 恢复正常的渲染设置
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // 恢复为白色或其他颜色
}

