#!/bin/python3

import os
import subprocess
import datetime
import argparse
from multiprocessing.pool import ThreadPool


sources = ["camellia_swi_default", 
           "aes_swi_small", 
           "cast5_swi_default", 
           "des_swi_default", 
           "hight_swi_default"]

depends = ["modes_of_operation"]


compile_command = "ubertime acompile_mon_multisrc"

src_folder = "./src_cl/"
bin_folder = "./bin_cl/"
datefmt = "%Y-%m-%d %H:%M:%S"



def make_binary_path(source_item):
    return os.path.join(bin_folder, source_item + ".aocx")

def make_cl_path(source_item):
    return os.path.join(src_folder, source_item + ".cl")


def is_update_required(source_item, depend_list, verbose=False):
    item_binary_mtime = os.path.getmtime(make_binary_path(source_item))
    
    build_sources = [make_cl_path(source_item)] + list(map(make_cl_path, depend_list))
    depends_with_mtimes = map(lambda src: (src, os.path.getmtime(src)), 
                              build_sources)
    
    triggers = list(filter(lambda dep_w_mtime: dep_w_mtime[1] > item_binary_mtime, 
                           depends_with_mtimes))
    
    if verbose and len(triggers) > 0:
        print("Updating {:<32} built on: {}".format(source_item, 
                                                    datetime.datetime.fromtimestamp(item_binary_mtime).strftime(datefmt)))
        for trigger in triggers:
            print("+-source {:<30} updated on: {}".format(trigger[0], 
                                                          datetime.datetime.fromtimestamp(trigger[1]).strftime(datefmt)))
            
    return len(triggers) > 0


def is_new_build_required(source_item, verbose=False):
    if not os.path.isfile(make_binary_path(source_item)):
        if verbose:
            print("Building {:<32} ex-novo".format(source_item))
        return True
    return False


def do_build_item(source_item):
    return subprocess.call("%s \"%s\"" % (compile_command, make_cl_path(source_item)), shell=True)



def parsecli():
    parser = argparse.ArgumentParser(description="Build/update multiple aocx binaries")
    parser.add_argument("-u", "--update", help="Update mode", action="store_true")
    parser.add_argument("-f", "--force", help="Skip modification time check and rebuild all", action="store_true")
    parser.add_argument("-v", "--verbose", help="Verbose mode", action="store_true")
    parser.add_argument("-t", "--threads", help="Run multiple compiles in parallel", type=int, default=1)
    parser.add_argument("--dry-run", help="Don't actually compile anything", action="store_true")
    parser.add_argument("source", help="Specific source to build, accept comma-separated list of multiple sources", type=str, default="None", nargs='?')
    return parser.parse_args()



if __name__ == "__main__":
    cli = parsecli()
    
    if cli.source != "None":
        buildlist = cli.source.split(",")
    else:
        buildlist = sources
        
    exec_buildlist = []
    
    for build_item in buildlist:
        if not os.path.isfile(make_cl_path(build_item)):
            print("Source item {} not found in path {}".format(build_item, make_cl_path(build_item)))
            continue
        
        if cli.force:
            if verbose:
                print("Reuilding {}".format(build_item))
            exec_buildlist.append(build_item)
        else:
            if is_new_build_required(build_item, verbose=cli.verbose):
                exec_buildlist.append(build_item)
            elif is_update_required(build_item, depends, verbose=cli.verbose):
                exec_buildlist.append(build_item)
                
    if not cli.dry_run:
        with ThreadPool(cli.threads) as p:
            return_statuses = p.map(do_build_item, exec_buildlist)
        
        nsuccess = len(list(filter(lambda rs: rs == 0, return_statuses)))
        print("Done compiling; %d success, %d failed." % (nsuccess, len(return_statuses)-nsuccess))
    

