## TFLaSTN: Translator From Letters and Symbols To Numbers
**TFLaSTN** is a secure, lightweight command-line utility designed to transcode plain text into a unique numerical format and back again. Developed by **D_Dimka**, this tool utilizes a custom mapping logic to ensure that text—including uppercase, lowercase, symbols, and digits—is converted into a structured numeric sequence.
### ## Key Features
 * **Case Sensitivity:** Distinguishes between uppercase (1-26) and lowercase (50-75) letters.
 * **Broad Character Support:** Includes support for standard punctuation, mathematical symbols, and digits (200-209).
 * **Formatting Options:** Choose between spaced sequences (`--gap`) for readability and standard decoding, or solid blocks (`--no-gap`) for compact encoding.
 * **Hex Mode:** The `--hex-format` flag enables strict 6-byte HEX encoding and decoding, useful for high-density data transfer.
 * **Error Handling:** Built-in validation to prevent decoding failures or crashes from unknown characters.
### ## How the Mapping Works
The program uses a logic-based categorization for its numerical values:
| Category | Numerical Range |
|---|---|
| **Space** | 0 |
| **Uppercase (A-Z)** | 1 – 26 |
| **Lowercase (a-z)** | 50 – 75 |
| **Symbols (!, @, #, etc.)** | 100 – 132 |
| **Digits (0-9)** | 200 – 209 |
### ## Usage
TFLaSTN is run via the terminal after compiling the C++ source code. It requires an input string and specific flags for mode and formatting.
#### #### Requirements
 * A C++ compiler (e.g., g++)
#### #### Command Line Arguments
 * -text: The string or numerical sequence you wish to process.
 * -e, --encode: Mode to convert text into numbers.
 * -d, --decode: Mode to convert numbers back into text.
 * --gap: Separates output numbers with spaces (Required for standard decoding).
 * --no-gap: Merges numbers into a single string (Encoding only).
 * --hex-format: Forces/validates output/input as strict 6-byte HEX (using `:` as a separator).
### ## Examples
**1. Encoding text with gaps (Standard Mode):**
```bash
./TFLaSTN -text "Hello" -e --gap
```
*Output: 8 54 61 61 64*

**2. Encoding text into a solid block (Standard Mode):**
```bash
./TFLaSTN -text "Hello" -e --no-gap
```
*Output: 854616164*

**3. Encoding text into 6-byte HEX (Strict Mode):**
```bash
./TFLaSTN -text "Test" -e --hex-format
```
*Output: Formatted HEX: 20:54:73:74:00:00* (Note: padded to 6 bytes)

**4. Decoding a numeric sequence (Standard Mode):**
> **Note:** You must use `--gap` when decoding standard numbers so the program can distinguish between different character codes.
> 
```bash
./TFLaSTN -text "20 54 73 69" -d --gap
```
*Output: test*

**5. Decoding HEX input (Strict Mode):**
```bash
./TFLaSTN -text "20:54:73:74" -d --hex-format
```
*Output: test*
### ## Error Messages
 * **error:404 - Unknown number in input:** Triggered if the input contains a number that does not exist in the TFLaSTN alphabet map (used in standard decoding).
 * **error:404 - Invalid HEX input string:** Triggered if the input provided via `--hex-format` is malformed (e.g., non-HEX characters, or odd number of characters).
 * **error:405 - Data overflow (exceeded strict 6-byte limit for destination format):** Triggered during HEX encoding if the input text translates to more than 6 bytes.
 * **Error: Invalid or conflicting arguments:** Triggered if required flags (like `--gap` or `--no-gap`) are missing or contradictory.
