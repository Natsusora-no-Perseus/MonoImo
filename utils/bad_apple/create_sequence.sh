#! /bin/bash
ffmpeg -i 3724723_da3-1-30032.mp4 -ss 00:00:00 -t 00:03:37 -r 25 -vf scale=80:60 -f image2 f_%05d.xbm
# `-ss`: Start time
# `-t`: Duration
# `-r`: Framerate
