# Blockchain
Implementation of a distributed ledger in C++

TODO:

-Implement PP2pP 

-Implement URB with majority ack

-Implement an output manager

-Implement FIFO queue

-Implement (more logic?)



Notes:

-since we do not care about the effeciency and only the speed of the system we are probably going to bitslam when sending. We need to ensure that we send the messages often enough that they are received, even with artifical packet loss, but not slow down the system. Maybe send each message 100(?) times and then terminate the thread. 

DONE:

-Implement UDP sender and receiver
