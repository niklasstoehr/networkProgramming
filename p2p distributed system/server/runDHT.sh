#!/bin/bash

./node "localhost" "6000" "30000" &
(sleep 1 && ./node "localhost" "6001" "5" "localhost" "6000") &
(sleep 1 && ./node "localhost" "6002" "55000" "localhost" "6001") &
(sleep 1 && ./node "localhost" "6003" "20000" "localhost" "6000")
