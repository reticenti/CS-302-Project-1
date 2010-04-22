#!/bin/bash

tempfile=`tempfile`

dialog --title "User IDs and Passwords" \
--menu "Choose an option" 15 55 7 \
"1" "Add New User" \
"2" "Delete User" \
"3" "Verify User" \
"4" "Print Users" \
"5"  "Store Data using Inorder Traversal" \
"6"  "Store Data using Preorder Traversal" \
"7"  "Store Data using Postorder Traversal" 2> $tempfile

return_value=$?

choice=`cat $tempfile`

case $choice in
	1)
    exec 3>&1
    tmpuser=$(dialog --title "User ID" \
        --inputbox "Please Enter a New User ID" 0 0 2>&1 1>&3)
    tmppass=$(dialog --title "Password" \
        --inputbox "Please Enter a New User Password" 0 0 2>&1 1>&3)
    exec 3>&-
    echo "$tmpuser $tmppass" >> UserIdPasswd.txt
	;;
	2)
	;;
	3)
	;;
	4)
    dialog --title "Users and their IDS" \
        --msgbox "`sort UserIdPasswd.txt`" 15 `wc -L UserIdPasswd.txt | awk '{print $1 + 4}'`
	;;
	5)
	;;
	6)
	;;
	7)
	;;
esac
