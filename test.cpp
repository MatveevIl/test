#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void blurFace(Mat& image, Rect face) { //Mat - объект(матрица), представляющий изображение; объект Rect - прямоугольник лица
    Mat faceROI = image(face); //создаёт объект, типа Mat, являющийся регионом интереса
    GaussianBlur(faceROI, faceROI, Size(101, 101), 0); //фэйсРои - лицо входящее, фэйсРОИ - лицо выходящее; размер ядра Гаусса и отклонение по Х и У - 0(стандартное)
    faceROI.copyTo(image(face)); //копируем размытое на исходное изображение
}

int main() {
    setlocale(LC_ALL, "Russian");
    string image_filename;

    cout << "Введите имя файла изображения: ";
    cin >> image_filename;

    Mat image = imread(image_filename); // создаём объект имаге типа Мат, имрид - функция опенсв для чтения изображений 

    if (image.empty()) {
        cout << "Не удалось загрузить изображение: " << image_filename << endl;
        return -1;
    }

    CascadeClassifier face_cascade; //объявляем объект стандартного для опенСВ класса CascadeClassifier для обнаружения объектов
    if (!face_cascade.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
        return -1;
    }

    std::vector<Rect> faces; //объявляем faces = вектор(массив, изменяющийся динамически)  для хранения прямоугольников = Rect
    face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30)); 
    /*функция для обнаружения лиц(входное изображение; вектор, куда сохраняем найденные лица;
    1.1 - параметр масштабирования, чем меньше - тем точнее, но дольше работает; 
    2 - параметр, определяющий, сколько прямоугольников дб рядом, чтоб кандидат стал лицом, чем больше значение - тем меньше ложных срабатываний, но можно пропустить лица
    0 - других флагов нет
    CASCADE_SCALE_IMAGE - размер изображения масштабируем во время поиска
    Сайз - минимальный размер лица для определения*/

    for (size_t i = 0; i < faces.size(); i++) {
        blurFace(image, faces[i]);
    }//Для каждого лица вызываем функцию размывания лиц

    string output_filename = "blurred_" + image_filename;

    imwrite(output_filename, image);

    cout << "Изображение с размытыми лицами сохранено как: " << output_filename << endl;

    return 0;
}
