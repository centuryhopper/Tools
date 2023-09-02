import asyncio
import asyncpg
import os
import urllib.parse as up
from typing import List,Dict

from dotenv import load_dotenv, dotenv_values

load_dotenv()

env_vars = dotenv_values(".env")

'''
-connect to database
-get table fields and their datatypes
-create c# classes with them
'''


os.chdir(os.path.dirname(__file__))

up.uses_netloc.append("postgres")
url = up.urlparse(env_vars["DATABASE_URL"]) # Default PostgreSQL port is 5432

datatype_mapper = {
        'text':'string',
        'int': 'int',
        'varchar':'string',
        'bit': 'bool',
        }


async def main():
    table_names = ["passwordmanager_accounts"]
    for table_name in table_names:
        await get_data_from_postgres(table_name)

async def get_data_from_postgres(table_name: str):
    # Establish a database connection
    conn = await asyncpg.connect(
        database=url.path[1:],
        user=url.username,
        password=url.password,
        host=url.hostname,
        port=url.port
    )
    
    # Query to retrieve all rows from the table
    query = '''SELECT column_name, data_type
            FROM information_schema.columns
            WHERE table_name = '%s';
            ''' % table_name

    # Execute the query
    rows = await conn.fetch(query)

    # Close the database connection
    await conn.close()

    # Store data in a list of dictionaries
    data_list = []
    for row in rows:
        row_dict = {col: row[col] for col in row.keys()}
        data_list.append(row_dict)

    generate_csharp_class(table_name, data_list)

    # Print the retrieved data
    #for item in data_list:
    #    print(item)

def generate_csharp_class(table_name:str, lstOfDicts:List[Dict[str,str]]):
    csharp_code = f"public class {table_name}\n"
    csharp_code += "{\n"
    for column in lstOfDicts:
        column_name = column['column_name']
        data_type = column['data_type']
        csharp_code += "    public %s %s { get; set;}\n" % (datatype_mapper[data_type.lower()], column_name) 
    csharp_code += "}\n"

    file_name = "{}.cs".format(table_name)

    with open(os.path.join(os.getcwd(), "generated_csharp_models", file_name), "w") as file:
        file.write(csharp_code)

    print(f"C# class saved to {file_name}")



# Run the asynchronous code
loop = asyncio.get_event_loop()
loop.run_until_complete(main())


