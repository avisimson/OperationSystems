#Name: Avi Simson id: 205789100
#!/bin/bash
if [ "$#" -ne 2 ]
then
	#not 2 arguments so error.
	echo "error: you did not enter 2 arguments"
else
	#first argument is the bank costumer
	person=$1
	#second argument is bank list
	list=$2
	#print all occurences in file
	grep "$person" "$list"
	#total balance
	grep "$person" "$list" | awk -F ' ' '{ sum=sum+$3 } END { print "Total balance: "sum }'
fi
