#include <opencv2/opencv.hpp>


#ifndef CVAIM_FUNCTIONS_H
#define CVAIM_FUNCTIONS_H

#define LOG(x) std::cout << x << std::endl



using namespace cv;
using namespace dnn;


template<class T>
Point_<T> nearestFace(std::vector<Rect_<T>> &faces, Point_<T>& center);

template<class T>
std::vector<Rect_<T>> detectFaces(Net &net, Mat &blob, float confidenceThreshold, Size_<T> &scale);

template<class T>
Point_<T> rectCenter(Rect_<T> &rect) {
    return (rect.br() + rect.tl()) / 2.f;
}

template<class T>
T vecLength(Point_<T> &v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

template<class T>
Point_<T> limitVec(Point_<T> v, T maxAimVecLen) {
    float vLen = vecLength(v);
    if (vLen > maxAimVecLen) v = maxAimVecLen * v / vLen;
    return v;
}


#endif //CVAIM_FUNCTIONS_H
