#include "ppm_image.h"
#include <iostream>
#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

static const string_view PPM_SIG = "P6"sv;
static const int PPM_MAX = 255;    

bool SavePPM(const Path& file, const Image& image) {
     try {
        ofstream out(file, ios::binary);
        out.exceptions(std::ofstream::failbit);
        out << PPM_SIG << '\n' << image.GetWidth() << ' ' << image.GetHeight() << '\n' << PPM_MAX << '\n';
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        std::vector<char> buff(w * 3);

        for (int y = 0; y < h; ++y) {
            const Color* line = image.GetLine(y);
            for (int x = 0; x < w; ++x) {
                buff[x * 3 + 0] = static_cast<char>(line[x].r);
                buff[x * 3 + 1] = static_cast<char>(line[x].g);
                buff[x * 3 + 2] = static_cast<char>(line[x].b);
            }
            out.write(buff.data(), w * 3);
        }

        return out.good();
     }
    catch (const std::ios_base::failure& e) {
        cout << "Ошибка запси в  файл: " << e.what() << endl;
        return  false;  
    }
    catch (const std::exception& e) {
        cerr << "Произошла непредвиденная ошибка: " << e.what() << endl;
        return  false; 
    } 
}

Image LoadPPM(const Path& file) {
     try {
        // открываем поток с флагом ios::binary
        // поскольку будем читать данные в двоичном формате
        ifstream ifs(file, ios::binary);
        ifs.exceptions(std::ifstream::failbit);
        std::string sign;
        int w, h, color_max;

        // читаем заголовок: он содержит формат, размеры изображения
        // и максимальное значение цвета
        ifs >> sign >> w >> h >> color_max;

        // мы поддерживаем изображения только формата P6
        // с максимальным значением цвета 255
        if (sign != PPM_SIG || color_max != PPM_MAX) {
            return {};
        }

        // пропускаем один байт - это конец строки
        const char next = ifs.get();
        if (next != '\n') {
            return {};
        }

        Image result(w, h, Color::Black());
        std::vector<char> buff(w * 3);

        for (int y = 0; y < h; ++y) {
            Color* line = result.GetLine(y);
            ifs.read(buff.data(), w * 3);
            for (int x = 0; x < w; ++x) {
                line[x].r = static_cast<byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<byte>(buff[x * 3 + 1]);
                line[x].b = static_cast<byte>(buff[x * 3 + 2]);
            }
        }

        return result;
     }
    catch (const std::ios_base::failure& e) {
        cout << "Ошибка чтения из файла: " << e.what() << endl;
        return   Image{};  
    }
    catch (const std::exception& e) {
        cerr << "Произошла непредвиденная ошибка: " << e.what() << endl;
        return   Image{};
    } 
    
}

}  // namespace img_lib