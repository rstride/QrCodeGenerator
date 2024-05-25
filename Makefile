INCLUDE_PATH = /opt/homebrew/include
LIB_PATH = /opt/homebrew/lib

all: qr_code_generator

qr_code_generator: main.cpp
	g++ -I$(INCLUDE_PATH) -L$(LIB_PATH) -o qr_code_generator main.cpp -lqrencode -lpng

clean:
	rm -f qr_code_generator
