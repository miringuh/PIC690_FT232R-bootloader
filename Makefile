FILE=f690
PFILE=parser
PATH=/opt/microchip/xc8/v2.46/pic/bin/xc8
PATH2=/opt/microchip/xc8/v2.46/bin/xc8-cc
all:
	${PATH2} -mcpu=16f690 -mno-keep-startup -Wpedantic --fill=0X3FFF@0:0X19F -w -mdownload-hex -ginhx32   -mconst-data-in-progmem -mc90lib  -funsigned-char -oheaders/$(FILE) ${FILE}.c
#	${PATH} --chip=16F690 --fill=0X3FFF@0:0X7F -Oheaders/$(FILE) ${FILE}.c
#	make parser
#	-mdownload
	/usr/bin/clang -oparser2.o parser.c
	./parser2.o>icsp.txt
parser:
#	/usr/bin/clang -oparser.o parser.c
#	./parser.o>../AVRs/isp.c
	/usr/bin/clang -oparser2.o parser.c
	./parser2.o>icsp.txt
arch:
#	xc8-ar [options] file.a [file.p1 file.o ...] c_code .p1 asm_code .o
	/opt/microchip/xc8/v2.46/bin/xc8-ar -r myLib.a ${FILE.p1} ${FILE.p1}

clean:
	/bin/rm -f headers/*.obj *.o headers/*.as headers/*.pre headers/*.sym headers/*.hex headers/*.sdb headers/*.lst headers/*.o headers/*.rlf headers/*.s headers/*.d headers/*.i headers/*.p1 headers/*.cmf headers/*.elf headers/*.hxl 
	/bin/clear 
