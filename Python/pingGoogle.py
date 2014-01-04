#!/usr/bin/env python
# check internet access by pinging google
import urllib2

def internet_on():
    try:
        response = urllib2.urlopen('http://www.google.com', timeout=1)
        return 1
    except urllib2.URLError as err: pass
    return 0

if __name__ == '__main__':
    print(internet_on())

