#!/bin/bash

# Check root permissions
if [ "$EUID" -ne 0 ]; then
    echo -e "\e[31m[+]\e[0m \e[31mPlease run as root\e[0m"
    exit 1
fi

KEYRING_DIR="/etc/apt/trusted.gpg.d"
KEY_FILE="$KEYRING_DIR/kali-archive-keyring.gpg"

echo -e "\e[31m[+]\e[0m \e[32mUpdating Kali Linux GPG archive keys...\e[0m"

# 2. Download and import
wget -qO- https://archive.kali.org/archive-key.asc | gpg --dearmor -o "$KEY_FILE" --yes

if [ $? -eq 0 ]; then
    chmod 644 "$KEY_FILE"
    echo -e "\e[31m[+]\e[0m \e[32mSignatures updated successfully in $KEY_FILE\e[0m"
    
    # 3. Updated packages list
    echo -e "\e[31m[+]\e[0m \e[32mUpdating apt package list...\e[0m"
    apt-get update -o Acquire::AllowInsecureRepositories=true -o Acquire::AllowDowngradeToInsecureRepositories=true
else
    echo -e "\e[31m[-]\e[0m \e[31mFailed to download or import GPG key\e[0m"
    exit 1
fi
