//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_VEC_H
#define COMMONVISUALIZER_VEC_H

namespace loco
{
  /**
   * length 4 vector (can be used as vec3)
   */
  union Vec
  {
    float _data[4];
    struct
    {
      float _x;
      float _y;
      float _z;
      float _w;
    };
    struct
    {
      float _r;
      float _g;
      float _b;
      float _a;
    };
  };

  struct Transform
  {
    Vec _translation;
    Vec _rotation;
  };

  struct LightSource
  {
    Vec _pos;
    Vec _color;
  };
}
#endif //COMMONVISUALIZER_VEC_H
