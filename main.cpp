#include <iostream>
#include <fstream>
#include <qrencode.h>
#include <png.h>

void saveToPNG(const QRcode *qrcode, const char *filename, int scale) {
    int width = qrcode->width;
    int size = width * scale;

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        std::cerr << "Could not open file for writing: " << filename << std::endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Could not allocate write struct" << std::endl;
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Could not allocate info struct" << std::endl;
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error during png creation" << std::endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);

    png_set_IHDR(png, info, size, size, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep row = (png_bytep) malloc(size * sizeof(png_byte));
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < width; x++) {
            int value = qrcode->data[y * width + x] & 1 ? 0 : 255;
            for (int i = 0; i < scale; i++) {
                row[x * scale + i] = value;
            }
        }
        for (int i = 0; i < scale; i++) {
            png_write_row(png, row);
        }
    }

    png_write_end(png, nullptr);
    free(row);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <string_to_encode>" << std::endl;
        return 1;
    }

    const char *text = argv[1];
    QRcode *qrcode = QRcode_encodeString(text, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

    if (qrcode) {
        saveToPNG(qrcode, "qrcode.png", 10);
        QRcode_free(qrcode);
        std::cout << "QR code generated and saved to qrcode.png" << std::endl;
    } else {
        std::cerr << "Failed to generate QR code" << std::endl;
    }

    return 0;
}
