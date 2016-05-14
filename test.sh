#!/bin/sh

cd src/
./server 1234 0 255 &
sleep 0.1
./server 5678 256 511 &
sleep 0.1
cd ../test
for i in 1 2 3; do
	echo "Start cycle $i"
	./countspace dm.conf > file1 &
	A=$!
	./countword dm.conf > file2 &
	B=$!
	wait $A $B

	A=`cat file1`
	B=`cat file2`
	rm file1 file2

	if [ x"$A" != x"$B" ]; then
		echo "FAIL"
		exit 1
	fi
	echo "End cycle $i"
done

echo "OK"
killall server
