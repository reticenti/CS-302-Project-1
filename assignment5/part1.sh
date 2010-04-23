#!/bin/bash

tempfile=`tempfile`
userlist="UserIdPasswd"

addNewUser() {
	tmpuser=""
	tmppass=""
	exec 3>&1
	tmpuser=$(dialog --title "User ID" \
				--inputbox "Please Enter a New User ID" 0 0 2>&1 1>&3)

	tmppass=$(dialog --title "Password" \
				--inputbox "Please Enter a New User Password" 0 0 2>&1 1>&3)
	exec 3>&-
	echo "$tmpuser $tmppass" >> $userpass
}

deleteUser() {
	dialog --title "Deleting" \
		--checklist  "Delete Users" 0 0 15 \
		--file $userlist 2> deleted

	cat deleted | tr -d \" | tr " " "\n" > deleted # puts the file into a good format
	grep -vf deleted $userlist > tmp # deletes the users not found in deleted
	cp tmp $userlist
	#rm deleted
}

printUsers(){
	dialog --title "Users and their IDS" \
	--msgbox "`sort $userlist`" 0 0
}

while :
do

	dialog --cancel-label "Exit" --title "User IDs and Passwords" \
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
			addNewUser;;
		2)
			deleteUser;;
		3)
		;;
		4)
			printUsers;;
		5)
		;;
		6)
		;;
		7)
		;;
		* ) #Exit or Esc
		clear
		exit 0
		;;
	esac
done
