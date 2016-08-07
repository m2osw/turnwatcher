CFLAGS=`pkg-config --cflags gtkmm-2.4` -g
LDFLAGS=`pkg-config --libs gtkmm-2.4`
OBJS=examplewindow.o main.o
BIN=examplewindow

all: $(BIN)

clean:
	rm -f *.o $(BIN)

$(BIN): $(OBJS)
	gcc $(LDFLAGS) $(OBJS) -o $(BIN)

.cc.o:
	gcc $(CFLAGS) -c $<

main.o: main.cc examplewindow.h

examplewindow.o: examplewindow.cc examplewindow.h
