INC = inc
SRC = src
MISC = misc
TESTS = tests
CC = g++
CFLAGS = -g -w 
ASMSRC = $(wildcard $(SRC)/asm/*.cpp)
#ASMINC = $(wildcard $(INC)/asm/*.hpp)
ASMOBJ = $(patsubst $(SRC)/asm/%.cpp,$(SRC)/asm/%.o,$(ASMSRC))
ASMDEP = $(patsubst $(SRC)/asm/%.cpp,$(SRC)/asm/%.d,$(ASMSRC))
COMMONSRC = $(wildcard $(SRC)/common/*.cpp)
#COMMONINC = $(wildcard $(INC)/common/*.hpp)
COMMONOBJ = $(patsubst $(SRC)/common/%.cpp,$(SRC)/common/%.o,$(COMMONSRC))
COMMONDEP = $(patsubst $(SRC)/common/%.cpp,$(SRC)/common/%.d,$(COMMONSRC))
LINKERSRC = $(wildcard $(SRC)/linker/*.cpp)
#LINKERINC = $(wildcard $(INC)/linker/*.hpp)
LINKEROBJ = $(patsubst $(SRC)/linker/%.cpp,$(SRC)/linker/%.o,$(LINKERSRC))
LINKERDEP = $(patsubst $(SRC)/linker/%.cpp,$(SRC)/linker/%.d,$(LINKERSRC))
EMUSRC = $(wildcard $(SRC)/emu/*.cpp)
#EMUINC = $(wildcard $(INC)/emu/*.hpp)
EMUOBJ = $(patsubst $(SRC)/emu/%.cpp,$(SRC)/emu/%.o,$(EMUSRC))
EMUDEP = $(patsubst $(SRC)/emu/%.cpp,$(SRC)/emu/%.d,$(EMUSRC))
MISCSRC = $(MISC)/bison.tab.c $(MISC)/len.yy.c
MISCINC =	$(MISC)/bison.tab.h
MISCOBJ = $(MISC)/bison.tab.o $(MISC)/len.yy.o
MISCDEP = $(MISC)/bison.tab.d

build: asmbuild linkerbuild emubuild

asmbuild : $(MISCOBJ) $(COMMONOBJ) $(ASMOBJ)
	$(CC) -o asembler  $^ -g

linkerbuild: $(LINKEROBJ) $(COMMONOBJ)
	$(CC) -o linker $^ -g
emubuild : $(EMUOBJ) 
	$(CC) $^ -o emulator -g

$(SRC)/common/%.d: $(SRC)/common/%.cpp
	$(CC) -MM -MT $(SRC)/common/$*.o $< $(CFLAGS)-I$(INC)/common > $@
include $(COMMONDEP) 

$(SRC)/asm/%.d: $(SRC)/asm/%.cpp
	$(CC) -MM -MT $(SRC)/asm/$*.o $< $(CFLAGS)-I$(INC)/asm > $@
include $(ASMDEP) 

$(SRC)/linker/%.d: $(SRC)/linker/%.cpp
	$(CC) -MM -MT $(SRC)/linker/$*.o $< $(CFLAGS)-I$(INC)/linker > $@
include $(LINKERDEP) 

$(SRC)/emu/%.d: $(SRC)/emu/%.cpp
	$(CC) -MM -MT $(SRC)/emu/$*.o $< $(CFLAGS)-I$(INC)/emu > $@
include $(EMUDEP)

$(MISC)/%.d : $(MISC)/%.c
	$(CC) -MM -MT $(MISC)/$*.o $< $(CFLAGS)-I$(MISC) > $@
include $(MISCDEP)
$(SRC)/common/%.o : $(SRC)/common/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)-I$(INC)/common

$(SRC)/asm/%.o : $(SRC)/asm/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)-I$(INC)/asm

$(SRC)/linker/%.o : $(SRC)/linker/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)-I$(INC)/linker

$(SRC)/emu/%.o : $(SRC)/emu/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)-I$(INC)/emu

$(MISC)/%.o : $(MISC)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)-I$(MISC)

$(MISC)/bison.tab.c : $(MISC)/bison.y
	bison -v -d $< -o $@

$(MISC)/len.yy.c: $(MISC)/flex.l $(MISC)/bison.tab.h 
	flex -o $@ $< 


clean:
	rm -f $(MISC)/bison.tab.c
	rm -f $(MISC)/bison.tab.h
	rm -f $(MISC)/len.yy.c
	rm -f $(MISC)/*.d
	rm -f $(MISC)/*.o
	rm -f $(SRC)/common/*.o
	rm -f $(SRC)/common/*.d
	rm -f $(SRC)/asm/*.o
	rm -f $(SRC)/asm/*.d
	rm -f $(SRC)/linker/*.o
	rm -f $(SRC)/linker/*.d
	rm -f $(SRC)/emu/*.o
	rm -f $(SRC)/emu/*.d
	rm -f $(TESTS)/nivo-a/*.o
	rm -f $(TESTS)/nivo-a/*.o.otxt
	rm -f $(TESTS)/nivo-a/program.hex
	rm -f $(TESTS)/nivo-a/program.hextxt
	rm -f $(TESTS)/nivo-b/*.o
	rm -f $(TESTS)/nivo-b/*.o.otxt
	rm -f $(TESTS)/nivo-b/program.hex
	rm -f $(TESTS)/nivo-b/program.hextxt
	rm -f $(TESTS)/nivo-c/*.o
	rm -f $(TESTS)/nivo-c/*.o.otxt
	rm -f $(TESTS)/nivo-c/program.hex
	rm -f $(TESTS)/nivo-c/program.hextxt
	rm -f ./asembler
	rm -f ./linker
	rm -f ./emulator

.PHONY: clean asmbuild linkerbuild emubuild build 
