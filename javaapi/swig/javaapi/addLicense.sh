#!/bin/bash
# addLicense.sh

# Adds license to every java file
# Param1: license file
# Param2: java folder
# Use: addLicense.sh ../LICENSE ../ilwisjava/src/main/java/org/n52/ilwis/java

if [ $# -ne 2 ]
then
	printf "Requires 2 arguments."
else
	header=$1
	folder=$2
	tempFile=$folder/temp.txt
	for f in $folder/*.java
	do
		if grep -Fq "GNU General Public License" "$f";
		then
			:
		else
			printf "/*\n" >"$tempFile"
			cat "$header" >> "$tempFile"
			printf "\n*/\n" >> "$tempFile"
			cat "$f" >> "$tempFile"
			mv -f "$tempFile" "$f"
		fi
	done	
fi

: <<'END'
