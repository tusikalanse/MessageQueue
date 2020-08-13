
# topicNumber, consumerNumber, subscriptionsPerConsumer, consumerMessage

for ((i=1; i<=$2; i++)) 
do  
./testconsumer $1 $3 $4 &
# sleep 0.1
done  

wait 