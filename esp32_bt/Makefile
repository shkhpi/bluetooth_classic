PLATFORM = esp32:esp32:esp32da
PORT = /dev/ttyUSB0
ARDUINO_SRC = esp32_bt.ino
SRC = bluetooth.cc bluetooth.h log.cc log.h

all:
	arduino-cli compile -b $(PLATFORM) ./$(ARDUINO_SRC)

flash:
	arduino-cli compile -b $(PLATFORM) -u -p $(PORT) ./$(ARDUINO_SRC)

.PHONY: format
format:
	clang-format -style=file -i $(ARDUINO_SRC) $(SRC)
