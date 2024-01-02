import os, sys, argparse
from PIL import Image, ImageFont, ImageDraw

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

cmd_parser.add_argument('-i', '--include',
                        help='range (in unicode) of characters to be converted',
                        default=(32, 128),
                        nargs='+',
                        type=tuple)

cmd_parser.add_argument('-I', '--Include',
                        help='file containing characters to be converted',
                        nargs='+', # Quit on empty argument
                        type=argparse.FileType('r'))

cmd_parser.add_argument('-o, --output',
                        default='output',
                        help='name of output file')

cmd_parser.add_argument('-f', '--format',
                        choices=['HEADER', 'RAW', 'BMP'],
                        default='HEADER',
                        nargs=1,
                        help='format of output file (c header, raw bytes, BMPs)')


args = cmd_parser.parse_args()

# print(args.fontname)

font = ImageFont.truetype(args.fontname)
