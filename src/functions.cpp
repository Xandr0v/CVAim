#include "functions.h"



using namespace cv;
using namespace dnn;



template<class T>
std::vector<Rect_<T>> detectFaces(Net &net, Mat &blob, float confidenceThreshold, Size_<T> &scale) {
    net.setInput(blob);
    Mat detection = net.forward();
    std::vector<Rect_<T>> faces;

    for (int id[4] = {0}; id[2] < detection.size[2]; id[2]++) {

        id[3] = 2;
        float confidence = detection.at<float>(id);

        if (confidence > confidenceThreshold) {
            Point_<T> c1, c2;
            id[3] = 3;
            c1.x = (detection.at<float>(id) * scale.width);
            id[3] = 4;
            c1.y = (detection.at<float>(id) * scale.height);
            id[3] = 5;
            c2.x = (detection.at<float>(id) * scale.width);
            id[3] = 6;
            c2.y = (detection.at<float>(id) * scale.height);
            Rect_<T> rect(c1, c2);
            faces.push_back(rect);
        }
    }
    return faces;
}



template<class T>
Point_<T> nearestFace(std::vector<Rect_<T>> &faces, Point_<T>& center) {
    Point_<T> aim = center;
    float maxTargetCoef = 0;
    for (Rect_<T> &face: faces) {

        Point_<T> faceCenter = rectCenter(face);
        Point_<T> faceVec = faceCenter - center;
        float targetCoef = face.area() / vecLength(faceVec);


        if (maxTargetCoef < targetCoef) {
            maxTargetCoef = targetCoef;
            aim = faceCenter;
        }
    }
    return aim;
}


template std::vector<Rect_<float>> detectFaces(Net &net, Mat &blob, float confidenceThreshold, Size_<float> &scale);
template Point_<float> nearestFace<float>(std::vector<Rect_<float>> &faces, Point_<float>& center);





