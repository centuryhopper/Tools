import pandas as pd
import sys

FILE_NAME = rf"./{sys.argv[1]}.csv"
df = pd.read_csv(FILE_NAME)  # Reads the CSV file into a dataframe
# table_name = "EAR_CoralDRM_MI"
table_name = sys.argv[2]

from datetime import datetime

now = datetime.now()
formatted = now.strftime("%Y_%m_%d_%H_%M_%S")
with open(f"insert_statements_{formatted}.sql", "w", encoding="utf-8") as f:
    for index, row in df.iterrows():
        columns = ", ".join(df.columns)

        values = ", ".join(["'{}'".format(str(v).replace("'", "''")) for v in row])

        sql = f"INSERT INTO {table_name} ({columns}) VALUES ({values});\n"

        f.write(sql)
