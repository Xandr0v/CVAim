
#include <opencv2/opencv.hpp>
#include <iostream>
#include "vendor/SerialPort/SerialPort.h"
#include "functions.h"
#include "vendor/json/json.hpp"
#include <fstream>


using namespace cv;
using namespace dnn;
using namespace nlohmann;

int main() {
    std::ifstream fin("settings.txt");
    if (!fin) {
        LOG("Cannot open settings.txt");
        return 1;
    }
    json j;
    if (json::accept(fin)) {
        fin.seekg(0);
        j = json::parse(fin);
        fin.close();
    }
    else {
        LOG("Incorrect settings");
        j = {
            {"port", "\\\\.\\COM3"},
            {"capSource", "0"},
            {"dnnModelFile", "res_ssd_300Dim.caffeModel"},
            {"dnnConfigFile", "weights.prototxt"},
            {"dnnRes", {300, 300}},
            {"dnnMeanVal", {104, 177, 123}},
            {"dnnSwapRB", false},
            {"dnnCrop", false},
            {"windowName", "CVFaceAim"},
            {"showWindow", true},
            {"confidenceThreshold", 0.3f},
            {"maxAimVecLen", 80.f},
            {"baudRate", 9600}
        };
        fin.close();
        std::ofstream fout("settings.txt");
        fout << std::setw(4) << j << std::endl;
        fout.close();
    }



    std::string port = j["port"].get<std::string>();
    std::string capSource = j["capSource"].get<std::string>();
    std::string dnnModelFile = j["dnnModelFile"].get<std::string>();
    std::string dnnConfigFile = j["dnnConfigFile"].get<std::string>();
    Size dnnRes = Size(
        j["/dnnRes/0"_json_pointer].get<int>(),
        j["/dnnRes/1"_json_pointer].get<int>()
        );
    Scalar dnnMeanVal = Scalar(
        j["/dnnMeanVal/0"_json_pointer].get<int>(),
        j["/dnnMeanVal/1"_json_pointer].get<int>(),
        j["/dnnMeanVal/2"_json_pointer].get<int>()
        );
    bool dnnSwapRB = j["dnnSwapRB"].get<bool>();
    bool dnnCrop = j["dnnCrop"].get<bool>();
    std::string windowName = j["windowName"].get<std::string>();
    bool showWindow = j["showWindow"].get<bool>();
    float confidenceThreshold = j["confidenceThreshold"].get<float>();
    float maxAimVecLen = j["maxAimVecLen"].get<float>();
    int baudRate = j["baudRate"].get<int>();


    SerialPort arduino((char*)port.c_str(), baudRate);
    if (arduino.isConnected()) LOG("Connection made");


    VideoCapture cap;
    if (capSource.size() == 1 && capSource[0]-'0'<=9) cap.open(capSource[0]-'0');
    else cap.open(capSource);

    Net net = readNetFromCaffe(dnnConfigFile, dnnModelFile);

    std::vector<Rect2f> faces;
    Mat img;


    cap.read(img);
    Size2f capSize = img.size();
    Point2f windowCenter(capSize / 2.f);

    while (true) {
        cap.read(img);

        Mat blob = blobFromImage(img, 1.0, dnnRes, dnnMeanVal, dnnSwapRB, dnnCrop);

        faces = detectFaces(net, blob, confidenceThreshold, capSize);

        Point2f aim = nearestFace(faces, windowCenter);
        Point2f limVec = limitVec(aim - windowCenter, maxAimVecLen);

        char data[MAX_DATA_LENGTH];
        sprintf(data, "%.2f %.2f", limVec.x, limVec.y);
        arduino.writeSerialPort(data, MAX_DATA_LENGTH);
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


    destroyAllWindows();

    return 0;

}
