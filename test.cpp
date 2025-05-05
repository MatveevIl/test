#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void blurFace(Mat& image, Rect face) {
    Mat faceROI = image(face);
    GaussianBlur(faceROI, faceROI, Size(101, 101), 0); 
    faceROI.copyTo(image(face));
}

int main() {
    setlocale(LC_ALL, "Russian");
    string image_filename;

    cout << "Введите имя файла изображения: ";
    cin >> image_filename;

    Mat image = imread(image_filename);

    if (image.empty()) {
        cout << "Не удалось загрузить изображение: " << image_filename << endl;
        return -1;
    }

    CascadeClassifier face_cascade;
    if (!face_cascade.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
        return -1;
    }

    std::vector<Rect> faces;
    face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    for (size_t i = 0; i < faces.size(); i++) {
        blurFace(image, faces[i]);
    }

    string output_filename = "blurred_" + image_filename;

    imwrite(output_filename, image);

    cout << "Изображение с размытыми лицами сохранено как: " << output_filename << endl;

    return 0;
}
