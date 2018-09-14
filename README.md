# reliable-file-transfer
An application-level file transfer protocol built on top of UDP using novel bandwidth-monitoring techniques to beat FTP and SCP throughputs.


Binary file command: 
- mac: dd if=/dev/urandom of=data.bin bs=1m count=1000
- ubuntu: dd if=/dev/urandom of=data.bin bs=1M count=1000
- diff: https://stackoverflow.com/questions/12118403/how-to-compare-binary-files-to-check-if-they-are-the-same

Different Methods: https://www.keycdn.com/support/udp-file-transfer/ 

40 Mb solution: https://github.com/lsingal/Fast-and-Reliable-File-Transfer-over-UDP

read/write problem: https://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files

CRC verificaiton:  https://www.linuxquestions.org/questions/programming-9/sending-files-over-udp-sockets-in-c-254978/

to compile server_md5 and client mdf
gcc server_md5.c -lm -lssl -lcrypto -o server
gcc client_md5.c -lm -lssl -lcrypto -o client 
