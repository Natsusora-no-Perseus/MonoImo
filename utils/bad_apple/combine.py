import os

# Define the input and output file names
input_files = ['f_{:05d}.xbm'.format(i) for i in range(1, 5423)]
output_file = 'out_seq.h'

# Open the output file for writing
with open(output_file, 'w') as f:
    # Write the header information
    f.write("#define BADAPPLE_W 80\n")
    f.write("#define BADAPPLE_H 60\n")
    f.write("#define FRAME_CNT 5423\n")
    f.write('// Bitmap array\n')
    f.write('static unsigned char badapple[FRAME_CNT][600] = {\n')

    # Loop over the input files and write their contents to the output file
    for input_file in input_files:
        with open(input_file, 'r') as g:
            # Read the part in curly braces from the input file
            bitmap_data = g.read()
            #f.write('{')
            start_idx = bitmap_data.find('{')
            end_idx = bitmap_data.rfind('}')
            f.write(bitmap_data[start_idx:end_idx])
            f.write("},\n")

            # Write the bitmap data to the output file

    # Write the footer information
    f.write('};\n\n')
