#!/bin/python

import imageio
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage : filename tile_size"
        exit();
    im = imageio.imread(sys.argv[1])
    size = int(sys.argv[2])
    nb_image_x = im.shape[0] / size;
    nb_image_y = im.shape[1] / size;
    tex_id = 0;
    moy = [0, 0, 0, 0]
    for y in  range(0, nb_image_y):
        for x in  range(0, nb_image_x):
            for i in  range(0, size):
                for j in  range(0, size):
                    moy += im[x * size + i][y * size + j]
            moy = moy / [size * size, size * size, size * size, size * size];
            print str(tex_id) + "{" + str(moy[0]) + "," + str(moy[1]) + ","+ str(moy[2]) + ","+ str(moy[3]) + "},"
            moy = [0, 0, 0, 0]
            tex_id += 1;

