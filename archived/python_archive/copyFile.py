import sys
from utils.FileOrganizer import FileUtils

FileUtils.copyOverFile(sys.argv[1], sys.argv[2])
print('copied file over')
