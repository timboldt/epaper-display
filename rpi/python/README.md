## Raspberry Pi Python Version

This version uses a Raspberry Pi (specifically, a RPi Zero W) to run the code. There is an Adafruit Featherwing socket on top of a RPi protoboard hat.

Crontab is set up as follows:
```
# m h  dom mon dow   command
*/3 * * * * sh /home/pi/bin/eclock.sh >> /home/pi/log/eclock.log
```

The `~/log` directory is also the location of the cache file which is updated on each run.

The contents of `~/bin/eclock.sh` are simply:
```
date
echo "Running..."
/home/pi/workspace/epaper-display/rpi-python/eclock.py
```
