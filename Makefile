all: clean compile install monitor

clean:
	- rm *.bin

compile:
	particle compile electron

install:
	particle flash --serial *.bin

monitor:
	sleep 3
	particle serial monitor
