default: main

main:
	(cd src; make)

clean:
	(cd src; make clean)

# remove the exe file
erase:
	rm -rf main
