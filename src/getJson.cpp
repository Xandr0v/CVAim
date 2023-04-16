#include "getJson.h"



json getJson(std::string fileName) {

    std::ifstream fin(fileName);
    if (!fin) {
        LOG("Cannot open" << fileName);

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
            {"port", "COM3"},
            {"capSource", "0"},
            {"dnnModelFile", "face_detection/res_ssd_300Dim.caffeModel"},
            {"dnnConfigFile", "face_detection/weights.prototxt"},
            {"dnnRes", {300, 300}},
            {"dnnMeanVal", {104, 177, 123}},
            {"dnnSwapRB", false},
            {"dnnCrop", false},
            {"windowName", "CVFaceAim"},
            {"showWindow", true},
            {"confidenceThreshold", 0.3f},
            {"baudRate", 9600},
            {"vecRect", {-100.f, -100.f, 200.f, 200.f}}
        };
        fin.close();
        std::ofstream fout(fileName);
        fout << std::setw(4) << j << std::endl;
        fout.close();
    }

    return j;

}