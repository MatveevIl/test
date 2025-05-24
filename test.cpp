#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <mutex>


using namespace cv;
using namespace std;
namespace fs = std::filesystem;

std::mutex blurFace_mutex;



void blurFace(Mat& image, Rect face) { //Mat - объект(матрица), представляющий изображение; объект Rect - прямоугольник лица

    std::lock_guard<std::mutex>lock(blurFace_mutex);

    Mat faceROI = image(face); //создаёт объект, типа Mat, являющийся регионом интереса
    GaussianBlur(faceROI, faceROI, Size(101, 101), 0); //фэйсРои - лицо входящее, фэйсРОИ - лицо выходящее; размер ядра Гаусса и отклонение по Х и У - 0(стандартное)
    faceROI.copyTo(image(face)); //копируем размытое на исходное изображение
}

void image_handler(fs::path file_path ) {

    CascadeClassifier face_cascade; //объявляем объект стандартного для опенСВ класса CascadeClassifier для обнаружения объектов
    CascadeClassifier face_cascade2;
    CascadeClassifier face_cascade3;

    if (!face_cascade.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
        //return -1;
    }

    if (!face_cascade2.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
        //return -1;
    }

    if (!face_cascade3.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
        //return -1;
    }

    std::cout << "Начат поток: " << std::this_thread::get_id() << "  Для: "<<file_path.filename().string()<< std::endl;

   
    Mat image = imread(file_path.string()); // Используем полный путь к файлу

    // Проверяем, удалось ли загрузить изображение
    if (image.empty()) {
        std::cerr << "Не удалось загрузить изображение: " << file_path.string() << std::endl;
    }

    Mat greyImage;
    cvtColor(image, greyImage, COLOR_BGR2GRAY);
    std::vector<Rect> faces; //объявляем faces = вектор(массив, изменяющийся динамически)  для хранения прямоугольников = Rect
    face_cascade.detectMultiScale(greyImage, faces, 1.05, 6, 0, Size(35, 40));
    /*функция для обнаружения лиц(входное изображение; вектор, куда сохраняем найденные лица;
    первый параметр в диапазоне 1,05-1,4 - параметр масштабирования, чем меньше - тем точнее, но дольше работает;
    второй параметр 2-6 - параметр, определяющий, сколько прямоугольников дб рядом, чтоб кандидат стал лицом, чем больше значение - тем меньше ложных срабатываний, но можно пропустить лица
    0 - других флагов нет
    CASCADE_SCALE_IMAGE - размер изображения масштабируем во время поиска
    Сайз - минимальный размер лица для определения*/

    std::vector<Rect> faces2;
    face_cascade2.detectMultiScale(greyImage, faces2, 1.05, 5, 0, Size(30, 30));
    std::vector<Rect> faces3;
    face_cascade3.detectMultiScale(greyImage, faces3, 1.04, 4, 0, Size(25, 25));
    //std::vector<Rect> faces4;
    //face_cascade4.detectMultiScale(greyImage, faces4, 1.05, 3, 0, Size(25, 25));
    /*for (size_t i = 0; i < faces4.size(); i++) {
        rectangle(image, faces4[i], Scalar(255, 0, 0), 2);
        blurFace(image, faces4[i]);
    }*/ //это тестовый отрывок для проверки одного каскада 
    std::vector<Rect> final_face;

    for (size_t i = 0; i < faces.size(); i++) {
        for (size_t j = 0; j < faces2.size(); j++) {
            for (size_t y = 0; y < faces3.size(); y++) {

                Rect intersection = faces[i] & faces2[j] & faces3[y];
                if (intersection.area() > 0) {
                    final_face.push_back(faces[i]);
                    break;
                }
            }
        }
    }
    for (size_t i = 0; i < final_face.size(); i++) {
        rectangle(image, final_face[i], Scalar(255, 0, 0), 2);
        blurFace(image, final_face[i]);
    }

    // Создаем имя для выходного файла
    // Используем имя исходного файла для создания имени выходного файла
    std::string output_filename = "blurred_" + file_path.filename().string();
    // Получаем путь к папке, где находится исходный файл
    std::string output_path = file_path.parent_path().string() + "/" + output_filename;

    // Сохраняем получившееся изображение
    imwrite(output_path, image);

    std::cout << "Изображение с размытыми лицами сохранено как: " << output_path << std::endl;
    image.release();
    greyImage.release();
    
    std::cout << "Завершён поток: " << std::this_thread::get_id() << std::endl;

}

int main() {
    setlocale(LC_ALL, "Russian");

    
    /*CascadeClassifier face_cascade4;
    if (!face_cascade4.load("D:\\openCV\\opencv\\sources\\data\\haarcascades\\haarcascade_profileface.xml")) {
        cout << "Не удалось загрузить классификатор каскадов Хаара" << endl;
    return -1;
    }*/

    std::vector<std::thread> threads;

    std::string folder_path;
    std::cout << "Введите путь к папке: ";
    std::cin >> folder_path;

    fs::path path_obj(folder_path);  // Преобразуем строку в объект пути

    if (!fs::exists(path_obj) || !fs::is_directory(path_obj)) {
        std::cerr << "Ошибка: Указанная папка не существует или не является папкой." << std::endl;
        return 1;
    }

    std::vector<fs::path> files; // Храним пути к файлам

    try {
        for (const auto& entry : fs::directory_iterator(path_obj)) {
            if (fs::is_regular_file(entry)) {
                files.push_back(entry.path()); // Добавляем путь к файлу в вектор
            }
        }

        //std::cout << "Файлы в папке:" << std::endl;
        for (const auto& file_path : files) {
            std::cout << file_path.filename().string() << std::endl; // Выводим только имя файла
            threads.emplace_back(image_handler, file_path);

            //image_handler(file_path);

        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::cout << "Все потоки завершены." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Произошла ошибка при обработке файлов: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

