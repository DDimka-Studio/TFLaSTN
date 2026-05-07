import argparse

def translate_text():
    # Setup the command-line argument parser with your name
    parser = argparse.ArgumentParser(description="D_Dimka Transcoder: Secure Text-to-Numbers tool")

    # Arguments for input and processing modes
    parser.add_argument("-text", type=str, required=True, help="Text or numbers to process")
    
    # Mode selection: Encode or Decode
    mode_group = parser.add_mutually_exclusive_group(required=True)
    mode_group.add_argument("-e", "--encode", action="store_true", help="Convert text to numbers")
    mode_group.add_argument("-d", "--decode", action="store_true", help="Convert numbers back to text")

    # Output formatting: spaces between numbers or a solid block
    format_group = parser.add_mutually_exclusive_group(required=True)
    format_group.add_argument("--gap", action="store_true", help="Separate numbers with spaces (required for decoding)")
    format_group.add_argument("--no-gap", action="store_true", help="Merge numbers into one string (encoding only)")

    args = parser.parse_args()

    # Unique mapping created by D_Dimka
    # Logic: Space=0, UpperCase=1-26, LowerCase=50-75, Symbols=100+, Digits=200+
    alphabet_map = {
        ' ': 0,
        'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5, 'F': 6, 'G': 7, 'H': 8, 'I': 9, 'J': 10, 'K': 11, 'L': 12, 'M': 13, 'N': 14, 'O': 15, 'P': 16, 'Q': 17, 'R': 18, 'S': 19, 'T': 20, 'U': 21, 'V': 22, 'W': 23, 'X': 24, 'Y': 25, 'Z': 26,
        'a': 50, 'b': 51, 'c': 52, 'd': 53, 'e': 54, 'f': 55, 'g': 56, 'h': 57, 'i': 58, 'j': 59, 'k': 60, 'l': 61, 'm': 62, 'n': 63, 'o': 64, 'p': 65, 'q': 66, 'r': 67, 's': 68, 't': 69, 'u': 70, 'v': 71, 'w': 72, 'x': 73, 'y': 74, 'z': 75,
        '.': 100, ',': 101, ':': 102, ';': 103, '!': 104, '?': 105, '/': 106, '\\': 107, '|': 108, '-': 109, '_': 110, '*': 111, '+': 112, '=': 113, '%': 114, '<': 115, '>': 116, '(': 117, ')': 118, '[': 119, ']': 120, '{': 121, '}': 122, "'": 123, '"': 124, '`': 125, '@': 126, '#': 127, '$': 128, '^': 129, '&': 130, '~': 131, '№': 132,
        '0': 200, '1': 201, '2': 202, '3': 203, '4': 204, '5': 205, '6': 206, '7': 207, '8': 208, '9': 209,
    }

    if args.decode:
        # Decoding without gaps is impossible due to varying number lengths
        if args.no_gap:
            print("error:404 - Decoding requires '--gap' mode")
            return
        
        # Creating a reverse lookup table (number -> character)
        reverse_map = {str(v): k for k, v in alphabet_map.items()}
        input_numbers = args.text.split()
        
        try:
            decoded_result = [reverse_map[num] for num in input_numbers]
            print("".join(decoded_result))
        except KeyError:
            print("error:404 - Unknown number in input")
        
    elif args.encode:
        # Case-sensitive encoding: A != a
        # We skip characters not found in the map to prevent crashes
        result = []
        for char in args.text:
            if char in alphabet_map:
                result.append(str(alphabet_map[char]))
            else:
                # If char not in map, we can put a placeholder or just skip
                continue
                
        separator = " " if args.gap else ""
        print(separator.join(result))

if __name__ == "__main__":
    translate_text()
