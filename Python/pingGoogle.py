#!/usr/bin/env python
# check internet access by pinging google

# taken from a stackoverflow answer by unutbu:
#   http://stackoverflow.com/questions/3764291/checking-network-connection

import urllib2

def internet_on():
    try:
        response = urllib2.urlopen('http://www.google.com', timeout=5)
        return 1
    except urllib2.URLError as err: pass
    return 0

if __name__ == '__main__':
    print(internet_on())

