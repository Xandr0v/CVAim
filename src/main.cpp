#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "functions.h"
#include "getJson.h"



using namespace cv;
using namespace dnn;
using namespace nlohmann;





int main() {

    json j = getJson("settings.txt");

    auto port = j["port"].get<std::string>();
    auto capSource = j["capSource"].get<std::string>();
    auto dnnModelFile = j["dnnModelFile"].get<std::string>();
    auto dnnConfigFile = j["dnnConfigFile"].get<std::string>();
    auto dnnSwapRB = j["dnnSwapRB"].get<bool>();
    auto dnnCrop = j["dnnCrop"].get<bool>();
    auto windowName = j["windowName"].get<std::string>();
    auto showWindow = j["showWindow"].get<bool>();
    auto confidenceThreshold = j["confidenceThreshold"].get<float>();
    auto baudRate = j["baudRate"].get<int>();

    auto dnnRes = Size(
        j["/dnnRes/0"_json_pointer].get<int>(),
        j["/dnnRes/1"_json_pointer].get<int>()
        );
    auto dnnMeanVal = Scalar(
        j["/dnnMeanVal/0"_json_pointer].get<int>(),
        j["/dnnMeanVal/1"_json_pointer].get<int>(),
        j["/dnnMeanVal/2"_json_pointer].get<int>()
        );

    auto vecRect = Rect2f(
        j["/vecRect/0"_json_pointer].get<float>(),
        j["/vecRect/1"_json_pointer].get<float>(),
        j["/vecRect/2"_json_pointer].get<float>(),
        j["/vecRect/3"_json_pointer].get<float>()
        );







    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };

    // Open the serial port
    hSerial = CreateFile(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port" << std::endl;
        //return 1;
    }

    // Set the serial port parameters
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error getting serial port state" << std::endl;
        //return 1;
    }
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error setting serial port state" << std::endl;
        //return 1;
    }

    // Set the serial port timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error setting serial port timeouts" << std::endl;
        //return 1;
    }









    VideoCapture cap;
    if (capSource.size() == 1 && capSource[0]-'0'<=9) cap.open(capSource[0]-'0');
    else cap.open(capSource);


    Net net = readNetFromCaffe(dnnConfigFile, dnnModelFile);
    std::vector<Rect2f> faces;
    Mat img;


    cap.read(img);
    Size2f capSize = img.size();
    Point2f windowCenter(capSize / 2.f);


    char data[255];
    DWORD bytesWritten;
    while (true) {
        cap.read(img);

        Mat blob = blobFromImage(img, 1.0, dnnRes, dnnMeanVal, dnnSwapRB, dnnCrop);
        faces = detectFaces(net, blob, confidenceThreshold, capSize);


        Point2f aim = nearestFace(faces, windowCenter);
        Point2f limVec = limitVecRect(aim - windowCenter, vecRect);



        // Send data over the serial port
        sprintf(data, "%04.f %04.f", limVec.x, limVec.y);
        if (!WriteFile(hSerial, data, sizeof(data), &bytesWritten, NULL)) {
            //std::cerr << "Error writing to serial port" << std::endl;
            //return 1;
        }
        LOG(data);





        if (showWindow) {
            for (Rect2f &face: faces) rectangle(img, face, Scalar(0, 255, 0), 4);
            line(img, Point(320, 240), limVec + windowCenter, Scalar(0, 0, 255), 6, LINE_4);
            flip(img, img, 1);
            imshow(windowName, img);
        }

        int k = cv::waitKey(1);
        if (k == 27) break;
        if (showWindow && cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) < 1) break;
    }

    // Close the serial port
    CloseHandle(hSerial);
    destroyAllWindows();

    return 0;

}
