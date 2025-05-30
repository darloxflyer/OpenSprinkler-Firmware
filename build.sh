#!/bin/bash
set -e

DEBUG=""

while getopts ":s:u:d" opt; do
  case $opt in
    s)
	  SILENT=true
	  #command shift
      ;;
    d)
          DEBUG="-DENABLE_DEBUG -DSERIAL_DEBUG"
	  #command shift
      ;;
    u)
          useopts+=("$OPTARG")
          #command shift
      ;;
  esac
done
shift $((OPTIND -1))

# Process include options to build additional modules
USESSD1306=""
USEMODULES=""
SKIP_PACKAGES=false
for val in "${useopts[@]}"; do
  if [ "$val" == "ssd1306" ]; then
    USEMODULES+="-DUSE_SSD1306 "
    USESSD1306="1"
  elif [ "$val" == "manual_relay" ]; then
    USEMODULES+="-DMANUAL_RELAY "
  elif [ "$val" == "use_gpio_buttons" ]; then
    USEMODULES+="-DUSE_GPIO_BUTTONS "
  elif [ "$val" == "skip_packages" ]; then
    SKIP_PACKAGES=true
  else
    echo "Unknown use option specified: $val"
    exit 0
  fi
done

echo "Building OpenSprinkler..."

#Git update submodules

if git submodule status | grep --quiet '^-'; then
    echo "A git submodule is not initialized."
    git submodule update --recursive --init
else
    echo "Updating submodules."
    git submodule update --recursive
fi

if [ "$1" == "demo" ]; then
	echo "Installing required libraries..."
	apt-get install -y libmosquitto-dev libssl-dev
	echo "Compiling demo firmware..."

    ws=$(ls external/TinyWebsockets/tiny_websockets_lib/src/*.cpp)
    otf=$(ls external/OpenThings-Framework-Firmware-Library/*.cpp)
    g++ -o OpenSprinkler -DDEMO -DSMTP_OPENSSL $DEBUG -std=c++14 -g -O0 -include string.h main.cpp OpenSprinkler.cpp program.cpp opensprinkler_server.cpp utils.cpp weather.cpp gpio.cpp mqtt.cpp smtp.c -Iexternal/TinyWebsockets/tiny_websockets_lib/include $ws -Iexternal/OpenThings-Framework-Firmware-Library/ $otf -lpthread -lmosquitto -lssl -lcrypto
elif [ "$1" == "osbo" ]; then
	echo "Installing required libraries..."
	apt-get install -y libmosquitto-dev libssl-dev
	echo "Compiling osbo firmware..."

    ws=$(ls external/TinyWebsockets/tiny_websockets_lib/src/*.cpp)
    otf=$(ls external/OpenThings-Framework-Firmware-Library/*.cpp)
	g++ -o OpenSprinkler -DOSBO -DSMTP_OPENSSL $DEBUG -std=c++14 -include string.h main.cpp OpenSprinkler.cpp program.cpp opensprinkler_server.cpp utils.cpp weather.cpp gpio.cpp mqtt.cpp smtp.c -Iexternal/TinyWebsockets/tiny_websockets_lib/include $ws -Iexternal/OpenThings-Framework-Firmware-Library/ $otf -lpthread -lmosquitto -lssl -lcrypto
else

        CXXFLAGS=""
        ARDUINO_OVERRIDES=""
        SSD1306LIB=""
        
        if [ ! "$SKIP_PACKAGES" = true ]; then
  	  echo "Installing required libraries..."
  	  apt-get update
	  apt-get install -y libmosquitto-dev raspi-gpio libi2c-dev libssl-dev libgpiod-dev

	  ssd1306=""
	  if [ "$USESSD1306" == "1" ]; then
	    echo "Installing SSD1306 Display Support"
	    apt-get install -y libbcm2835-dev
	  fi
	fi
	
	if [ "$USESSD1306" == "1" ]; then
	    echo "Including SSD1306 Drivers"
	    ssd1306="-Iexternal/SSD1306_OLED_RPI/include external/SSD1306_OLED_RPI/src/SSD1306_OLED.cpp external/SSD1306_OLED_RPI/src/SSD1306_OLED_graphics.cpp external/SSD1306_OLED_RPI/src/SSD1306_OLED_Print.cpp external/SSD1306_OLED_RPI/src/SSD1306_OLED_font.cpp LinuxSSD1306Display.cpp"
	    SSD1306LIB="-lbcm2835"
	fi

	if ! command -v raspi-gpio &> /dev/null
	then
		echo "Command raspi-gpio is required and is not installed"
		exit 0
	fi

	USEGPIO=""
	GPIOLIB=""

	if [ -h "/sys/class/gpio/gpiochip512" ]; then
		echo "using libgpiod"
		USEGPIO="-DLIBGPIOD"
		GPIOLIB="-lgpiod"
	fi

	echo "Compiling ospi firmware..."
    ws=$(ls external/TinyWebsockets/tiny_websockets_lib/src/*.cpp)
    otf=$(ls external/OpenThings-Framework-Firmware-Library/*.cpp)
	g++ -o OpenSprinkler $CXXFLAGS -DOSPI $ARDUINO_OVERRIDES $USEMODULES $USEGPIO -DSMTP_OPENSSL $DEBUG -std=c++14 -include string.h $ssd1306 main.cpp OpenSprinkler.cpp program.cpp opensprinkler_server.cpp utils.cpp weather.cpp gpio.cpp mqtt.cpp smtp.c -Iexternal/TinyWebsockets/tiny_websockets_lib/include $ws -Iexternal/OpenThings-Framework-Firmware-Library/ $otf -lpthread -lmosquitto -lssl -lcrypto $GPIOLIB $SSD1306LIB
fi

if [ -f /etc/init.d/OpenSprinkler.sh ]; then
    echo "Detected the only init.d start up script, removing."
    echo "If you still want OpenSprinkler to launch on startup make sure when you run the build script to answer \"Y\" to the following question."
    /etc/init.d/OpenSprinkler.sh stop
    rm /etc/init.d/OpenSprinkler.sh
fi

if [ ! "$SILENT" = true ] && [ -f OpenSprinkler.service ] && [ -f startOpenSprinkler.sh ] && [ ! -f /etc/systemd/system/OpenSprinkler.service ]; then

	read -p "Do you want to start OpenSprinkler on startup? " -n 1 -r
	echo

	if [[ ! $REPLY =~ ^[Yy]$ ]]; then
		exit 0
	fi

	echo "Adding OpenSprinkler launch service..."

	# Get current directory (binary location)
	pushd "$(dirname $0)" > /dev/null
	DIR="$(pwd)"
	popd > /dev/null

	# Update binary location in start up script
	sed -e 's,\_\_OpenSprinkler\_Path\_\_,'"$DIR"',g' OpenSprinkler.service > /etc/systemd/system/OpenSprinkler.service

	# Make file executable
	chmod +x startOpenSprinkler.sh

    # Reload systemd
    systemctl daemon-reload

    # Enable and start the service
    systemctl enable OpenSprinkler
    systemctl start OpenSprinkler
fi

echo "Done!"
