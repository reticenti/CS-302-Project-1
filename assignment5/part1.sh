#!/bin/sh

#!/bin/sh

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

you_chose=`cat $tempfile`

case $return_value in
	0)
	echo "You chose'$you_chose'.";;
	1)
	;;
	255)
	;;
esac
