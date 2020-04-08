#!/bin/bash
mkdir -p 8091/Lekhnovskiy
cd 8091/Lekhnovskiy
date > alexandr.txt
date --date="next mon" > filedate.txt
cat alexandr.txt filedate.txt > result.txt
cat result.txt
