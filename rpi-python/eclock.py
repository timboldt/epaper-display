#!/usr/bin/env python3
from PIL import Image

def main():
    im = Image.open("IMG_2897.JPG")
    print(im.format, im.size, im.mode)

if __name__ == "__main__":
    main()