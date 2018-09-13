
# in C program put these in system( ... );

# no need for input and output in the code
# it's handled in the script, just send the txt file

# on sender on the file to be sent
# send the txt file to receiver for comparison 
md5sum data1M.bin > sender_md5.txt;

# on receiver on the received file
md5sum data1M.bin > receiver_md5.txt

# on receiver after file transfer is complete to check 
diff receiver_md5.txt sender_md5.txt > diff.txt

if [ -s diff.txt ]
then
    echo "diff returned something so they're different"
else
    echo "diff returned nothing so they're identical"
fi
