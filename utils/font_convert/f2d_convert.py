import os, sys, argparse
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
                        nargs='?',
                        type=int,
                        help='the size of the font to be converted')

cmd_parser.add_argument('-i', '--include',
                        help='range (in unicode) of characters to be converted',
                        default=[],
                        nargs='+',
                        type=int)

cmd_parser.add_argument('-I', '--Include',
                        help='file containing characters to be converted',
                        nargs='+', # Quit on empty argument
                        type=argparse.FileType('r', encoding='UTF-8'))

cmd_parser.add_argument('-d', '--downsample',
                        default=1,
                        help='scales the resulting picture by given factor',
                        nargs='?',
                        metavar='FACTOR',
                        type=float)

cmd_parser.add_argument('-r', '--resample',
                        choices=['NEAREST', 'BOX', 'BILINEAR', 'HAMMING',
                                 'BICUBIC', 'LANCZOS'],
                        default='BILINEAR',
                        dest='resample',
                        nargs='?',
                        help='the type of resampling algorithm used to scale font')

cmd_parser.add_argument('-t', '--threshold',
                        # TODO: fix this!
                        #choices=range(0, 256),
                        default=128,
                        help='threshold for binarizing a character bitmap (0~255)',
                        metavar='THRESH',
                        nargs='?',
                        type=int)

cmd_parser.add_argument('-k', '--kerning-ref',
                        help='reference glyph for calculating kerning',
                        default='B',
                        nargs='?')

cmd_parser.add_argument('-f', '--format',
                        choices=['HEADER', 'RAW', 'BMP'],
                        default='HEADER',
                        nargs='?',
                        help='format of output file (c header, raw bytes, BMPs)')

cmd_parser.add_argument('-o', '--output',
                        default='output',
                        help='name of output file',
                        nargs='?',
                        metavar='OUTPUT')

cmd_parser.add_argument('-v', '--verbose',
                        action='count',
                        default=1,
                        help='verbose level (0 ~ 3)')

# TODO: An option to set the kerning reference glyphs MUST be added,
      # if kerning is to be allowed to work with non-english glyphs
# TODO: Add option to begin halfways, i.e. convert bitmaps to bit sequence

cmd_args = cmd_parser.parse_args()

GlyphHeader = namedtuple('GlyphHeader',
                        ['id', 'width', 'height', 'xoffset', 'yoffset',
                        'xadvance', 'lkern', 'rkern'])

# Element in format (<ch_header>, <img>)
glyph_lst = [] # list to hold the generated bitmaps



# Construct a GlyphHeader (i.e. individual glyph info)
def get_glyphheader(character, font):
    bbox = font.getbbox(character) # Get bounding box of character
    width = round(bbox[2]*cmd_args.downsample)
    height = round(bbox[3]*cmd_args.downsample)
    xoffset, yoffset = bbox[0], bbox[1]
    xadvance = font.getbbox(character + character)[2] - bbox[2]
    lkern, rkern = 0, 0 # TODO: implement method to calculate kerning

    return GlyphHeader(ord(character), width, height, xoffset, yoffset,
                      xadvance, lkern, rkern)


# Create a bitmap image from GlyphHader info
def get_image(gly_header, thresh, font):
    bbox = font.getbbox(chr(gly_header.id))
    ret = Image.new('L', (bbox[2], bbox[3]), color=255)
    draw_context = ImageDraw.Draw(ret)
    draw_context.text((0, 0), chr(gly_header.id), font=font)

    resamp_dict= \
        {'NEAREST': Image.Resampling.NEAREST, 'BOX': Image.Resampling.BOX,\
        'BILINEAR': Image.Resampling.BILINEAR, 'HAMMING': Image.Resampling.HAMMING,\
        'BICUBIC': Image.Resampling.BICUBIC, 'LANCZOS': Image.Resampling.LANCZOS}

    ret = ret.resize((int(ret.width*cmd_args.downsample),\
                int(ret.height*cmd_args.downsample)),\
            resample=resamp_dict[cmd_args.resample])
    ret = ret.point(lambda pix: 255 if pix > thresh else 0)
    ret = ret.convert('1') # convert to monochrome
    return ret



# Adds a glyph's header and bitmap to a list;
# returns False if it already exists in the list, True otherwise
def list_addglyph(gly_header, img, lst):
    if any(elem[0].id == gly_header.id for elem in lst):
        return False
    else:
        lst.append((gly_header, img))
        return True


# Adds all characters with Unicode encoding within a range into a list
def num_addch(num, lst, font, cmd_args):
    print('h')
    header = get_glyphheader(chr(num), font)
    img = get_image(header, cmd_args.threshold, font)
    if list_addglyph(header, img, lst): # Not a replicate
        if cmd_args.format == 'BMP': # Begin output file
            img.save(cmd_args.output + str(num) + '.bmp')

        if cmd_args.verbose > 1:
            print('Added \"' + chr(num) + '\" to glyph list')
        if cmd_args.verbose > 2:
            print('Header: ' + repr(header))



# Adds all characters read from a text file into a list
def txt_addch(file, lst, font, cmd_args):
    ch = file.read(1)
    while ch != '':
        header = get_glyphheader(ch, font)
        img = get_image(header, cmd_args.threshold, font)
        if list_addglyph(header, img, lst): # Not a replicate
            if cmd_args.format == 'BMP':
                img.save(cmd_args.output + str(header.id) + '.bmp')

            if cmd_args.verbose > 1:
                print('Added \"' + ch + '\" to glyph list')
            if cmd_args.verbose > 2:
                print('Header: ' + repr(header))
        else:
            print('Ignoring replicate character \"' + ch + '\"')
        ch = file.read(1)
    file.close()



# Read the font in
try:
    font = ImageFont.truetype(cmd_args.fontname[0], cmd_args.size)
except OSError:
    sys.exit("Failed to load font. Perhaps the name is wrong?")


if cmd_args.include != None:
    if len(cmd_args.include) % 2 == 0: # Convert to tuple pairs
        for i in range(0, len(cmd_args.include) // 2):
            cmd_args.include[i] = [cmd_args.include[i], cmd_args.include[i+1]]
            cmd_args.include[i].sort()
            cmd_args.include[i] = tuple(cmd_args.include[i])
            del(cmd_args.include[i+1])
        if cmd_args.verbose > 1:
            print("Characters ingested from --input option.")
    else:
        sys.exit("Input range has odd number of arguments.")


# ======= Now we convert the glyphs individually to bitmaps:
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
        num_addch(i, glyph_lst, font, cmd_args)

if cmd_args.Include != None:
    if cmd_args.verbose > 1:
        print("Characters ingested from --Input option.")
    for elem in cmd_args.Include: # we then parse all --Include arguments
        txt_addch(elem, glyph_lst, font, cmd_args)

if cmd_args.format == 'BMP': # if this is true, then we are done
    sys.exit('Bitmaps saved. Exiting ...')



# ======= Now we got the bitmaps, let's convert them to bit sequences:
