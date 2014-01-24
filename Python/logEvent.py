#!/usr/bin/env python
# log event for arduino yun RouterReset

from datetime import datetime, timedelta
import os
import sys
import updateWeb

# function to log time/event
def logEvent (directory):
    now = datetime.today()

    # command-line argument: status (startup/up/down)
    status = sys.argv[1] if len(sys.argv)>1 else 'startup'

    logfile = os.path.join(directory, now.strftime('log_%Y-%m.txt'))
    lastlog_file = os.path.join(directory, '.lastlog.txt')
    lastlog_status = status # default: no change

    if os.path.exists(lastlog_file):
        lastlog = open(lastlog_file).readlines()
        lastlog_time = datetime.strptime(lastlog[0].strip(), '%Y-%m-%d %H:%M:%S')
        lastlog_status = lastlog[1].strip()

    # does log file exist?
    logfile_not_new = os.path.exists(logfile)

    # add to log
    with open(logfile, 'at') as f:
        output = '%-10s %-8s %-7s' % (now.strftime('%Y-%m-%d'),
                                    now.strftime('%H:%M:%S'), status)
        if status == "startup":
            output += '; connected to ' + getSSID()

        if (status != 'startup' and status != lastlog_status):
            output += (' (was %-4s ' % lastlog_status +
                       strfdelta(now - lastlog_time)) + ')'
        f.write(output + '\n')

    # update 'last log'
    if status != 'startup' and (not os.path.exists(lastlog_file) or status != lastlog_status):
        with open(lastlog_file, 'wt') as f:
            f.write(now.strftime('%Y-%m-%d %H:%M:%S') + '\n')
            f.write(status + '\n')
    if status == 'startup' and os.path.exists(lastlog_file):
        os.remove(lastlog_file)

    # if it was a new log file, re-build web page:
    if not logfile_not_new:
        updateWeb.updateWeb(directory)


# function for formatting change in time
def strfdelta(tdelta):
    hours, rem = divmod(tdelta.seconds, 3600)
    hours += tdelta.days * 24
    minutes, seconds = divmod(rem, 60)
    return("%d:%02d:%02d" % (hours, minutes, seconds))

# function to get SSID
def getSSID():
    return os.popen('iwconfig wlan0 | grep ESSID').readline().strip().split('"')[-2]

if __name__ == '__main__':
    logEvent('/mnt/sd/arduino/www')
