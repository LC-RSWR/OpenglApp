#include "pch.h"  // 确保包含预编译头
#include "Draw.h"


Draw::Draw()
{
}

Draw::~Draw()
{
}

void Draw::draw(CDC* pDC)
{
    for (auto it : elements)
    {
        it->draw(pDC);
    }
}

