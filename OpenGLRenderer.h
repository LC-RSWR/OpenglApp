#pragma once
// OpenGLRenderer.h

#pragma once
#include <GL/gl.h>   // OpenGL的核心库
#include <GL/glu.h>  // OpenGL实用库，提供了更多高层次的功能
#include <windows.h> // Windows API头文件，允许OpenGL与Windows系统进行交互

class OpenGLRenderer
{
public:
    // 构造函数：初始化OpenGL渲染器
    OpenGLRenderer();

    // 析构函数：清理OpenGL资源
    ~OpenGLRenderer();

    int width;  // 渲染窗口的宽度
    int height; // 渲染窗口的高度

    // 初始化OpenGL环境，设置窗口句柄
    void InitOpenGL(HWND hWnd);

    // 清理OpenGL相关资源
    void CleanupOpenGL();

    // 调整OpenGL视口大小
    void ResizeOpenGL(int width, int height);

    // 渲染场景
    void RenderScene();

    // 设置相机视角或视图矩阵
    void setView();

    // 切换线框模式（绘制立方体时使用线框或填充）
    void ToggleWireframe(bool wireframe);

    // 设置立方体的旋转角度
    void SetRotation(float xAngle, float yAngle, float zAngle);

    // 设置立方体的平移
    void SetTranslation(float x, float y, float z);

    // 设置立方体的缩放
    void SetScaling(float scaleX, float scaleY, float scaleZ);

    // 处理键盘输入（例如，改变视角、控制物体）
    void ProcessKeyboardInput(UINT nChar);


    // 物体的旋转角度（x、y、z轴）
    GLfloat m_rotation[3];
    // 物体的缩放（x、y、z）
    GLfloat m_scaling[3];
private:
    // OpenGL的设备上下文（HDC）
    HDC m_hDC;

    // OpenGL的渲染上下文（HGLRC）
    HGLRC m_hRC;

    // 是否启用线框模式
    bool m_wireframe;

   

    // 物体的平移（x、y、z）
    GLfloat m_translation[3];

    // 绘制一个标准的立方体
    void DrawCube(bool texture);

    // 绘制一个线框立方体
    void DrawWireframeCube();

    // 绘制一个带透明度的立方体
    void DrawCubeWithTransparency();

    // 设置场景中的光照
    void SetupLighting();

    // 设置纹理（通过纹理文件路径）
    void SetupTexture(const char* textureFilePath);

    // 使用光照进行渲染
    void RenderWithLighting();

    // 绘制阴影
    void RenderShadow();

private:
    // 相机的位置和方向
    float cameraPosX = 0.0f, cameraPosY = 1.0f, cameraPosZ = 20.0f;  // 相机的位置，初始设置在 (0, 1, 5)
    float cameraFrontX = 0.0f, cameraFrontY = 0.0f, cameraFrontZ = -1.0f;  // 相机的前方方向，指向 Z 轴负方向
    float cameraUpX = 0.0f, cameraUpY = 1.0f, cameraUpZ = 0.0f;  // 相机的上方方向，指向 Y 轴正方向

    // 相机控制的速度，用于控制相机的平移和旋转速度
    float moveSpeed = 0.1f;  // 控制相机移动的速度
    float rotateSpeed = 1.0f; // 控制相机旋转的速度

    // 鼠标控制的角度（可选）
    float yaw = -90.0f, pitch = 0.0f;  // 用于计算相机的旋转角度，yaw 控制水平旋转，pitch 控制垂直旋转
};
