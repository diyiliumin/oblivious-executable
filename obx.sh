#!/bin/bash

ob-fill() {
    local cmd
    read -e -p "$(pwd)\$ " -i "$1" cmd && eval "$cmd"
}

result=$(obxsupport "$@" | tee /dev/tty)

echo -e "\033[0m" 
bashs=$(fgrep $'obxtag\r' <<< "$result")
echo "$bashs" | sed 's/obxtag\r//; s/\x1b\[[0-9;]*m//g' | sed 's/obnum//'

IFS=$'\n'
for line in $bashs; do
	tput cuu 1
done
tput cuu 1
unset IFS 

tput cuu 1
echo "输入数字选择命令，可多个，空格隔开，将按照顺序执行，0代表直接退出"
read -e -p "> " input

IFS=$'\n'
for line in $bashs; do
	tput cud 1
done
unset IFS 


if [ "$input" = "0" ]; then
    exit 0
fi

echo -e "\033[0m" 

for num in $input; do
	match=$(fgrep "obnum${num}:obxtag"$'\r' <<< "$result")
	clean=$(echo "$match" | sed $'s/.*obxtag\r\x1b\\[1;32m//; s/\x1b\\[0m$//'  | tr -d '\r\n')
	ob-fill "$clean"
done

