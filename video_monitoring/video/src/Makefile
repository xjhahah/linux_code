all:
	arm-linux-gcc *.c -o ../bin/armmain -I./jpeg -L./jpeg -ljpeg -lapi_v4l2_arm -pthread -Wimplicit-function-declaration
PHONY:clean
clean:
	rm ../bin/armmain
