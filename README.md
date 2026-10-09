# Image Steganography using C

## 📌 Project Overview

This project implements image steganography in C, allowing text data to be hidden inside a BMP image and later extracted from the modified image.

The project demonstrates how binary image data can be manipulated at the bit level to embed and recover secret information.

## 🎯 Features

- Encode text data into a BMP image
- Decode hidden text from a stego image
- Uses C file handling for binary data processing
- Performs bit-level manipulation for data hiding
- Separate encoder and decoder programs
- Tested using GCC on Linux/WSL

## 🛠️ Technologies Used

- **Language:** C
- **Compiler:** GCC
- **Platform:** Linux / WSL
- **File Format:** BMP
- **Concepts:** Bit manipulation, pointers, structures, file handling, binary data processing

## 📂 Project Structure

| File | Description |
|---|---|
| `encode.c` | Encoding implementation |
| `encode.h` | Encoder declarations |
| `decode.c` | Decoding implementation |
| `decode.h` | Decoder declarations |
| `test_encode.c` | Encoder entry point |
| `test_decode.c` | Decoder entry point |
| `common.h` | Common definitions |
| `types.h` | Project data types |
| `beautiful.bmp` | Sample BMP image used for testing |
| `.gitignore` | Prevents generated/private files from being committed |

## 🔄 Working Flow

```text
Original BMP Image
       +
 Secret Text File
       |
       v
    Encoder
       |
       v
  Stego BMP Image
       |

       v
    Decoder
       |
       v
 Recovered Text

