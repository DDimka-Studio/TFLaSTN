#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>

// Table for instant conversion of numbers to uppercase HEX characters
const char HEX_LOOKUP[] = "0123456789ABCDEF";

// Forward array for encoding (index = character ASCII code)
int enc_mask[256];
// Reverse array for decoding (index = encoded number)
char dec_mask[256];
// Validity flags for decoding
bool dec_valid[256];

void init_maps() {
    // Initialize arrays with default values (-1 means no character)
    std::fill_n(enc_mask, 256, -1);
    std::fill_n(dec_valid, 256, false);

    // Mapping: 0 = Space, Uppercase letters = 1-26, Lowercase = 50-75, Symbols = 100-132, Digits = 200-209
    std::pair<char, int> mapping[] = {
        {' ', 0},
        {'A', 1}, {'B', 2}, {'C', 3}, {'D', 4}, {'E', 5}, {'F', 6}, {'G', 7}, {'H', 8}, {'I', 9}, {'J', 10}, 
        {'K', 11}, {'L', 12}, {'M', 13}, {'N', 14}, {'O', 15}, {'P', 16}, {'Q', 17}, {'R', 18}, {'S', 19}, 
        {'T', 20}, {'U', 21}, {'V', 22}, {'W', 23}, {'X', 24}, {'Y', 25}, {'Z', 26},
        {'a', 50}, {'b', 51}, {'c', 52}, {'d', 53}, {'e', 54}, {'f', 55}, {'g', 56}, {'h', 57}, {'i', 58}, 
        {'j', 59}, {'k', 60}, {'l', 61}, {'m', 62}, {'n', 63}, {'o', 64}, {'p', 65}, {'q', 66}, {'r', 67}, 
        {'s', 68}, {'t', 69}, {'u', 70}, {'v', 71}, {'w', 72}, {'x', 73}, {'y', 74}, {'z', 75},
        {'.', 100}, {',', 101}, {':', 102}, {';', 103}, {'!', 104}, {'?', 105}, {'/', 106}, {'\\', 107}, 
        {'|', 108}, {'-', 109}, {'_', 110}, {'*', 111}, {'+', 112}, {'=', 113}, {'%', 114}, {'<', 115}, 
        {'>', 116}, {'(', 117}, {')', 118}, {'[', 119}, {']', 120}, {'{', 121}, {'}', 122}, {'\'', 123}, 
        {'"', 124}, {'`', 125}, {'@', 126}, {'#', 127}, {'$', 128}, {'^', 129}, {'&', 130}, {'~', 131},
        {'0', 200}, {'1', 201}, {'2', 202}, {'3', 203}, {'4', 204}, {'5', 205}, {'6', 206}, {'7', 207}, 
        {'8', 208}, {'9', 209}
    };

    for (const auto& p : mapping) {
        enc_mask[static_cast<unsigned char>(p.first)] = p.second;
        dec_mask[p.second] = p.first;
        dec_valid[p.second] = true;
    }
}

// Convert HEX character to number 0-15
inline int hex_char_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

void print_help() {
    std::cout << "TFLaSTN: Translator From Letters and Symbols To Numbers\n\n"
              << "Usage:\n"
              << "  -text <string>       Text or numbers to process\n"
              << "  -file <path>         Read input from a file\n"
              << "  -out <path>          Save output to a file\n"
              << "  -e, --encode         Convert text to numbers\n"
              << "  -d, --decode         Convert numbers back to text\n"
              << "  --gap                Separate numbers with spaces (required for default decoding)\n"
              << "  --no-gap             Merge numbers into one string (encoding only)\n"
              << "  --hex-format         Format/validate output as strict 6-byte HEX, or decode from it\n";
}

int main(int argc, char* argv[]) {
    // Disables console delays
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string text = "";
    bool encode = false, decode = false;
    bool gap = false, no_gap = false, hex_format = false;
    bool text_provided = false;
    std::string file_path = "";
    bool file_provided = false;
    std::string out_path = "";
    bool out_provided = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-text" && i + 1 < argc) {
            text = argv[++i];
            text_provided = true;
        } else if (arg == "-file" && i + 1 < argc) {
            file_path = argv[++i];
            file_provided = true;
        } else if (arg == "-out" && i + 1 < argc) {
            out_path = argv[++i];
            out_provided = true;
        } else if (arg == "-e" || arg == "--encode") {
            encode = true;
        } else if (arg == "-d" || arg == "--decode") {
            decode = true;
        } else if (arg == "--gap") {
            gap = true;
        } else if (arg == "--no-gap") {
            no_gap = true;
        } else if (arg == "--hex-format") {
            hex_format = true;
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        }
    }

    if ((!text_provided && !file_provided) || (encode == decode) || (!gap && !no_gap && !hex_format) || (gap && no_gap) || (gap && hex_format) || (no_gap && hex_format)) {
        std::cerr << "Error: Invalid or conflicting arguments.\n";
        print_help();
        return 1;
    }

    init_maps();

    if (file_provided) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open input file: " << file_path << "\n";
            return 1;
        }
        text.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    std::ofstream out_file;
    if (out_provided) {
        out_file.open(out_path);
        if (!out_file) {
            std::cerr << "Error: Could not open output file for writing: " << out_path << "\n";
            return 1;
        }
    }
    std::ostream& out = out_provided ? out_file : std::cout;

    if (decode) {
        std::vector<int> input_numbers;
        input_numbers.reserve(text.length() / 2); // Pre-allocate memory

        if (hex_format) {
            // Clean and parse HEX on the fly without intermediate strings
            int high = -1;
            for (char c : text) {
                if (c == ':' || c == ' ') continue;
                int val = hex_char_to_val(c);
                if (val == -1) {
                    std::cerr << "error:404 - Invalid HEX input string\n";
                    return 0;
                }
                if (high == -1) {
                    high = val;
                } else {
                    input_numbers.push_back((high << 4) | val);
                    high = -1;
                }
            }
            if (high != -1) { // Odd number of HEX characters
                std::cerr << "error:404 - Invalid HEX input string\n";
                return 0;
            }

            // Remove trailing zeros
            while (!input_numbers.empty() && input_numbers.back() == 0) {
                input_numbers.pop_back();
            }
        } else {
            // Manual parsing of numbers by spaces
            int current_num = 0;
            bool has_digit = false;
            for (char c : text) {
                if (c >= '0' && c <= '9') {
                    current_num = current_num * 10 + (c - '0');
                    has_digit = true;
                } else if (c == ' ') {
                    if (has_digit) {
                        input_numbers.push_back(current_num);
                        current_num = 0;
                        has_digit = false;
                    }
                }
            }
            if (has_digit) input_numbers.push_back(current_num);
        }

        // Output decoded result directly to the stream
        std::string decoded_result;
        decoded_result.reserve(input_numbers.size());
        for (int num : input_numbers) {
            if (num >= 0 && num < 256 && dec_valid[num]) {
                decoded_result.push_back(dec_mask[num]);
            } else {
                std::cerr << "error:404 - Unknown number in input\n";
                return 0;
            }
        }
        out << decoded_result << "\n";
    } 
    else if (encode) {
        if (hex_format) {
            std::vector<unsigned char> bytes;
            bytes.reserve(text.length());
            
            for (char c : text) {
                int val = enc_mask[static_cast<unsigned char>(c)];
                if (val != -1) {
                    bytes.push_back(static_cast<unsigned char>(val));
                }
            }

            auto print_as_hex = [&](std::ostream& os, const std::vector<unsigned char>& b_vec) {
                for (size_t i = 0; i < b_vec.size(); ++i) {
                    os << HEX_LOOKUP[(b_vec[i] >> 4) & 0x0F] << HEX_LOOKUP[b_vec[i] & 0x0F];
                    if (i < b_vec.size() - 1) os << ':';
                }
            };

            if (bytes.size() > 6) {
                // 1. First 6 bytes for MAC
                out << "MAC: ";
                std::vector<unsigned char> truncated(bytes.begin(), bytes.begin() + 6);
                print_as_hex(out, truncated);
                out << "\n";

                // 2. Error
                std::cerr << "error:405 - Data overflow (exceeded strict 6-byte limit for destination format)\n";

                // 3. Full string
                out << "Full HEX: ";
                print_as_hex(out, bytes);
                out << "\n";
                return 0;
            }

            // If less than 6 bytes, pad with zeros
            while (bytes.size() < 6) {
                bytes.push_back(0);
            }

            out << "Formatted HEX: ";
            print_as_hex(out, bytes);
            out << "\n";
            return 0;
        }

        // Standard encoding
        bool first = true;
        for (char c : text) {
            int val = enc_mask[static_cast<unsigned char>(c)];
            if (val != -1) {
                if (!first && gap) out << ' ';
                out << val;
                first = false;
            }
        }
        out << "\n";
    }

    return 0;
}
