#include "camera.hpp"

Camera::Camera()
{
    m_Width = 5;
    m_Height = 5;

    m_ScreenPosition.x = 0;
    m_ScreenPosition.y = 0;

    m_WorldPosition.x = 0;
    m_WorldPosition.y = 0;
}

Camera::~Camera()
{

}

void Camera::setWidth(unsigned int nwidth)
{
    m_Width = nwidth;
}

void Camera::setHeight(unsigned int nheight)
{
    m_Height = nheight;
}

void Camera::setDimensions(unsigned int nwidth, unsigned int nheight)
{
    setWidth(nwidth);
    setHeight(nheight);
}

void Camera::setScreenPosition(int xpos, int ypos)
{
    m_ScreenPosition.x = xpos;
    m_ScreenPosition.y = ypos;
}

void Camera::setScreenPosition(vector2i npos)
{
    setScreenPosition(npos.x, npos.y);
}

void Camera::setWorldPosition(int xpos, int ypos)
{
    m_WorldPosition.x = xpos;
    m_WorldPosition.y = ypos;
}

void Camera::setWorldPosition(vector2i npos)
{
    setWorldPosition(npos.x, npos.y);
}

void Camera::setCenter(int xpos, int ypos)
{
    //setWorldPosition(xpos - (m_Width/2), ypos - (m_Height/2));
    setWorldPosition(xpos - (m_Width/2), ypos - (m_Height/2) );
}

void Camera::setCenter(vector2i npos)
{
    setCenter(npos.x, npos.y);
}

bool Camera::PositionInView(vector2i tpos)
{
    if(tpos.x < m_WorldPosition.x || tpos.x >= m_WorldPosition.x + int(m_Width) ||
       tpos.y < m_WorldPosition.y || tpos.y >= m_WorldPosition.y + int(m_Height) ) return false;

    return true;
}

vector2i Camera::PositionToScreen(vector2i tpos)
{
    tpos.x += m_ScreenPosition.x;
    tpos.y += m_ScreenPosition.y;

    tpos.x -= m_WorldPosition.x;
    tpos.y -= m_WorldPosition.y;

    return tpos;
}
