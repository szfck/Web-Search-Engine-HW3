from glob import glob
import gzip
import os.path
import warc
import re

# List any of the WARC files found in the data folder
warc_files = glob('wet_files/*.wet.gz')

output_dir = '../output/intermediate-output-1/'

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# Process each of the WARC files we found
files_processed = 0
END = "==============="
ENDD = "szfck" + END + "szfck" + "@@@@@"
for fn in warc_files:
    pos = fn.rfind(".warc.wet.gz")
    file_id = fn[pos - 5: pos]
    file_path = output_dir + file_id + '.txt'
    if os.path.exists(file_path):
        continue
    file = open(file_path, 'w')

    for record in warc.WARCFile(fileobj=(gzip.open(fn))):
        url = record.header.get('WARC-Target-URI', None)
        page_size = record.header.get('Content-Length', None)
        if not url:
            continue
        if not page_size:
            continue
        if not re.match('^[1-9][0-9]*$', page_size):
            print ('page size : ' + page_size)
            continue
        if not re.match('http[s]?://(.*?)', url):
            print ('url : ' + url)
            continue
        text = record.payload.read()
        file.write(url + '\n')
        file.write(text + '\n')
        file.write(ENDD + '\n')
        file.write(str(page_size) + '\n')
