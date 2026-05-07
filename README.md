## TFLaSTN: Translator From Letters and Symbols To Numbers
**TFLaSTN** is a secure, lightweight command-line utility designed to transcode plain text into a unique numerical format and back again. Developed by **D_Dimka**, this tool utilizes a custom mapping logic to ensure that text—including uppercase, lowercase, symbols, and digits—is converted into a structured numeric sequence.
### ## Key Features
 * **Case Sensitivity:** Distinguishes between uppercase (1-26) and lowercase (50-75) letters.
 * **Broad Character Support:** Includes support for standard punctuation, mathematical symbols, and digits (200-209).
 * **Formatting Options:** Choose between spaced sequences (--gap) for readability and decoding, or solid blocks (--no-gap) for compact encoding.
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
TFLaSTN is run via the terminal using Python. It requires an input string and specific flags for mode and formatting.
#### #### Requirements
 * Python 3.x
 * argparse module (standard in Python)
#### #### Command Line Arguments
 * -text: The string or numerical sequence you wish to process.
 * -e, --encode: Mode to convert text into numbers.
 * -d, --decode: Mode to convert numbers back into text.
 * --gap: Separates output numbers with spaces (Required for decoding).
 * --no-gap: Merges numbers into a single string (Encoding only).
### ## Examples
**1. Encoding text with gaps:**
```
python program.py -text "Hello" -e --gap

```
*Output: 8 54 61 61 64*
**2. Encoding text into a solid block:**
```bash
python program.py -text "Hello" -e --no-gap

```
*Output: 854616164*
**3. Decoding a numeric sequence:**
> **Note:** You must use --gap when decoding so the program can distinguish between different character codes.
> 
```
python program.py -text "20 54 73 69" -d --gap

```
*Output: test*
### ## Error Messages
 * **error:404 - Decoding requires '--gap' mode:** Triggered if you attempt to decode a solid block of numbers, as the variable lengths of the codes make them indistinguishable without spaces.
 * **error:404 - Unknown number in input:** Triggered if the input contains a number that does not exist in the TFLaSTN alphabet map.
