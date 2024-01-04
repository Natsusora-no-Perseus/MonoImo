# find_kref.py
# Finding the glyph affected by kerning the least in a font.
# Part of MonoImo library's utility scripts.
#
# Only works for Python version >= 3.8.
# For help, run `python3 find_kref.py -h`
#
# For more complete documentation, visit the repository below:
# https://github.com/Natsusora-no-Perseus/MonoImo
#
# This script is free software: you can distribute it and/or modify it under the terms
# of the GNU Lesser General Public License as published by the Free Software
# Foundation, either version 2.0 or later.
# This script is distributed in hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the GNU LGPL License for more details.
#
# (C) 2024- , https://github.com/Natsusora-no-Perseus


import sys, argparse
from PIL import ImageFont
from collections import namedtuple
from statistics import mean, median, stdev

parser = argparse.ArgumentParser(
        prog = 'find_kref.py',
        description='Finds the reference glyph for calculating kerning')

parser.add_argument('font', help='The font to be analyzed')

parser.add_argument('-i', '--include',
                    help='range (in Unicode) of characters to be covered',
                    default=[],
                    nargs='+',
                    type=int)

parser.add_argument('-s', '--size',
                    default=12,
                    dest='size',
                    nargs=1,
                    type=int,
                    help='the size of the font to be converted')

parser.add_argument('-r', '--rankby',
                    choices=['LMEAN', 'LMED', 'LSTDEV', 'LMAX',\
                             'RMEAN', 'RMED', 'RSTDEV', 'RMAX'],
                    metavar='SORTBY ...',
                    nargs='+',
                    help='display results are ranked by this argument;\
                            available options: {L, R} + {MEAN, MED, STDEV, MAX}')

parser.add_argument('-d', '--descending',
                    action='store_true',
                    help='ranks in descending order instead')

parser.add_argument('-c', '--count',
                    help='how many glyphs are displayed in the result',
                    default=10,
                    nargs=1,
                    type=int)


args = parser.parse_args()
chr_lst = []
kern_lst = []

KernStats = namedtuple('KernStats',
                       ['chr', 'lmean', 'lmed', 'lstdev', 'lmax',
                        'rmean', 'rmed', 'rstdev', 'rmax'])
# Make it print better:
setattr(KernStats, "__repr__", lambda self:\
        f"\"{self.chr}\": lmean={self.lmean:.4f}, lmed={self.lmed:.4f}, "\
        f"lstdev={self.lstdev:.4f}, lmax={self.lmax:.4f};\n     "\
        f"rmean={self.rmean:.4f}, rmed={self.rmed:.4f}, rstdev={self.rstdev:.4f}, " \
        f"rmax={self.rmax:.4f}")


try:
    font = ImageFont.truetype(args.font, args.size[0])
except OSError:
    sys.exit("Failed to load font. Perhaps the name is wrong?")


# takes in two characters and compute the sum of their kerning, left and right
def get_kern(ch1, ch2, font):
    l_kern_l = font.getlength(ch2 + ch1)
    l_nkern_l = font.getlength(ch2 + ch1, features=['-kern'])
    r_kern_l = font.getlength(ch1 + ch2)
    r_nkern_l = font.getlength(ch1 + ch2, features=['-kern'])
    return (abs(l_kern_l - l_nkern_l), abs(r_kern_l - r_nkern_l))

if len(args.include) % 2 == 1:
    sys.exit('Error: Odd number of --include arguments. Exiting ...')
else:
    for i in range(0, len(args.include)//2):
        args.include[i] = [args.include[i], args.include[i+1]]
        args.include[i].sort()
        args.include[i] = tuple(args.include[i]) # Format into groups
        del(args.include[i+1])
    print('Character read successfully.')


for elem in args.include: # Iterate all characters non-repetitively
    for i in range(elem[0], elem[1]+1):
        if chr(i) not in chr_lst:
            chr_lst.append(chr(i))

for ch1 in chr_lst:
    # List of kerning val relative to all characters in chr_lst
    tmp_lst = \
            [get_kern(str(ch1), str(ch2), font) for ch2 in chr_lst]
    l_lst, r_lst = [val[0] for val in tmp_lst], [val[1] for val in tmp_lst]
    kern_lst.append(KernStats(ch1,
                              mean(l_lst), median(l_lst), stdev(l_lst), max(l_lst),
                              mean(r_lst), median(r_lst), stdev(r_lst), max(r_lst)))



sortword_lst = ['LMEAN', 'LMED', 'LSTDEV', 'LMAX', 'RMEAN', 'RMED', 'RSTDEV', 'RMAX']
sortfunc_lst = \
        [lambda x: x.lmean, lambda x: x.lmed, lambda x: x.lstdev, lambda x: x.lmax,
         lambda x: x.rmean, lambda x: x.rmed, lambda x: x.rstdev, lambda x: x.rmax]

printcnt = min(args.count[0], len(kern_lst)) 
for sortword in sortword_lst:
    if sortword in args.rankby:
        print('======= Sorting by ' + sortword + ': =======')
        kern_lst.sort(key=sortfunc_lst[sortword_lst.index(sortword)],\
                reverse=args.descending)
        for elem in kern_lst[0:printcnt]:
            print(repr(elem))
