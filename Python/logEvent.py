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

# command-line argument: status (startup/on/off)
status = sys.argv[1] if len(sys.argv)>1 else 'startup'

logfile = os.path.join(directory, now.strftime('log_%Y-%m.txt'))
lastlog_file = os.path.join(directory, '.lastlog.txt')
lastlog_status = status # default: no change

if os.path.exists(lastlog_file):
    lastlog = open(lastlog_file).readlines()
    lastlog_time = datetime.strptime(lastlog[0].strip(), '%Y-%m-%d %H:%M:%S')
    lastlog_status = lastlog[1].strip()

# add to log
with open(logfile, 'at') as f:
    output = '%-10s %-8s %-7s' % (now.strftime('%Y-%m-%d'), 
                                 now.strftime('%H:%M:%S'), status)
    if (status != 'startup' and status != lastlog_status):
        output += (' (was %-3s ' % lastlog_status +
                   strfdelta(now - lastlog_time)) + ')'

    f.write(output + '\n')

if status != 'startup' and (not os.path.exists(lastlog_file) or status != lastlog_status): # update 'last log'
  with open(lastlog_file, 'wt') as f:
      f.write(now.strftime('%Y-%m-%d %H:%M:%S') + '\n')
      f.write(status + '\n')

