#!/bin/bash
sudo apt-get install make gcc libncurses5-dev
printf "\n\nTo run the program, execute 'make'.\nWould you like to run the program? "
read -r -p "(yes/no)" response
if [[ $response =~ ^([yY][eE][sS]|[yY])$ ]]
 then 
 make
fi