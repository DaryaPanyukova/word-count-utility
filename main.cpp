#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <cstring>

struct OptionFlags {
    bool lines, bytes, words, chars;
};

struct FileData {
    char* filename;
    size_t lines, bytes, words, chars;

    FileData(char* name) {
        filename = name;
        lines = bytes = words = chars = 0;
    }
};

OptionFlags flags;

inline bool isChar(char symb) {
    return (symb != ' ' && symb != '\t' && symb != '\n');
}

void ReadOption(char* term) {
    if (term[1] == '\0')
        throw std::invalid_argument("ERROR: invalid option name");
    if (term[1] == '-') {
        if (strcmp(term, "--lines") == 0) {
            flags.lines = true;
            return;
        } else if (strcmp(term, "--bytes") == 0) {
            flags.bytes = true;
            return;
        } else if (strcmp(term, "--words") == 0) {
            flags.words = true;
            return;
        } else if (strcmp(term, "--chars") == 0) {
            flags.chars = true;
            return;
        } else {
            throw std::invalid_argument("ERROR: invalid option name");
        }
    }

    for (size_t i = 1; term[i] != '\0'; ++i) {
        if (term[i] == 'l') {
            flags.lines = true;
        } else if (term[i] == 'c') {
            flags.bytes = true;
        } else if (term[i] == 'w') {
            flags.words = true;
        } else if (term[i] == 'm') {
            flags.chars = true;
        } else {
            throw std::invalid_argument("ERROR: invalid option name");
        }
    }
}

void GetFileData(FileData& file_data) {
    std::ifstream file(file_data.filename);

    char symb, prev_symb = ' ';
    while (file.get(symb)) {
        if (symb == '\n') {
            file_data.bytes += 2; //   \r\n
            file_data.lines += 1;
        } else {
            file_data.bytes += 1;
        }
        if (std::isprint(symb))
            file_data.chars += 1;
        if (isChar(prev_symb) && !isChar(symb)) {
            file_data.words += 1;
        }
        prev_symb = symb;
    }
    file_data.lines += 1; // last line
    if (isChar(prev_symb)) {
        file_data.words += 1;
    }
}

void PrintFileData(const FileData& file_data) {
    if (flags.lines) std::cout << "Lines: " << file_data.lines << '\n';
    if (flags.words) std::cout << "Words: " << file_data.words << '\n';
    if (flags.bytes) std::cout << "Bytes: " << file_data.bytes << '\n';
    if (flags.chars) std::cout << "Chars: " << file_data.chars << '\n';

    if (flags.lines + flags.words + flags.bytes + flags.chars == 0) {
        std::cout << file_data.lines << ' '
                  << file_data.words << ' '
                  << file_data.bytes << ' ';
    }
    std::cout << file_data.filename << "\n\n";
}

void ProcessFileRequests(std::vector<char*>& filenames) {
    for (size_t file_ind = 0; file_ind < filenames.size(); ++file_ind) {
        char* filename = filenames[file_ind];
        FileData cur_file_data(filename);

        GetFileData(cur_file_data);
        PrintFileData(cur_file_data);
    }
}

int main(int argc, char** argv) {
    std::vector<char*> filenames;
    for (size_t i = 1; i < argc; ++i) {
        char* term = argv[i];
        if (term[0] == '-') { // option
            ReadOption(term);
        } else { // filename
            filenames.push_back(term);
        }
    }
    ProcessFileRequests(filenames);

    return 0;
}