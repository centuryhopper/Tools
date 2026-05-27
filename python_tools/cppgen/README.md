Install on system pip with "pip install -e."

cppgen.py [PROJECT_NAME]

cd PROJECT_NAME/build

cmake .. && cmake --build .

run ./app to see the print statement
run ./tests to see whether the tests have passed

### cd PROJECT_NAME

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
