#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

using std::vector;

class Camera
{
public:
    Camera();
    void rayThrough( double x, double y, ray &r );

    void rayThroughAntialising(int x, int y, int range, vector<ray>& r, int buffer_width, int buffer_height);

    void setEye( const vec3f &eye );
    void setLook( double, double, double, double );
    void setLook( const vec3f &viewDir, const vec3f &upDir );
    void setFOV( double );
    void setAspectRatio( double );
    vec3f getLook() { return look; }

    double getAspectRatio() { return aspectRatio; }
private:
    mat3f m;                     // rotation matrix
    double normalizedHeight;    // dimensions of image place at unit dist from eye
    double aspectRatio;
    
    void update();              // using the above three values calculate look,u,v
    
    vec3f eye;                   // camera position
    vec3f look;                  // direction to look
    vec3f u,v;                   // some unit vectors in the width(u) and height(v)
};

#endif
