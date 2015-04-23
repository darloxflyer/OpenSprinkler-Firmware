#!/bin/bash

echo "Building OpenSprinkler..."

if [ "$1" == "demo" ]; then
	g++ -o OpenSprinkler -DDEMO -m32 main.cpp OpenSprinkler.cpp program.cpp server.cpp utils.cpp weather.cpp gpio.cpp
elif [ "$1" == "osbo" ]; then
	gcc -o OpenSprinkler -DOSBO main.cpp OpenSprinkler.cpp program.cpp server.cpp utils.cpp weather.cpp gpio.cpp
else
	gcc -o OpenSprinkler -DOSPI main.cpp OpenSprinkler.cpp program.cpp server.cpp utils.cpp weather.cpp gpio.cpp
fi

if [ -f OpenSprinkler.launch ] && [ ! -f /etc/init.d/OpenSprinkler.sh ]; then

	read -p "Do you want to start OpenSprinkler on startup? " -n 1 -r
	echo

	if [[ ! $REPLY =~ ^[Yy]$ ]]; then
		exit 0
	fi

	echo "Adding OpenSprinkler launch script..."

	# Get current directory (binary location)
	pushd `dirname $0` > /dev/null
	DIR=`pwd`
	popd > /dev/null

	# Update binary location in start up script
	sed -e 's,\_\_OpenSprinkler\_Path\_\_,'"$DIR"',g' OpenSprinkler.launch > OpenSprinkler.sh

	# Make file executable
	chmod +x OpenSprinkler.sh

	# Move start up script to init.d directory
	sudo mv OpenSprinkler.sh /etc/init.d/

	# Add to auto-launch on system startup
	sudo update-rc.d OpenSprinkler.sh defaults

	# Start the deamon now
	sudo /etc/init.d/OpenSprinkler.sh start

fi

echo "Done!"