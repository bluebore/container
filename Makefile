

CXX=g++
CXXFLAGS=-g2 -Wall -Werror
LDFLAGS=

SOURCE= $(wildcard *.cc)
OBJS=$(SOURCE:.cc=.o)

all: container

container: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf container

rootfs:
	rm -rf rootfs
	tar zxf rootfs.tar.gz
