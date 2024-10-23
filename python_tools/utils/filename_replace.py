#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python


import os
import sys
import datetime


if __name__ == "__main__":
    file_path = ""
    pattern_to_remove = ""
    try:
        file_path = sys.argv[1]
    except Exception as e:
        print("Please pass in a directory you want to check for")
        sys.exit(1)
    try:
        pattern_to_remove = sys.argv[2]
    except Exception as e:
        print("Please pass in a pattern you want to remove")
        sys.exit(1)

    if not os.path.exists(file_path):
        raise Exception("path doesn't exist")

    if not any(pattern_to_remove in file for file in os.listdir(file_path)):
        raise Exception(
            "no images have been generated from the fawkes exe file. Please make sure images have been generated first"
        )

    for file in os.listdir(file_path):
        abs_path = os.path.join(file_path, file)
        if os.path.isdir(abs_path):
            continue
        if pattern_to_remove not in file:
            os.remove(abs_path)
            continue
        print(abs_path)

        # remove the pattern so that fawkes can process the image again
        file = file.replace(pattern_to_remove, "")
        new_abs_path = os.path.join(file_path, file)
        os.rename(abs_path, new_abs_path)
