FUSE_FILE=fuse-2.9.3
#-----
SOURCES=./main.c \
		./log.c \
		./CService.cpp
GXX=g++ -g -Wall
CPPFLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=-lstdc++ -lcurl
target:
	$(GXX) -o main  $(SOURCES) $(CPPFLAGS) $(LDFLAGS) `pkg-config --libs fuse`
disc:
	umount /tmp/mnt/ 
conn:
	mkdir /tmp/mnt -p
	./main /tmp/mnt 
	tail -F bbfs.log
clean:
	rm -fr *.o main ${FUSE_FILE}
test:
	rm -fr /tmp/mnt/ss 
	echo "ss" > /tmp/mnt/ss

env:
	tar zxvf ${FUSE_FILE}.tar.gz
	cd ${FUSE_FILE}  && \
		./configure
	cd ${FUSE_FILE} && make 
	cd ${FUSE_FILE} && make install

php:
	sudo apt-get install apache2
	sudo apt-get install php5
	sudo apt-get install libapache2-mod-php5
	sudo /etc/init.d/apache2 restart
all: target disc conn
	echo "run"
