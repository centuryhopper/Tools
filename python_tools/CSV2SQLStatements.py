import csv
import os
import sys
import time
from pathlib import Path

### POSTGRESQL syntax ###

os.chdir(os.path.dirname(__file__))
PATH_TO_CSV = sys.argv[1]

OUTPUT_FOLDER_NAME = 'generated_postgresql_statements'
OUTPUT_DIR = Path.cwd() / f'{OUTPUT_FOLDER_NAME}/'
OUTPUT_DIR.mkdir(exist_ok=True)

reader = None

# Open the CSV file
with open(PATH_TO_CSV, 'r') as csvfile:
    reader = csv.reader(csvfile)

    # get filename from *.csv file
    filename = os.path.basename(PATH_TO_CSV).split('.')[0]

    with open(f'{os.getcwd()}/{OUTPUT_FOLDER_NAME}/{time.strftime("%Y_%m_%d_%H_%M_%S")}.txt', 'w') as output:

        columns = next(reader)
        columns = [col for col in columns]
        columns_with_types = [col+' varchar(256)' for col in columns]
        print(columns)

        N = len(columns)

        column_names_with_types = ', '.join(columns_with_types)
        column_names = ', '.join(columns)

        output.write(f'''CREATE TABLE IF NOT EXISTS {filename}
                     ({column_names_with_types});\n''')

        # Loop over each row in the CSV file and generate SQL insert statement
        for row in reader:
            # escape the single and double quotes
            values = [row[i].replace("\'","\'\'").replace('\"', '\"\"') for i in range(N)]
            values = [f"'{value}'" for value in values]
            values = ', '.join(values)
            output.write(f"INSERT INTO {filename} ({column_names}) VALUES ({values});\n")



'''
-- select * from trip_advisor_orlando_2023_02_25_23_28_57
select distinct title, review_url, address from trip_advisor_orlando_2023_02_25_23_28_57
where address LIKE ANY (ARRAY['%Florida%', '%Orlando%', '%Kissimmee%', '%Clermont%', '%Oviedo%']);

-- SELECT distinct title, review_url, COUNT(review_url) as num_duplicates
-- FROM trip_advisor_orlando_2023_02_25_23_28_57
-- GROUP BY title, review_url
-- HAVING COUNT(review_url) > 1;
'''