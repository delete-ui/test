#pragma execution_character_set("utf-8")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

class BmpViewer {
public:
    void openBMP(const std::string& fileName);
    void displayBMP();
    void closeBMP();

private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::vector<std::vector<int>> pixelData; // 0 - black, 1 - white
    int width;
    int height;

    char scalePixel(int x, int y, int scaleX, int scaleY);
    void adjustConsoleSize(); // Функция для изменения размера консоли
};

void BmpViewer::openBMP(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Unable to open file");
    }

    // Чтение заголовка BMP файла
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
        throw std::runtime_error("Only 24 and 32 bit BMP formats are supported");
    }

    width = infoHeader.biWidth;
    height = infoHeader.biHeight;

    pixelData.resize(height, std::vector<int>(width));

    int padding = (4 - (width * 3) % 4) % 4; // Вычисляем отступ

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char color[3]; // RGB
            file.read(reinterpret_cast<char*>(color), 3);

            // Читаем пиксели в формате RGB (обратите внимание на порядок)
            if (color[0] == 0 && color[1] == 0 && color[2] == 0) {
                pixelData[i][j] = 0; // черный
            }
            else if (color[0] == 255 && color[1] == 255 && color[2] == 255) {
                pixelData[i][j] = 1; // белый
            }
            else {
                throw std::runtime_error("Unsupported color found in image");
            }
        }
        file.ignore(padding); // Пропустить отступ
    }

    file.close();
}

char BmpViewer::scalePixel(int x, int y, int scaleX, int scaleY) {
    for (int i = 0; i < scaleY; ++i) {
        for (int j = 0; j < scaleX; ++j) {
            // Возвращаем символ в любом масштабе
            if (i == 0 && j == 0) {
                return pixelData[y][x] == 0 ? '█' : ' ';
            }
        }
    }
    return ' ';
}

void BmpViewer::adjustConsoleSize() {
    // Уменьшаем размеры, чтобы избежать искажения при отображении
    int displayWidth = width > 220 ? 220 : width;
    int displayHeight = height > 25 ? 25 : height;

    std::string command = "mode con: cols=" + std::to_string(displayWidth) + " lines=" + std::to_string(displayHeight);
    system(command.c_str());
}

void BmpViewer::displayBMP() {
    adjustConsoleSize(); // Настраиваем размер консоли

    for (int i = height - 1; i >= 0; i--) { // BMP сохраняет пиксели снизу вверх
        for (int j = 0; j < width; j++) {
            char displayChar = scalePixel(j, i, 1, 1); // Отображаем с масштабированием 1:1
            std::cout << displayChar;
        }
        std::cout << std::endl;
    }
}

void BmpViewer::closeBMP() {
    // Освобождение ресурсов, если нужно
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: drawBmp.exe <path_to_bmp_file>" << std::endl;
        return 1;
    }

    BmpViewer viewer;

    try {
        viewer.openBMP(argv[1]);
        viewer.displayBMP();
        viewer.closeBMP();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
        


    }

    return 0;
}