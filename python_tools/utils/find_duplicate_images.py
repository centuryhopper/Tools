#! /home/leo_zhang/miniconda3/envs/web_auto/bin/python


import os
from PIL import Image, ImageStat
import time
import sys


start = time.perf_counter()
image_folder = ""
try:
    image_folder = sys.argv[1]
    if not os.path.exists(image_folder):
        raise Exception("The path you provided does not exist")
except Exception as e:
    print(e)
    sys.exit(1)
image_files = [
    _
    for _ in os.listdir(image_folder)
    if _.lower().endswith("jpg") or _.lower().endswith("png")
]


duplicate_files = []

# O(n^2) runtime
for file_org in image_files:
    if file_org not in duplicate_files:
        image_org = Image.open(os.path.join(image_folder, file_org))
        pix_mean1 = ImageStat.Stat(image_org).mean
        for file_check in image_files:
            if file_check != file_org:
                image_check = Image.open(os.path.join(image_folder, file_check))
                pix_mean2 = ImageStat.Stat(image_check).mean

                if pix_mean1 == pix_mean2:
                    duplicate_files.append(file_org)
                    duplicate_files.append(file_check)
        print(duplicate_files)
end = time.perf_counter()
print(f"total execution time: {(end - start):.1f} seconds")
print(duplicate_files)


"""
./test/ took 2242.2 seconds 

duplicates found:

['wx_camera_1508721265523.jpg', 'wx_camera_1508721265523(1).jpg', 'mmexport1471113634580.jpg', 'mmexport1471113634580(1).jpg', 'mmexport1471113631382(1).jpg', 'mmexport1471113631382.jpg', 'IMG_20171101_203731.jpg', 'IMG_20171101_203731(1).jpg', 'mmexport1471229597243.jpg', 'mmexport1471229597243(1).jpg', 'mmexport1471229524740(1).jpg', 'mmexport1471229524740.jpg']

"""
