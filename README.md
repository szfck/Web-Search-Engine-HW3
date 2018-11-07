### Build

```sh
mkdir output
mkdir output/intermediate-output-1
mkdir output/intermediate-output-2
mkdir output/intermediate-output-3

cd extract
# download wet files
python download.py
# extract wet file info to intermediate-output-1
python extract_text.py
cd ..

mkdir build
cd build
cmake ..
make
# build index for files in [0, 100) to intermediate-output-2
./build_index 0 100
# merge [0, 100) indexes to 1 index to intermediate-output-3
./merge 1 0 100 0
cd ..

# add file to communicate with web page
touch in.txt out.txt
```

### Run C++ Engine 
```sh
cd build
./main
```

### Run Web Server (Node Express)
```sh
cd frontend/server
npm i
npm run start
```

### Run Web Frontend (Angular)
```sh
cd frontend
npm i
ng serve --open
```