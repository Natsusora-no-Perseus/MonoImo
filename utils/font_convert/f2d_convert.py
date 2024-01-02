import os, sys, argparse
from typing import NamedTuple
from PIL import Image, ImageFont, ImageDraw
from collections import namedtuple

# Command line argument parser instance
cmd_parser = argparse.ArgumentParser(
        prog = 'f2d_convert.py',
        description = 'Converts .odf and .ttf fonts to LZ77 compressed bitmaps')

cmd_parser.add_argument('fontname',
                        nargs=1,
                        help='name of the font to be converted')

cmd_parser.add_argument('-s', '--size',
                        default=12,
                        dest='size',
                        nargs=1,
                        type=int,
                        help='the size of the font to be converted')

cmd_parser.add_argument('-r', '--resample',
                        choices=['NEAREST', 'BOX', 'BILINEAR', 'HAMMING',
                                 'BICUBIC', 'LANCZOS'],
                        default='BILINEAR',
                        dest='resample',
                        nargs=1,
                        help='the type of resampling algorithm used to scale font')

cmd_parser.add_argument('-t', '--threshold',
                        #choices=range(0, 256),
                        default=128,
                        help='threshold for binarizing a character bitmap (0~255)',
                        metavar='THRESH',
                        nargs=1,
                        type=int)

cmd_parser.add_argument('-i', '--include',
                        help='range (in unicode) of characters to be converted',
                        default=[32, 128],
                        nargs='+',
                        type=int)

cmd_parser.add_argument('-I', '--Include',
                        help='file containing characters to be converted',
                        nargs='+', # Quit on empty argument
                        type=argparse.FileType('r'))

cmd_parser.add_argument('-o, --output',
                        default='output',
                        help='name of output file',
                        metavar='OUTPUT')

cmd_parser.add_argument('-f', '--format',
                        choices=['HEADER', 'RAW', 'BMP'],
                        default='HEADER',
                        nargs=1,
                        help='format of output file (c header, raw bytes, BMPs)')

cmd_parser.add_argument('-v', '--verbose',
                        action='count',
                        default=1,
                        help='verbose level (0 ~ 3)')

# TODO: Add arguments for bitmap image input & output

cmd_args = cmd_parser.parse_args()

# Read the font in
try:
    font = ImageFont.truetype(cmd_args.fontname[0], cmd_args.size[0])
except OSError:
    sys.exit("Failed to load font. Perhaps the name is wrong?")

if len(cmd_args.include) % 2 != 0:
    sys.exit("Input range has odd number of arguments.")
else: # Convert to tuple pairs
    for i in range(0, len(cmd_args.include) // 2):
        cmd_args.include[i] = [cmd_args.include[i], cmd_args.include[i+1]]
        cmd_args.include[i].sort()
        cmd_args.include[i] = tuple(cmd_args.include[i])
        del(cmd_args.include[i+1])
    if cmd_args.verbose > 1:
        print("Characters included.")

# ===== Now we convert the characters individually to bitmaps:
# Individual character header
CharHeader = namedtuple('CharHeader',
                        ['id', 'width', 'height', 'xoffset', 'yoffset',
                        'xadvance', 'lkern', 'rkern'])

def get_charheader(character):
    bbox = font.getbbox(character) # Get bounding box of character
    width = bbox[2] - bbox[0]
    height = bbox[3] - bbox[1]
    xoffset, yoffset = bbox[0], bbox[1]
    xadvance = font.getbbox(character + character)[2] - bbox[2]
    lkern, rkern = 0, 0 # TODO: implement method to calculate kerning
    return CharHeader(ord(character), width, height, xoffset, yoffset,
                      xadvance, lkern, rkern)



def get_image(ch_header, thresh):
    ret = Image.new('L', (ch_header.width, ch_header.height), color=255)
    draw_context = ImageDraw.Draw(ret)
    draw_context.text((0-ch_header.xoffset, 0-ch_header.yoffset), chr(ch_header.id), font=font)

    ret = ret.point(lambda pix: 255 if pix > thresh else 0)
    ret = ret.convert('1') # convert to monochrome
    return ret



for elem in cmd_args.include:
    for i in range(elem[0], elem[1]+1):
        if cmd_args.verbose > 2:
            print('Converting \"' + chr(i) + '\"to bitmap ...')
            ch_header = get_charheader(chr(i))
            print(ch_header)
            ret = get_image(ch_header, 128)
            ret.save(str(i) + '_ch.bmp')


