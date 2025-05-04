#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// Функция для размытия лица на изображении
void blurFace(Mat& image, Rect face) {
    // Получаем ROI (Region of Interest) лица
    Mat faceROI = image(face);

    // Размываем ROI с помощью GaussianBlur
    GaussianBlur(faceROI, faceROI, Size(101, 101), 0); // Size(51, 51) - размер ядра размытия, можно менять

    // Заменяем ROI в исходном изображении размытым ROI
    faceROI.copyTo(image(face));
}

int main() {
    setlocale(LC_ALL, "Russian");

    

    // Объявляем переменную для хранения имени файла изображения
    string image_filename;

    // Запрашиваем имя файла у пользователя
    cout << "Введите имя файла изображения: ";
    cin >> image_filename;

    // Загружаем изображение
    Mat image = imread(image_filename);

    // Проверяем, удалось ли загрузить изображение
    if (image.empty()) {
        cout << "Не удалось загрузить изображение: " << image_filename << endl;
        return -1;
    }

    // Загружаем классификатор каскадов Хаара для обнаружения лиц
    CascadeClassifier face_cascade;
    if (!face_cascade.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
        return -1;
    }

    // Обнаруживаем лица на изображении
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    // Размываем каждое обнаруженное лицо
    for (size_t i = 0; i < faces.size(); i++) {
        blurFace(image, faces[i]);
    }

    // Создаем имя для выходного файла
    string output_filename = "blurred_" + image_filename;

    // Сохраняем получившееся изображение
    imwrite(output_filename, image);

    cout << "Изображение с размытыми лицами сохранено как: " << output_filename << endl;

    return 0;
}
