#!/bin/bash


# producerCount, messageNumberPerProducer, messageLenMin, messageLenMax, topicNumber, consumerNumber, subscriptionsPerConsumer
# consumerMessage = topicNumber / subscriptionsPerConsumer * producerCount * messageNumberPerProducer;
let consumerMessage=$7*$1*$2/$5
echo $consumerMessage
for ((i=1; i<=$6; i++)) 
do  
./testconsumer $5 $7 $consumerMessage &
# sleep 0.1
done  

sleep 0.1
# echo $1
for ((i=1; i<=$1; i++)) 
do  
./testproducer $2 $3 $4 $5 &
# sleep 0.1
done  

wait 