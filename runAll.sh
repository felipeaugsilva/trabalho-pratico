#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: <pasta_saida> <pasta_entrada>"
    exit 1
fi

saida=$1
entrada=$2

if [ ! -d $saida ]; then
    mkdir $saida
fi

echo "small-0"
./heur -t 120 -o ${saida}/small-0 ${entrada}/small-0-40-100-7.in
echo "small-1"
./heur -t 120 -o ${saida}/small-1 ${entrada}/small-1-40-100-5.in
echo "small-2"
./heur -t 120 -o ${saida}/small-2 ${entrada}/small-2-40-100-4.in
echo "small-3"
./heur -t 120 -o ${saida}/small-3 ${entrada}/small-3-40-100-8.in
echo "small-4"
./heur -t 120 -o ${saida}/small-4 ${entrada}/small-4-40-100-2.in

echo "medium-0"
./heur -t 120 -o ${saida}/medium-0 ${entrada}/medium-0-100-1000-5.in
echo "medium-1"
./heur -t 120 -o ${saida}/medium-1 ${entrada}/medium-1-100-1000-9.in
echo "medium-2"
./heur -t 120 -o ${saida}/medium-2 ${entrada}/medium-2-100-1000-0.in
echo "medium-3"
./heur -t 120 -o ${saida}/medium-3 ${entrada}/medium-3-100-1000-1.in
echo "medium-4"
./heur -t 120 -o ${saida}/medium-4 ${entrada}/medium-4-100-1000-0.in
echo "medium-5"
./heur -t 120 -o ${saida}/medium-5 ${entrada}/medium-5-100-1000-2.in
echo "medium-6"
./heur -t 120 -o ${saida}/medium-6 ${entrada}/medium-6-100-1000-8.in
echo "medium-7"
./heur -t 120 -o ${saida}/medium-7 ${entrada}/medium-7-100-1000-10.in

echo "big-0"
./heur -t 120 -o ${saida}/big-0 ${entrada}/big-0-300-1000-10.in
echo "big-1"
./heur -t 120 -o ${saida}/big-1 ${entrada}/big-1-300-1000-1.in
