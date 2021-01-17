vpath %.h include

OBJ = obj
SRC = src
INCLUDE = -I ../include
MAILBOXPTH = ./server/.mailboxes

COMPOBJ = cd obj && gcc -c ../
COMPSER = gcc -o

target : test/server test/server/mailserver 

test/server :
	cd test && mkdir server && mkdir $(MAILBOXPTH)

test/server/mailserver : $(OBJ)/mailserver.o \
	$(OBJ)/cmmd.o $(OBJ)/mio.o $(OBJ)/msocket.o \
	$(OBJ)/mtcp.o $(OBJ)/threadpool.o $(OBJ)/user.o
	$(COMPSER) $@ $^ $(INCLUDE) -lpthread

$(OBJ)/mailserver.o : $(SRC)/mailserver.c
	$(COMPOBJ)$^ $(INCLUDE) -lpthread

$(OBJ)/cmmd.o : $(SRC)/cmmd.c
	$(COMPOBJ)$^ $(INCLUDE) -lpthread

$(OBJ)/mio.o : $(SRC)/mio.c
	$(COMPOBJ)$^ $(INCLUDE)

$(OBJ)/msocket.o : $(SRC)/msocket.c
	$(COMPOBJ)$^ $(INCLUDE)

$(OBJ)/mtcp.o : $(SRC)/mtcp.c
	$(COMPOBJ)$^ $(INCLUDE) -lpthread

$(OBJ)/threadpool.o : $(SRC)/threadpool.c
	$(COMPOBJ)$^ $(INCLUDE) -lpthread

$(OBJ)/user.o : $(SRC)/user.c
	$(COMPOBJ)$^ $(INCLUDE)

clean :
	rm $(OBJ)/*.o