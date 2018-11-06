### Build

```$xslt
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

# add file to communicate with web
touch in.txt out.txt
```

### Run 
```$xslt
cd build
./main
```