#ifndef UI__RECTANGLE_H
#define UI__RECTANGLE_H

namespace tedomari {
namespace ui {

template<typename T>
struct Rectangle {
  Rectangle() : x(0), y(0), width(0), height(0) {}
  Rectangle(T xoffset, T yoffset, T w, T h) :
    x(xoffset), y(yoffset), width(w), height(h) {}
  T x;
  T y;
  T width;
  T height;

  inline T getRight() const { return x+width; }
  inline T getBottom() const { return y+height; }
};

}}

#endif // UI__RECTANGLE_H

