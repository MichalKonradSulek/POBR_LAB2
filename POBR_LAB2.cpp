// POBR_LAB1.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>


int calculateKernelSum(const cv::Mat_<int>& kernel) {
    int sum = 0;
    for (int row = 0; row < kernel.rows; ++row) {
        for (int col = 0; col < kernel.cols; ++col) {
            sum += kernel(row, col);
        }
    }
    return sum;
}

cv::Mat filter(const cv::Mat& original, const cv::Mat_<int>& kernel) {
    CV_Assert(original.channels() == 3);
    CV_Assert(kernel.cols == kernel.rows && kernel.rows % 2 != 0);
    const unsigned kernelHalfSize = kernel.rows / 2;
    const int kernelSum = calculateKernelSum(kernel);
    cv::Mat_<cv::Vec3b> _original = original;
    cv::Mat_<cv::Vec3b> _new = cv::Mat_<cv::Vec3b>(original.rows - 2*kernelHalfSize, original.cols - 2*kernelHalfSize);
    for (unsigned imageRow = kernelHalfSize; imageRow < original.rows - kernelHalfSize; ++imageRow) {
        for (unsigned imageCol = kernelHalfSize; imageCol < original.cols - kernelHalfSize; ++imageCol) {
            cv::Vec3i sum = cv::Vec3i(0, 0, 0);
            for (unsigned kernelRow = 0; kernelRow < kernel.rows; ++kernelRow) {
                for (unsigned kernelCol = 0; kernelCol < kernel.cols; ++kernelCol) {
                    for (unsigned color = 0; color < 3; ++color) {
                        sum[color] += kernel(kernelRow, kernelCol) *
                            _original(imageRow + kernelRow - kernelHalfSize, imageCol + kernelCol - kernelHalfSize)[color];
                    }
                }
            }
            if (kernelSum != 0) {
                for (unsigned color = 0; color < 3; ++color) {
                    sum[color] /= kernelSum;
                }
            }
            _new(imageRow - kernelHalfSize, imageCol - kernelHalfSize) = sum;
        }
    }
    return _new;
}

void task1(bool isSavingEnabled) {
    std::cout << "Task1\n";
    cv::Mat image = cv::imread("Lena.png");
    cv::Mat_<int> kernel = cv::Mat_<int>::ones(5, 5);
    cv::Mat newImage = filter(image, kernel);
    cv::imshow("original", image);
    cv::imshow("Task1-1", newImage);
    cv::Mat_<int> kernel2 = cv::Mat_<int>(5, 5);
    for (int& i : kernel2) {
        i = -1;
    }
    kernel2(2, 2) = 24;
    cv::Mat newImage2 = filter(image, kernel2);
    cv::imshow("Task1-2", newImage2);
    if (isSavingEnabled) {
        cv::imwrite("Task1-1.png", newImage);
        cv::imwrite("Task1-2.png", newImage2);
    }
}

void getArgumentsFromStdin(int& kernelSize, int& index) throw(std::invalid_argument) {
    std::cout << "Podaj rozmiar jadra:" << std::endl;
    std::string input;
    std::cin >> input;
    kernelSize = std::stoi(input);
    if (kernelSize < 0 || kernelSize % 2 == 0) {
        throw std::invalid_argument("niepoprawna wielkosc jadra");
    }
    std::cout << "Podaj indeks filtru:" << std::endl;
    std::cin >> input;
    index = std::stoi(input);
    if (index < 0 || index >= kernelSize * kernelSize) {
        throw std::invalid_argument("niepoprawny indeks");
    }
}

cv::Vec3b getPixelWithGivenRank(const cv::Mat_<cv::Vec3b>& area, const unsigned rank) {
    std::vector<std::pair<unsigned, unsigned>> arrayOfPixels;
    arrayOfPixels.reserve(((unsigned)area.rows) * area.cols);
    for (unsigned i = 0; i < area.rows; ++i) {
        for (unsigned j = 0; j < area.cols; ++j) {
            unsigned index = i * area.cols + j;
            unsigned luminosity = area(i, j)[0] + area(i, j)[1] + area(i, j)[2];
            arrayOfPixels.emplace_back(luminosity, index);
        }
    }
    std::sort(arrayOfPixels.begin(), arrayOfPixels.end());
    unsigned index = arrayOfPixels.at(rank).second;
    return area(index / 7, index % 7);
}

cv::Mat rankFilter(const cv::Mat& original, unsigned kernelSize, unsigned index) {
    CV_Assert(original.channels() == 3);
    CV_Assert(kernelSize % 2 == 1 && kernelSize > 0);
    CV_Assert(index < kernelSize* kernelSize);
    const unsigned kernelHalfSize = kernelSize / 2;
    cv::Mat_<cv::Vec3b> _new = cv::Mat_<cv::Vec3b>(original.rows - 2 * kernelHalfSize, original.cols - 2 * kernelHalfSize);
    for (unsigned imageRow = kernelHalfSize; imageRow < original.rows - kernelHalfSize; ++imageRow) {
        for (unsigned imageCol = kernelHalfSize; imageCol < original.cols - kernelHalfSize; ++imageCol) {
            cv::Mat_<cv::Vec3b> kernel(original,
                cv::Range(imageRow - kernelHalfSize, imageRow + kernelHalfSize + 1),
                cv::Range(imageCol - kernelHalfSize, imageCol + kernelHalfSize + 1)
            );
            _new(imageRow - kernelHalfSize, imageCol - kernelHalfSize) = getPixelWithGivenRank(kernel, index);
        }
    }
    return _new;
}

void task2(bool isSavingEnabled) {
    std::cout << "Task1\n";
    int kernelSize, index;
    try {
        getArgumentsFromStdin(kernelSize, index);
    }
    catch (std::invalid_argument& exception) {
        std::cerr << "invalid argument: " << exception.what() << std::endl;
        return;
    }
    cv::Mat image = cv::imread("Lena.png");
    std::cout << "obliczanie" << std::endl;
    cv::Mat newImage = rankFilter(image, kernelSize, index);
    cv::imshow("original", image);
    cv::imshow("Task2", newImage);
    if (isSavingEnabled) {
        cv::imwrite("Task2.png", newImage);
    }
    
}


int main()
{
    task1(true);
    task2(true);

    cv::waitKey(-1);
    return 0;
}
