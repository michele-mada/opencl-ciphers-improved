import re
import sys

"""
    Input file.c
    Replace all the array-based lookup tables with the
    equivalent switch-case construct
"""

re_findlut = re.compile("^(?P<type>[^\\n]+) +(?P<name>[^\\n]+)\[(?P<size>[0-9]+)\] *= *\{(?P<content>[^\}]+)\}", re.MULTILINE|re.IGNORECASE);

fname = sys.argv[1]
lut_index_type = "uchar"



def replacelut(matchobj):
    array_elements = filter(lambda e: len(e) > 0, 
                            map(lambda s: s.strip(), 
                                matchobj.group("content").split(",")))
    
    ret = "%s %s(%s index) {\n" % (matchobj.group("type"), matchobj.group("name"), lut_index_type)
    ret += "    switch (index) {\n"
    for i,element in enumerate(array_elements):
        ret += "        case %d: return %s;\n" % (i, element)
    ret += "    }\n}\n"
    
    return ret



with open(fname, "r") as fp:
    text = fp.read()
    print(re_findlut.sub(replacelut, text))
    
