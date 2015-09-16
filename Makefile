CFLAGS := -g -Os
TARGETS := loop

all: $(TARGETS)

run: all
	./loop

dis: all
	objdump -ldS loop

clean:
	rm -f $(TARGETS)
