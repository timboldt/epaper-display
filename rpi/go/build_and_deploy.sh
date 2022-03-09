#!/bin/bash
env GOARCH=arm GOOS=linux go build .
scp eclock pzw1:/home/pi
ssh pzw1 "/home/pi/eclock"