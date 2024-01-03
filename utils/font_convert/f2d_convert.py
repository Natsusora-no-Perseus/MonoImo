import os, sys, argparse
from typing import NamedTuple
from PIL import Image, ImageFont, ImageDraw
from collections import namedtuple

# Command line argument parser instance
cmd_parser = argparse.ArgumentParser(
        prog = 'f2d_convert.py',
        description = 'Converts .odf and .ttf fonts to LZ77 compressed bitmaps',
        epilog="For the '-t' option, a higher value (higher luminosity)\
         ignores more fine details,\n\t while the opposite preserves more details.")

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

cmd_parser.add_argument('-o', '--output',
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

# TODO: Add option to downsample font file

cmd_args = cmd_parser.parse_args()

CharHeader = namedtuple('CharHeader',
                        ['id', 'width', 'height', 'xoffset', 'yoffset',
                        'xadvance', 'lkern', 'rkern'])

char_lst = [] # list to hold the generated bitmaps
# Element in format (<ch_header>, <img>)



# Construct a charheader (i.e. individual character info)
def get_charheader(character, font):
    bbox = font.getbbox(character) # Get bounding box of character
    width = bbox[2]
    height = bbox[3]
    xoffset, yoffset = bbox[0], bbox[1]
    xadvance = font.getbbox(character + character)[2] - bbox[2]
    lkern, rkern = 0, 0 # TODO: implement method to calculate kerning
    return CharHeader(ord(character), width, height, xoffset, yoffset,
                      xadvance, lkern, rkern)


# Create a bitmap image from charheader info
def get_image(ch_header, thresh):
    ret = Image.new('L', (ch_header.width, ch_header.height), color=255)
    draw_context = ImageDraw.Draw(ret)
    draw_context.text((0, 0),
                      chr(ch_header.id), font=font)

    ret = ret.point(lambda pix: 255 if pix > thresh else 0)
    ret = ret.convert('1') # convert to monochrome
    return ret



# Adds a character's header and bitmap to a list;
# returns False if it already exists in the list, True otherwise
def list_addch(ch_header, img, lst):
    if any(elem[0].id == ch_header.id for elem in lst):
        return False
    else:
        lst.append((ch_header, img))
        return True



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
"""
Individual character header information:
'id’               : Unicode encoding of this character
'width' 'height'   : bounding box size (in pixels)
'xoffset' 'yoffset': x and y distance from (0, 0) (top left corner)
'xadvance'         : advancement in x-direction after drawing one character
'lkern' 'rkern'    : right and left kerning
"""


for elem in cmd_args.include: # we first parse all --include arguments
    for i in range(elem[0], elem[1]+1):
        ch_header = get_charheader(chr(i), font)
        ret_img = get_image(ch_header, cmd_args.threshold[0])
        if cmd_args.format[0] == 'BMP':
            ret_img.save(cmd_args.output + str(i) + '.bmp')
        list_addch(ch_header, ret_img, char_lst)
        if cmd_args.verbose > 1:
            print('Converting \"' + chr(i) + '\"to bitmap ...')
        if cmd_args.verbose > 2:
            print('Added chr with header ' + repr(ch_header) + ' to internal list.')


if cmd_args.format[0] == 'BMP':
    sys.exit('Bitmaps saved. Exiting ...')

# TODO: ... then we parse all --Include parameters
