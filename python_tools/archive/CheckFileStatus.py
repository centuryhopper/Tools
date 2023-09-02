
import sys
import os
from time import sleep
from pathlib import Path


path = sys.argv[1]

MAX_NUM_FILES= 20

try:
    MAX_NUM_FILES = int(sys.argv[2])
except:
    pass
    #raise Exception("invalid MAX_NUM_FILES parameter")

if not os.path.exists(path):
    raise Exception("The path you provided does not exist or is invalid.")
if not 'screenshots' in path:
    raise Exception("You should really only use this file on screenshots")


while True:
    #print(os.stat(path).st_mtime)
    if len(os.listdir(path)) > MAX_NUM_FILES:
        for f in Path(path).iterdir():
            if f.is_file():
                f.unlink()

    sleep(2)

