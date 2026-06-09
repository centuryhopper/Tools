import pandas as pd
import sys
from pathlib import Path

# Directory containing this script
SCRIPT_DIR = Path(__file__).resolve().parent

# Ensure we're working relative to the script location
OUTPUT = SCRIPT_DIR / "output"
OUTPUT.mkdir(exist_ok=True)

# Optional: make the script's directory the current working directory
import os
os.chdir(SCRIPT_DIR)

FILE_NAME = rf"csv_files/{sys.argv[1]}.csv"
df = pd.read_csv(FILE_NAME)  # Reads the CSV file into a dataframe
# table_name = "EAR_CoralDRM_MI"
table_name = sys.argv[2]

from datetime import datetime

now = datetime.now()
formatted = now.strftime("%Y_%m_%d_%H_%M_%S")
with open(f"{OUTPUT}/insert_statements_{formatted}.sql", "w", encoding="utf-8") as f:
    for index, row in df.iterrows():
        columns = ", ".join(df.columns)

        values = ", ".join(["'{}'".format(str(v).replace("'", "''")) for v in row])

        sql = f"INSERT INTO {table_name} ({columns}) VALUES ({values});\n"

        f.write(sql)
