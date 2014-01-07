#!/usr/bin/env python
# update web page with links to log files

import os
import re

# function to update web
def updateWeb (directory):

    # grab head and tail from index.html
    webfile = os.path.join(directory, 'index.html')
    f = open(webfile)
    webhead = ''
    webtail = ''
    head = True
    tail = False
    for line in f.readlines():
        if re.search(r'</ul>', line):
            tail = True
        if head:
            webhead += line
        if tail:
            webtail += line
        if re.search(r'<ul>', line):
            head = False
    f.close()
            
    # re-write the head
    f = open(webfile, 'wt')
    f.write(webhead)

    # write list of log files
    files = [file for file in os.listdir(directory) if file.startswith('log_') and
             file.endswith('.txt')]
    for file in files:
        f.write('<li><a href="' + file + '">' + grab_date(file) + ' log file</a></li>\n')

    # re-write the tail
    f.write(webtail)

def grab_date (filename):
    return re.findall(r'\d+-\d+', filename)[0]

if __name__ == '__main__':
    updateWeb('/mnt/sd/arduino/www')
