#include <math.h>

#ifndef MATH_H
#define MATH_H

namespace math {

    const float PI = 3.14159265;


    static float deg_to_rad(float theta)
    {
        return theta * PI / 180.f;
    }
    
    static float rad_to_deg(float rad)
    {
        return rad * 180.f / PI;
    }

    static float angle_from_origin(float x2, float y2) 
    {
        // this assumes x and y < 1 unit and in relation to 0,0 but it would be easy to fix it 
        // to pass in an alternate origin
        float delta_x = x2 - 0.f;
        float delta_y = y2 - 0.f;
        float rad = atan2(delta_y, delta_x);
        return rad_to_deg(rad);
    }

    static float get_rand()
    {
        return rand() / float(RAND_MAX);
    }
}

#endif