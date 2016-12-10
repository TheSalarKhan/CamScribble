#include <iostream>

class Point {
public:
  int x,y;
  Point() {}
  Point(int x, int y) {
    this->x = x;
    this->y = y;
  }
};

class PerspectiveCorrectionParams {
public:
  Point tl,tr,bl,br;
  int width,height;
};

class PerspectiveCorrection {
public:
  PerspectiveCorrection() {
    params.tl =
      params.tr =
      params.bl =
      params.br = Point(0,0);


    std::cout << params.tl.x << params.tl.y;

  }

private:
  PerspectiveCorrectionParams params;
};


int main() {
  PerspectiveCorrection p;
  return 0;
}
