### Build

```$xslt
mkdir output
mkdir output/intermediate-output-1
mkdir output/intermediate-output-2
mkdir output/intermediate-output-3

cd extract
python download.py
python extract_text.py
cd ..

mkdir build
cd build
cmake ..
make
./build_index 0 100
./merge 1 0 100 0
cd ..
```

### Run 
```$xslt
cd build
./main
```