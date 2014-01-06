#!/usr/bin/env python
# log event for arduino yun RouterReset

# for dealing with dates and times
from datetime import datetime, timedelta

# for checking if a file exists
import os

# for dealing with command-line arguments
import sys

# function for formatting change in time
def strfdelta(tdelta):
    hours, rem = divmod(tdelta.seconds, 3600)
    hours += tdelta.days * 24
    minutes, seconds = divmod(rem, 60)
    return("%d:%02d:%02d" % (hours, minutes, seconds))

directory = '/mnt/sd/arduino/www/'

# current time
now = datetime.today()

# command-line arguments: status (startup/on/off) and logdiff
status = sys.argv[1] if len(sys.argv)>1 else ''
logdiff = sys.argv[2] != '' if len(sys.argv)>2 else False

logfile = os.path.join(directory, now.strftime('log_%Y-%m.txt'))
lastlog_file = os.path.join(directory, '.lastlog.txt')
lastlog_status = '' # default: no last log

if logdiff and os.path.exists(lastlog_file):
    lastlog = open(lastlog_file).readlines()
    lastlog_time = datetime.strptime(lastlog[0].strip(), '%Y-%m-%d %H:%M:%S')
    lastlog_status = lastlog[1].strip()
else: # no previous log time to diff from
    logdiff = False

if logdiff and (status == 'startup' or status == lastlog_status):
    logdiff = False  # not really a change

# add to log
with open(logfile, 'at') as f:
    output = '%-10s %-8s %-7s' % (now.strftime('%Y-%m-%d'), 
                                 now.strftime('%H:%M:%S'), status)
    if logdiff:
        output += (' (after being %-3s ' % lastlog_status + 
                   strfdelta(now - lastlog_time)) + ')'

    f.write(output + '\n')

if status != 'startup' and (not os.path.exists(lastlog_file) or status != lastlog_status): # update 'last log'
  with open(lastlog_file, 'wt') as f:
      f.write(now.strftime('%Y-%m-%d %H:%M:%S') + '\n')
      f.write(status + '\n')

