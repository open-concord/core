
G=g++ -std=c++20 -c -I../../lib/inc/ -I./../../inc/
B=bin/
OBJ=$(B)bijson.o \
			$(B)birank.o \
			$(B)branch.o \
			$(B)strman.o \
			$(B)server.o \
			$(B)claf.o

default: $(OBJ) inc/claf.hpp
	ar cr bin/libclaf.a $(OBJ)
	ranlib bin/libclaf.a

# global (to claf) headers
H=inc/claf.hpp

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
$(B)claf.o: $(H)
	$(G) src/claf.cpp -o $@

clean:
	rm -f bin/*
