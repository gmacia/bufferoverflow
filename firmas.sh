gpg --keyserver hkp://keys.gnupg.net --recv-key 7D8D0BF6

gpg -a --export ED444FF07D8D0BF6 | sudo apt-key add -
