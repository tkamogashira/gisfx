#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <GL/glut.h>

#define SIZE_OF_LINE 4096*16

typedef unsigned char uchar;

typedef struct{
  double longitude; //∑–≈Ÿ
  double latitude; //∞ﬁ≈Ÿ
} Point;

typedef struct _Element Element;
struct _Element{
  std::string name;
  std::vector<_Element> attribute;
  std::vector<_Element> value;
  Point * point;
};

#endif
