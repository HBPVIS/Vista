#!/bin/bash

./startslaves_newcave.sh HoloSpace $@

sleep 5

./master_newcave.sh HoloSpace $@

sleep 1

./kill.sh