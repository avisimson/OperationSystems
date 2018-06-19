#Name:Avi Simson id:205789100
#!/bin/bash
#path to directory
arg=$(printf %q "$1")
eval cd $arg
#ignore spaces
IFS=$'\n'
for i in $(ls)
do
	if [[ -d $i ]]
	then
		echo "$i is a directory"
	elif [[ -f $i ]]
	then
		echo "$i is a file"
	fi
done
