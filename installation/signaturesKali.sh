#!/bin/bash

if [ "$EUID" -ne 0  ]
	then echo -e "\e[31m[+]\e[0m \e[32mPlease run as root\e[0m"
	exit
fi

wget -q -O - archive.kali.org/archive-key.asc | apt-key add

echo -e "[+] \e[32mSignatures updated\e[0m"
