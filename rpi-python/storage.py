#  Copyright 2020 Google LLC
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https:#www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

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
