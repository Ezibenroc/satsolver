CXX = g++
CPP_FILES = `ls src/*.cpp`

ARCHIVE_NAME=CornebizeLorentz

all: main

archive:
	@rm --force $(ARCHIVE_NAME).tar*
	git archive HEAD --output $(ARCHIVE_NAME).tar --prefix=$(ARCHIVE_NAME)/
	cd rapport; pdflatex rapport.tex; cp rapport.pdf ..
	tar --update --file $(ARCHIVE_NAME).tar --transform=s,^,$(ARCHIVE_NAME)/, rapport.pdf rapport/rapport.pdf
	rm rapport.pdf
	gzip $(ARCHIVE_NAME).tar

main: src/*.cpp src/*.h
	$(CXX) -Wall -g -o main main.c $(CPP_FILES)

clean:
	rm --force main

.PHONY: clean
