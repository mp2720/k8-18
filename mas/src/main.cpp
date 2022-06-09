#include <iostream>
#include <cstring>
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
        std::cout << "Invalid usage. Type " << argv[0] << " -h to get help.";
        return -1;
    }

    PassOne p1 = PassOne(Cursor::from_file_path(source_path));
    p1.exec();
    handle_errors();

    std::ofstream bin_file("mc.bin", std::ios::out | std::ios::binary),
            labels_file("labels.txt", std::ios::out);
    if (!bin_file) {
        Diagn::error(nullptr, "cannot open output 'mc.bin' file");
        return -1;
    } else if (!labels_file) {
        Diagn::error(nullptr, "cannot open 'labels.txt' file");
        return -1;
    }

    PassTwo p2 = PassTwo(p1.labels, p1.mic_instrs, 0, bin_file, labels_file);
    p2.exec();
    handle_errors();

    Diagn::report(std::cout); // Выдать предупреждения.

    return 0;
}
