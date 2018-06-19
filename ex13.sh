#Name: Avi Simson id: 205789100
#!/bin/bash
#check if num of arguments isnt 1
IFS=$'\n'
if [ "$#" -ne 1 ]
then
    echo "error: only one argument is allowed"
else
    #file gets argument name and then check if exist.
    file=$(echo $1 | xargs)
    echo $file
    if [ -f $file ]
    then
        if [ ! -d safe_rm_dir ]
	then
            mkdir safe_rm_dir
        fi
	cp $file safe_rm_dir/
        rm $file
        echo "done!"
    else
        echo "error: there is no such file"
    fi
fi
