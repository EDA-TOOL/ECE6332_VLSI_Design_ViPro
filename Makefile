CC = g++
INCLUDE =
LIB =
ELFFILE = regFileBin
DEBUG_FLAG = NODEBUG

all: $(ELFFILE)

$(ELFFILE): mainRegFile.o calculator.o parser.o periphery.o RegFile.o userInput.o metalCap.o
	$(CC) -D$(DEBUG_FLAG) mainRegFile.o calculator.o parser.o periphery.o RegFile.o userInput.o metalCap.o -o $(ELFFILE)

mainRegFile.o: mainRegFile.cpp calculator.h parser.h periphery.h RegFile.h userInput.h metalCap.h
	$(CC) -D$(DEBUG_FLAG) -c mainRegFile.cpp

calculator.o: calculator.cpp calculator.h
	$(CC) -D$(DEBUG_FLAG) -c calculator.cpp

parser.o: parser.cpp parser.h
	$(CC) -D$(DEBUG_FLAG) -c parser.cpp

periphery.o: periphery.cpp periphery.h
	$(CC) -D$(DEBUG_FLAG) -c periphery.cpp

RegFile.o: RegFile.cpp RegFile.h
	$(CC) -D$(DEBUG_FLAG) -c RegFile.cpp

userInput.o: userInput.cpp userInput.h
	$(CC) -D$(DEBUG_FLAG) -c userInput.cpp

metalCap.o: metalCap.cpp metalCap.h
	$(CC) -D$(DEBUG_FLAG) -c metalCap.cpp

.PHONY : clean
clean:
	rm *.o $(ELFFILE) output.txt
