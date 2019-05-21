#!/usr/bin/env python3
from argparse import ArgumentParser
from PIL import Image
import os, struct, zlib

def convert_to_pixels(lst):
    pixels = []
    while(len(lst)>0):
        pxl = lst[:4]
        pixels.append(tuple(pxl))
        lst = lst[4:]
    return pixels

parser = ArgumentParser()

parser = ArgumentParser(description='Convert monochrome images into C arrays of bytes')
parser.add_argument('input_image', help='the image to convert')

parser.add_argument('-o',
                    dest='output_format',
                    choices=('t', 'i', 'a'),
                    default='i',
                    help='output as 8x8 tiles(=t), raw image(=i) (default) or alpha mask (=a)')

parser.add_argument('--inverted', '--inv',
                    dest='inverted',
                    action='store_true',
                    default=False,
                    help='input image should be treated as inverse b/w')

parser.add_argument('-f', dest='output_filename', help='output filename')

args = parser.parse_args()

try:
    im = open(args.input_image, 'rb')
except (FileNotFoundError):
    print("File {} not found.".format(args.input_image))
    exit(1)

raw_data = im.read()
data = bytearray(raw_data)
im.close()

header = data[:128]
header = struct.unpack('<LHHHHHLHLLBxxxHBB', header[:36])
print("Magic Number: {0:#04x}".format(header[1]))

frame_count = header[2]

frame_data = data[128:]
frame_header = frame_data[:16]
frame_header = struct.unpack('<LHHHHL', frame_header)
print("Num Frames:", frame_count)
frames = []
for f in range(frame_count):
    frame_chunk_count = frame_header[2]
    ptr = 16
    layers = []
    for i in range(frame_chunk_count):
        layer = {}
        chunk_data = struct.unpack('<LH', frame_data[ptr:ptr+6])
        size = chunk_data[0]
        type = "{0:#04x}".format(chunk_data[1])
        print(type)
        #TODO: allow for hidden layers.
        #if type == '0x2004': # Layer Chunk

        if type == '0x2005': # Cel Chunk
            chunk_data = frame_data[ptr+6: ptr+size]
            chunk_header = struct.unpack('<HhhBHxxxxxxxHH', chunk_data[:20])
            layer['pixels'] = convert_to_pixels(zlib.decompress(chunk_data[20:]))
            layer['width']=chunk_header[5]
            layer['height']=chunk_header[6]

            layers.append(layer)
        ptr += size
    if len(layers)>0:
        frames.append(layers)

""" OK, so Aseprite is far cleverer than I am and stores things very efficiently
    which means I have to do a whole bunch more work to extract things in the
    formats I need.

    I need to:
      Extract the layer information to get the *actual* sprite size
      fill in missing data (it'll just be zeroes)
      work out how it stores the background layer colour!

    """

print(len(frames))
for f in frames:
    print(len(f))
print(len(frames[0][0]['pixels']), frames[0][0]['pixels'])


exit()


pixels = frames[0][0]['pixels']

width = frames[0][0]['width']
height = frames[0][0]['width']

if width % 8 > 0 or height % 8 > 0:
    print("Image dimensions not divisible by 8.\n\nThis is currently unsupported.")
    exit(1)

image = []

for y in range(height // 8):
    for x in range(width):
        b = 0
        for i in range(8):
            pixel = pixels[((y*8)+i) * width + x]
            if args.inverted:
                if pixel == (255, 255, 255) or pixel == (255, 255, 255, 255):
                    b = b | (1 << i)
            elif args.output_format == 'a':
                if pixel == (255, 0, 255) or pixel == (255, 0, 255, 255):
                    b = b | (1 << i)
            else:
                if pixel == (0, 0, 0) or pixel == (0, 0, 0, 255):
                    b = b | (1 << i)
        image.append(b)

output_filename = args.output_filename
if not output_filename:
    output_filename = os.path.splitext(os.path.split(args.input_image)[1])[0]
    if args.output_format == 'a':
         output_filename += '-masks.txt'
    else:
        output_filename += '.txt'

f = open(output_filename, 'w')

if args.output_format == 't' or args.output_format == 'a':
    while(len(image) > 0):
        block = image[:8]
        image = image[8:]
        f.write("{0:#04x}, {1:#04x}, {2:#04x}, {3:#04x}, {4:#04x}, {5:#04x}, {6:#04x}, {7:#04x}, \n".format(*block))

else:
    for y in range(height // 8):
        line = ''
        for x in range(width):
            line = line + "{:#04x}, ".format(image.pop(0))

        f.write(line + "\n")

f.close()
