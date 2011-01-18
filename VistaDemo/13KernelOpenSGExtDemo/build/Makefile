all: makealldemos

DIRS = $(shell  find .. -maxdepth 1 -name "*Demo*" -type d)

makealldemos:
	for i in $(DIRS); do make -C $$i/build all; done
 
clean:
	for i in $(DIRS); do make -C $$i/build clean; done
