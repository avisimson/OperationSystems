#Name-Avi Simson id-205789100
#!/bin/bash
count=0
path=$(printf %q "$1")
eval cd $path
for i in $(ls)
do
	word=${i: -4}
	if [[ "$word" == ".txt" ]]
	then
		count=$[$count + 1]
	fi
done
echo Number of files that end with .txt in the directory is $count

