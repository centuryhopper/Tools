### Make it executable:

- chmod +x create_project.py

### Create a new project:

- ./create_project.py new MyOpenCVApp

#### This generates:

MyOpenCVApp/
├── CMakeLists.txt
├── project_config.json
└── src/main.cpp

### Add an extra package (for example, Eigen3):

- ./create_project.py add Eigen3
- ./create_project.py new MyOpenCVApp # regenerate CMakeLists with packages

### Remove a package:

- ./create_project.py remove Eigen3
- ./create_project.py new MyOpenCVApp
