import argparse
import sys

def translate_text():
    # Setup the command-line argument parser with the correct project name
    parser = argparse.ArgumentParser(description="TFLaSTN: Secure Letters and Symbols To Numbers transcoder")

    # Arguments for input and processing modes
    parser.add_argument("-text", type=str, required=True, help="Text or numbers to process")
    
    # Mode selection: Encode or Decode
    mode_group = parser.add_mutually_exclusive_group(required=True)
    mode_group.add_argument("-e", "--encode", action="store_true", help="Convert text to numbers")
    mode_group.add_argument("-d", "--decode", action="store_true", help="Convert numbers back to text")

    # Output formatting flags
    format_group = parser.add_mutually_exclusive_group(required=True)
    format_group.add_argument("--gap", action="store_true", help="Separate numbers with spaces (required for default decoding)")
    format_group.add_argument("--no-gap", action="store_true", help="Merge numbers into one string (encoding only)")
    format_group.add_argument("--hex-format", action="store_true", help="Format/validate output as strict 6-byte HEX, or decode from it")

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
        # HEX decoding mode (from MAC/Bluetooth hardware address format)
        if args.hex_format:
            # Strip colons, spaces and force uppercase for normalization
            clean_hex = args.text.replace(":", "").replace(" ", "").upper()
            
            # Validation: Ensure input contains only valid HEX characters and has an even length
            if not all(c in "0123456789ABCDEF" for c in clean_hex) or len(clean_hex) % 2 != 0:
                print("error:404 - Invalid HEX input string")
                return
                
            # Split solid HEX string into pairs of 2 characters
            hex_pairs = [clean_hex[i:i+2] for i in range(0, len(clean_hex), 2)]
            
            # Convert each HEX pair back to a decimal number string
            input_numbers = [str(int(pair, 16)) for pair in hex_pairs]
            
            # Remove technical trailing zeros used for 6-byte padding
            # Keeps standalone '0' if it represents an actual space character in the middle
            while len(input_numbers) > 0 and input_numbers[-1] == '0':
                input_numbers.pop()
        else:
            # Default decoding mode (requires numbers separated by spaces)
            input_numbers = args.text.split()

        # Reconstruct original text using reverse mapping
        reverse_map = {str(v): k for k, v in alphabet_map.items()}
        try:
            decoded_result = [reverse_map[num] for num in input_numbers]
            print("".join(decoded_result))
        except KeyError:
            print("error:404 - Unknown number in input")
        
    elif args.encode:
        # HEX encoding mode (into strict 6-byte hardware address format)
        if args.hex_format:
            hex_chars = []
            for char in args.text:
                if char in alphabet_map:
                    val = alphabet_map[char]
                    hex_chars.append(f"{val:02X}")
                else:
                    continue
            
            solid_hex = "".join(hex_chars)
            
            # Validate 6-byte limitation (maximum 12 HEX characters allowed)
            if len(solid_hex) > 12:
                allowed_hex = solid_hex[:12]
                formatted_hex = ":".join([allowed_hex[i:i+2] for i in range(0, 12, 2)])
                print(f"Formatted HEX (Truncated): {formatted_hex}")
                print("error:405 - Data overflow (exceeded strict 6-byte limit for destination format)")
                return
            
            # Pad with trailing zeros to fit exactly 6 bytes (12 chars)
            solid_hex = solid_hex.ljust(12, '0')
            formatted_hex = ":".join([solid_hex[i:i+2] for i in range(0, 12, 2)])
            print(f"Formatted HEX: {formatted_hex}")
            return

        # Default encoding mode
        result = []
        for char in args.text:
            if char in alphabet_map:
                result.append(str(alphabet_map[char]))
            else:
                continue
                
        separator = " " if args.gap else ""
        print(separator.join(result))

if __name__ == "__main__":
    translate_text()
