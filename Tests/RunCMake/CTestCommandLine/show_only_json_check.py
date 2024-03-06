import sys
import json
import re

def is_bool(x):
    return isinstance(x, bool)

def is_dict(x):
    return isinstance(x, dict)

def is_list(x):
    return isinstance(x, list)

def is_int(x):
    if sys.version_info[0] >= 3:
        return isinstance(x, int)
    else:
        return isinstance(x, long)

def is_string(x):
    if sys.version_info[0] >= 3:
        return isinstance(x, str)
    else:
        return isinstance(x, unicode)

def check_re(x, regex):
    assert re.search(regex, x)

with open(sys.argv[1]) as f:
    ctest_json = json.load(f)
