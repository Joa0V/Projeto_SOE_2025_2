#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <algorithm>

#include "YOLO11.hpp"

int main(int argc, char *argv[]){

    namespace fs = std::filesystem;
    
    const std::string modelPath = "../model/my_model.onnx";
    const std::string labelsPath = "../model/coco.test_names";
    std::string imagePath = "../input/test_image.jpg";
    std::vector<std::string> imageFiles;

    // Using arg as image path or directory
    if (argc > 1){
        imagePath = argv[1];
        if (fs::is_directory(imagePath)){
            // Collect all image files in the directory
            for (const auto &entry : fs::directory_iterator(imagePath)){
                if (entry.is_regular_file()){
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tiff" || ext == ".tif"){
                        imageFiles.push_back(fs::absolute(entry.path()).string());
                    }
                }
            }
            if (imageFiles.empty()){
                std::cerr << "No image files found in directory: " << imagePath << std::endl;
                return -1;
            }
        }
        else if (fs::is_regular_file(imagePath)){
            imageFiles.push_back(imagePath);
        }
        else{
            std::cerr << "Provided path is not a valid file or directory: " << imagePath << std::endl;
            return -1;
        }
    }
    else{
        std::cout << "Usage: " << argv[0] << " <image_path_or_folder>\n";
        std::cout << "No image path provided. Using default: " << imagePath << std::endl;
        imageFiles.push_back(imagePath);
    }

    YOLO11Detector detector(modelPath, labelsPath);

    for (const auto &imgPath : imageFiles){

        std::cout << "\nProcessing: " << imgPath << std::endl;

        cv::Mat image = cv::imread(imgPath);
        if (image.empty()){
            std::cerr << "Error: Could not open or find the image!\n";
            continue;
        }

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<Detection> results = detector.detect(image);
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start);
        std::cout << "Detection completed in: " << duration.count() << " ms" << std::endl;
        std::cout << "Number of detections found: " << results.size() << std::endl;

        for (size_t i = 0; i < results.size(); ++i){

            std::cout << "Detection " << i << ": Class=" << results[i].classId
                      << ", Confidence=" << results[i].conf
                      << ", Box=(" << results[i].box.x << "," << results[i].box.y
                      << "," << results[i].box.width << "," << results[i].box.height << ")" << std::endl;
        }

        detector.drawBoundingBox(image, results);

        cv::imwrite("../output/result.jpg", image);
    }
    return 0;
}
