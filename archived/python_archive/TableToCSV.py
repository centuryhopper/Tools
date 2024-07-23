import time
import requests
from bs4 import BeautifulSoup
import pandas as pd
import os

os.chdir(os.path.dirname(__file__))

url = 'https://github.com/coderQuad/New-Grad-Positions-2023'

r = requests.get(url)
# print(r.status_code)
soup = BeautifulSoup(r.text, 'html.parser')

desiredTable = soup.find('table', {'class': None})

# print(desiredTable.text)

thead = desiredTable.find('thead')
# print(thead)

columnNames = [name.text for name in thead.find_all('th')]
columnNames.extend(['url', 'isVoid'])
# print(columnNames)

lookup = {columnName:[] for columnName in columnNames}

for tbody in desiredTable.find_all('tbody'):
    rows = tbody.find_all('tr')
    for x,row in enumerate(rows):
        tds = row.find_all('td')
        hasDel = False
        atag = ''
        for i, td in enumerate(tds):
            # print(td.text.strip(), end=' ')
            lookup[columnNames[i]].append(td.text.strip())
            tmp = td.find('a')
            if tmp:
                atag = tmp
            if td.find('del'):
                hasDel = True
        lookup['isVoid'].append(hasDel)
        if atag:
                lookup['url'].append(atag['href'].strip())
        else:
            lookup['url'].append('n/a')

csvFolderPath = os.getcwd() + '/output/'
if not os.path.exists(csvFolderPath):
    os.mkdir(csvFolderPath)
fileName = time.strftime('%Y-%m-%d-%H:%M:%S')
for k,v in lookup.items():
    print(len(v))
df = pd.DataFrame(lookup)
df.to_csv(f'{csvFolderPath}{fileName}.csv')
