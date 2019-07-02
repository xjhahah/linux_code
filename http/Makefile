cc=gcc
src=http.c
bin=http 
LDFLAGS=-lpthread

$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS)

.PHONY:clean
clean:
	rm -f $(bin)
