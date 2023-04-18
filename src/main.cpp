#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "functions.h"
#include "parser.h"


using namespace cv;
using namespace dnn;

#ifndef LOG
#define LOG(x) std::cout << x << std::endl
#endif



int main() {
    Vars v("settings.txt");

    auto port = v.getVal<std::string>("port");
    auto capSource = v.getVal<std::string>("capSource");
    auto dnnModelFile = v.getVal<std::string>("dnnModelFile");
    auto dnnConfigFile = v.getVal<std::string>("dnnConfigFile");
    auto windowName = v.getVal<std::string>("windowName");
    auto dnnSwapRB = v.getVal<bool>("dnnSwapRB");
    auto dnnCrop = v.getVal<bool>("dnnCrop");
    auto showWindow = v.getVal<bool>("showWindow");
    auto confidenceThreshold = v.getVal<float>("confidenceThreshold");
    auto baudRate = v.getVal<int>("baudRate");
    auto aimRadius = v.getVal<int>("aimRadius");
    auto delay = v.getVal<int>("delay");
    auto dataFormat = v.getVal<std::string>("dataFormat");
    auto d1 = v.getValues<int>("dnnRes"); auto dnnRes = Size(d1[0], d1[1]);
    auto d2 = v.getValues<int>("dnnMeanVal"); auto dnnMeanVal = Scalar(d2[0], d2[1], d2[2]);
    auto d3 = v.getValues<float>("vecRect"); auto vecRect = Rect2f(d3[0], d3[1], d3[2], d3[3]);

    auto xc = v.getVal<float>("xc");
    auto yc = v.getVal<float>("yc");

//    auto d4 = v.getValues<std::string>("vs");
//    for (int i = 0; i < d4.size(); i++) {
//        LOG(d4[i]);
//    }



    HANDLE hSerial;
    hSerial = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) std::cerr << "Error opening serial port: " << port << std::endl;
    else LOG("Connection made: " << port);


    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) std::cerr << "Error getting serial port state" << std::endl;
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) std::cerr << "Error setting serial port state" << std::endl;


    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) std::cerr << "Error setting serial port timeouts" << std::endl;



    waitKey(1000);










    VideoCapture cap;
    if (capSource.size() == 1 && capSource[0]-'0'<=9) cap.open(capSource[0]-'0');
    else cap.open(capSource);


    Net net = readNetFromCaffe(dnnConfigFile, dnnModelFile);
    std::vector<Rect2f> faces;
    Mat img;


    cap.read(img);
    Size2f capSize = img.size();
    Point2f windowCenter(capSize / 2.f);


    dataFormat += "\n";
    char data[10];
    DWORD bytesWritten;
    while (true) {
        cap.read(img);

        Mat blob = blobFromImage(img, 1.0, dnnRes, dnnMeanVal, dnnSwapRB, dnnCrop);
        faces = detectFaces(net, blob, confidenceThreshold, capSize);


        Point2f aim = nearestFace(faces, windowCenter);
        Point2f limVec = limitVecRect(aim - windowCenter, vecRect);



        // Send data over the serial port


        if (vecLength(limVec) < aimRadius) limVec *= 0;
        sprintf(data, dataFormat.c_str(), xc * limVec.x, yc * limVec.y);

        if (!WriteFile(hSerial, data, sizeof(data), &bytesWritten, NULL)) {
            std::cerr << "Error writing to serial port" << std::endl;

        }

        LOG(data);




        if (showWindow) {
            for (Rect2f &face: faces) rectangle(img, face, Scalar(0, 255, 0), 2);
            line(img, windowCenter, limVec + windowCenter, Scalar(0, 0, 255), 2);
            circle(img, windowCenter, aimRadius, Scalar(255, 0, 0), 2);
            rectangle(img, Point(vecRect.tl() + windowCenter), Point(vecRect.br() + windowCenter), Scalar(255, 0, 0), 2);
            flip(img, img, 1);
            imshow(windowName, img);
        }

        int k = cv::waitKey(delay);
        if (k == 27) break;
        if (showWindow && cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) < 1) break;


    }

    // Close the serial port
    CloseHandle(hSerial);
    destroyAllWindows();

    return 0;

}
