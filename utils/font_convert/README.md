# Font Convert Tools
## Description
`f2d_convert.py`: Converts a TrueType (`.ttf`) or OpenType (`.otf`) font to a bitmap font.
- Run `python3 f2d_convert.py -h` for more detailed help message
- (Multiple) ranges of characters in Unicode encoding can be specified with `-i` option
- Files containing characters to be converted can also be included with `-I` option
- Example:
    - `python3 f2d_convert.py NotoSerif-Black.ttf -i 65 90 18873 18887 -I ch_src.txt -s 24 -d 0.5 -r LANCZOS -t 155 -k B -f BMP -o out -vv` does the following:
    - Include characters with Unicode encoding 65~90 and 18873~18887 and characters contained in `ch_src.txt`;
    - Converts the glyphs displayed with 24pt NotoSerif-Black.ttf font to bitmap, then shrinks them by a factor of 0.5, using LANCZOS resampling;
    - Calculate kerning for the output bitmap font using 'B' as a reference character that almost does not kern with other characters;
    - Output the results in BMP form, and name them as 'out<Unicode>.bmp';
    - Print level 3 (2+1) information during the process.
`find_kref.py`: Helps finding a character that kerns the least with other characters, for use in the -k option.
- Run `python3 find_kref.py -h` for help information
## Additional Information
- Try using the complete path (e.g. `/usr/share/fonts/truetype/noto/NotoSerif-Black.ttf`) if `f2d-convert.py` fails to find a font.
- Please find out if a certain glyph is included in a font before including it for conversion. There are (not yet) any checking for this.
