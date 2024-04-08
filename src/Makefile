CFILES:=$(wildcard *.c)
TARGET:=ESPController

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(CFILES)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TARGET) $(CFILES) -lubus -lubox -lserialport -lblobmsg_json -lcjson

clean:
	rm -f $(TARGET)