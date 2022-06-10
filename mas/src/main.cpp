#include <iostream>
#include <cstring>
#include <iomanip>
#include "../inc/pass1.h"
#include "../inc/pass2.h"

void handle_errors() {
    if (!Diagn::errors.empty()) {
        Diagn::report(std::cout);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    char *source_path;
    if (argc == 2) {
        if (!strcmp(argv[1], "-h")) {
            std::cout
                    << argv[0] << " - K8-18 microassembler\n"
                    << "Usage:\n"
                    << "\t" << argv[0] << " -h           print this help message\n"
                    << "\t" << argv[0] << " <source>     assemble file, write output to 'mc.bin' and 'labels.txt'\n";
            return 0;
        }

        source_path = argv[1];
    } else {
        std::cout << "Invalid usage. Type " << argv[0] << " -h to get help.\n";
        return -1;
    }

    try {
        auto *cursor = Cursor::from_file_path(source_path);
        if (cursor == nullptr) {
            std::cout << "Cannot open source file.\n";
            return -1;
        }

        PassOne p1 = PassOne(cursor);
        p1.exec();
        handle_errors();

        std::ofstream bin_file("mc.bin", std::ios::out | std::ios::binary),
                labels_file("info.txt", std::ios::out);
        if (!bin_file) {
            std::cout << "cannot open output 'mc.bin' file\n";
            return -1;
        } else if (!labels_file) {
            std::cout << "cannot open 'info.txt' file\n";
            return -1;
        }

        PassTwo p2 = PassTwo(p1.labels, p1.mic_instrs, 0, bin_file, labels_file);
        p2.exec();
        handle_errors();
        Diagn::report(std::cout); // Выдать предупреждения.

        std::cout << "generated " << p2.occupied << '/' << PassTwo::MC_SIZE << " microinstructions of microcode ("
                  << std::fixed << std::setprecision(1) << float(p2.occupied) / PassTwo::MC_SIZE * 100.0f
                  << "% occupied"
                  << ")" << std::endl;

        return 0;
    } catch (const std::exception &e) {
        std::cout << "FATAL ERROR " << e.what() << std::endl;
    }
}
