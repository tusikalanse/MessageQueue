#!/bin/bash


# producerCount, messageNumberPerProducer, messageLenMin, messageLenMax, topicNumber,

for ((i=1; i<=$1; i++)) 
do  
./testproducer $2 $3 $4 $5 &
# sleep 0.1
done  

wait 