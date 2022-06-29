
G=g++ -std=c++20 -c -I../../lib/inc/
B=bin/
OBJ=$(B)bijson.o \
			$(B)birank.o \
			$(B)branch.o \
			$(B)strman.o \
			$(B)server.o

default: $(OBJ) inc/claf.hpp
	g++ -c src/claf.cpp -o claf.o	

# global (to claf) headers
H=inc/claf.hpp ../../lib/inc/uttu.hpp

$(B)bijson.o: $(H)
	$(G) src/bijson.cpp -o $@
$(B)birank.o: $(H)
	$(G) src/birank.cpp -o $@
$(B)branch.o: $(H)
	$(G) src/branch.cpp -o $@
$(B)strman.o: $(H)
	$(G) src/strman.cpp -o $@
$(B)server.o: $(H) src/server/apply.hpp
	$(G) src/server/server.cpp -o $@

clean:
	rm -f bin/*
