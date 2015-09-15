CFLAGS := -g -Os
TARGETS := loop

all: $(TARGETS)

run: all
	./loop

dis: all
	objdump -d loop

clean:
	rm -f $(TARGETS)
