#!/bin/bash

# Shortcuts for executing commands on each docker node
# Call this file from the terminal passing the command name and eventually the command parameters where supported.

case "$1" in "help"|"h")
  echo "composer-install"
  echo "composer-require"
  echo "composer-update"
  echo "composer-remove"
  ;;

"composer-install")
  docker exec -it php-atk composer install
  ;;

"composer-require")
  docker exec -it php-atk composer require "$2"
  ;;

"composer-update")
  filteredArg="$(echo -e "${2}" | tr -d '[:space:]')"
  if [ ! -z "$filteredArg" ];
  then
       docker exec -it php-atk composer update "$2"
  else
      echo -n "---***---    RISKY OPERATION   ---***---"
      echo -n "Are you sure to update all the dependencies(y/n)? "
      read -r answer
      if [ "$answer" != "${answer#[Yy]}" ] ; then
        docker exec -it php-atk composer update
      fi
  fi
  ;;

"composer-remove")
  docker exec -it php-atk composer remove "$2"
  ;;

*)
  echo "$1 $2 is not a valid command"
  ;;
esac