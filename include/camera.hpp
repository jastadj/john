#ifndef CLASS_CAMERA
#define CLASS_CAMERA

#include "tools.hpp"

class Camera
{
private:

    unsigned int m_Width;
    unsigned int m_Height;

    vector2i m_ScreenPosition;
    vector2i m_WorldPosition;

public:
    Camera();
    ~Camera();

    void setWidth(unsigned int nwidth);
    void setHeight(unsigned int nheight);
    void setDimensions(unsigned int width, unsigned int nheight);
    void setScreenPosition(int xpos, int ypos);
    void setScreenPosition(vector2i npos);
    void setWorldPosition(int xpos, int ypos);
    void setWorldPosition(vector2i npos);

    unsigned int getWidth() { return m_Width;}
    unsigned int getHeight() { return m_Height;}
    vector2i getScreenPosition() { return m_ScreenPosition;}
    vector2i getWorldPosition() { return m_WorldPosition;}

    void setCenter(vector2i ncenter);
    void setCenter(int xpos, int ypos);

    bool PositionInView(vector2i tpos);
    vector2i PositionToScreen(vector2i tpos);

};
#endif // CLASS_CAMERA
