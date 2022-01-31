#include <cstdio>
#include <cstring>
#include <vector>

size_t process_shift(const char* token, const char cur, size_t shift, const std::vector<size_t> &table) {
    while (shift != -1 && cur != token[shift]) {
        shift = table[shift];
    }
    return shift;
}

void build_table(const char* token, std::vector<size_t> &table) {
    table[0] = -1;
    size_t shift = 0;
    for (size_t pos = 1; pos < table.size(); pos++, shift++) {
        if (token[pos] == token[shift]) {
            table[pos] = table[shift];
        } else {
            table[pos] = shift;
            shift = process_shift(token, token[pos], shift, table);
        }
    }
    table.push_back(shift);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Invalid input\nusage: %s <source> <string>", argv[0]);
        return -1;
    }
    FILE *input = fopen(argv[1], "rb");
    if (!input) {
        fprintf(stderr, "Could not open file\n");
        return -1;
    }
    const char* token = argv[2];
    const size_t str_len = strlen(token);
    size_t shift = 0;
    std::vector<size_t> table(str_len);
    build_table(token, table);
    int ch;
    while ((ch = fgetc(input)) != EOF) {
        shift = process_shift(token, (char)ch, shift, table);
        if (++shift == str_len) {
            printf("Yes\n");
            fclose(input);
            return 0;
        }
    }
    if (ferror(input)) {
        fprintf(stderr, "Corrupted file\n");
        fclose(input);
        return -1;
    }
    printf("No\n");
    fclose(input);
    return 0;
}

