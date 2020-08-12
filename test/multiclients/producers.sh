
#!/bin/bash
 
for i in {1..10000}
do
    ./producer &
    # sleep 0.001s
done
