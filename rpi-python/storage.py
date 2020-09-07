import json
import time

cache = {}

filename = "/home/pi/log/eclock_cache.json"


def load_cache():
    global cache
    try:
        # TODO: Have a mechanism to purge stale cache entries.
        with open(filename) as infile:
            cache = json.load(infile)
            print("Read cache from", infile.name)
    except OSError as err:
        print("OS error: {0}".format(err))


def save_cache():
    global cache
    with open(filename, 'w') as outfile:
        json.dump(cache, outfile)
        print("Wrote cache to", outfile.name)
