#!/bin/bash

# Detect weather there are empty GPU

flag=(0 0 0 0)
mail_address=0

read -p "Enter e-mail address which you want to recieve information: " mail_address

while [ 1 -eq 1 ]
do
	arr=()
	length=0
	nvidia-smi | grep % > temp
	while read line; do arr[length++]=${line}; done < temp
	rm temp

	for((p=0;p<length;p++))
	do
		if [ ${arr[p]:${#arr[p]}-18:2} == "0" ]
		then
			if [ ${flag[p]} -eq 0 ]
			then
				flag[p]=1

				echo -e "Subject: New empty GPU!">temp
				echo -e "GPU "$p" is empty now!">>temp
				echo -e " "
				nvidia-smi>>temp
				sendmail ${mail_address}<temp 
				rm temp
			fi
		else
			flag[p]=0
		fi
		
	done
	
	sleep 30s

done
