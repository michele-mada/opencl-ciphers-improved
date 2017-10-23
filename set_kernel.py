#!/bin/python

import os
import argparse


source_folder = "./src_cl"
bin_folder = "./bin_cl"


def parsecli():
    parser = argparse.ArgumentParser(description="Setup symlink to easily switch opencl kernel variant")
    parser.add_argument('algo', metavar='algo', help='Algorithm family to target', type=str)
    parser.add_argument('variant', metavar='variant', help='Kernel variant to target', type=str)
    return parser.parse_args()


def make_path_target(folder, algo, variant, ext):
    return os.path.join(folder, 
                        "%s_swi_%s.%s" % (algo, variant, ext))

def make_path_link(folder, algo, ext):
    return os.path.join(folder, 
                        "%s_swi.%s" % (algo, ext))


if __name__ == "__main__":
    cli = parsecli()
    if os.path.isdir(source_folder):
        link_path = make_path_link(source_folder, cli.algo, "cl")
        if os.path.isfile(link_path): os.remove(link_path)
        target_path = make_path_target("./", cli.algo, cli.variant, "cl")
        os.symlink(target_path, link_path)
        print("%s -> %s" % (link_path, target_path))
    if os.path.isdir(bin_folder):
        link_path = make_path_link(bin_folder, cli.algo, "aocx")
        if os.path.isfile(link_path): os.remove(link_path)
        target_path = make_path_target("./", cli.algo, cli.variant, "aocx")
        os.symlink(target_path, link_path)
        print("%s -> %s" % (link_path, target_path))
