#include "bmp_image.h"
#include "pack_defines.h"
#include <iostream>
#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    char letter_b;
    char letter_m;
    uint32_t header_data_size;
    uint32_t reserve_area;
    uint32_t offset_from_start;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t header_size_sec_prt;
    int32_t width;
    int32_t height;
    uint16_t planes_number;// = 1;
    uint16_t bit_per_pixel;// = 24;
    uint32_t compression_type;// = 0;
    uint32_t byte_in_data;
    int32_t hor_resol;// = 11811;
    int32_t vert_resol;// = 11811;
    int32_t using_color_number;// = 0;
    int32_t meaningful_color_number;// = 0x1000000;
    
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image){
     try {
        ofstream out(file, ios::binary);
        out.exceptions(std::ofstream::failbit);
        BitmapFileHeader file_header;
        BitmapInfoHeader info_header;
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        int stride = GetBMPStride(w);    
        file_header.letter_b = 'B';
        file_header.letter_m = 'M';
        file_header.offset_from_start = sizeof(file_header) + sizeof(info_header);
        file_header.reserve_area = {};
        file_header.header_data_size = sizeof(file_header) + sizeof(info_header) + stride* h;    
        info_header.header_size_sec_prt = sizeof(info_header);
        info_header.width = w;
        info_header.height = h;
        info_header.planes_number = 1;
        info_header.bit_per_pixel = 24;
        info_header.compression_type = 0;
        info_header.byte_in_data = stride* h;
        info_header.hor_resol = 11811;
        info_header.vert_resol = 11811;
        info_header.using_color_number = 0;
        info_header.meaningful_color_number = 0x1000000;    
        out.write(reinterpret_cast<char*>(&file_header),sizeof(file_header));
        out.write(reinterpret_cast<char*>(&info_header),sizeof(info_header));
        std::vector<char> buff(stride);
        for (int y = h-1; y >=0 ; --y) {
            const Color* line = image.GetLine(y);
            for (int x = 0; x < w; ++x) {
                buff[x * 3 + 2] = static_cast<char>(line[x].r);
                buff[x * 3 + 1] = static_cast<char>(line[x].g);
                buff[x * 3 + 0] = static_cast<char>(line[x].b);
            }
            if (w*3<stride){
                for (int x = w*3; x < stride; ++x){
                    buff[x] = static_cast<char>(std::byte{0});                
                }
            }
            out.write(buff.data(),stride);
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
    
};

// напишите эту функцию
Image LoadBMP(const Path& file){
    try {
        ifstream ifs(file, ios::binary);
        ifs.exceptions(std::ifstream::failbit);
        BitmapFileHeader file_header;
        BitmapInfoHeader info_header;
        ifs.read(reinterpret_cast<char*>(&file_header),sizeof(file_header));
        ifs.read(reinterpret_cast<char*>(&info_header),sizeof(info_header)); 
        
        
        
        if (info_header.header_size_sec_prt != sizeof(info_header)||
            info_header.byte_in_data!=GetBMPStride(info_header.width)*info_header.height){
                cout << "Ошибка считывания данных!" << endl;
                return   Image{};               
        }
        if ( file_header.letter_b != 'B'||
             file_header.letter_m != 'M'||
             file_header.offset_from_start != sizeof(file_header) + sizeof(info_header)||
             file_header.header_data_size != sizeof(file_header) + sizeof(info_header) + GetBMPStride(info_header.width)*info_header.height){
                cout << "Ошибка считывания данных!"<< endl;
                return   Image{}; 
        } 
        const int w = info_header.width;
        const int h = info_header.height;
        int stride = GetBMPStride(w);
        Image result(w, h, Color::Black());
        std::vector<char> buff(stride);
        for (int y = h-1; y >=0 ; --y) {
            Color* line = result.GetLine(y);
            ifs.read(buff.data(),stride);
            for (int x = 0; x < w; ++x) {
                line[x].b = static_cast<byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<byte>(buff[x * 3 + 1]);
                line[x].r = static_cast<byte>(buff[x * 3 + 2]);
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
}; 
    

}  // namespace img_lib