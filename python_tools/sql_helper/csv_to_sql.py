import pandas as pd

FILE_NAME = r"./2025 DRM Data_Updated Zone_Habitat_Chris.csv"
df = pd.read_csv(FILE_NAME)  # Reads the CSV file into a dataframe
# table_name = "EAR_CoralDRM_MI"
table_name = "CoralDRM"

from datetime import datetime

now = datetime.now()
formatted = now.strftime("%Y_%m_%d_%H_%M_%S")
with open(f"insert_statements_{formatted}.sql", "w", encoding="utf-8") as f:
    for index, row in df.iterrows():
        columns = ", ".join(df.columns)

        values = ", ".join(["'{}'".format(str(v).replace("'", "''")) for v in row])

        sql = f"INSERT INTO {table_name} ({columns}) VALUES ({values});\n"

        f.write(sql)
