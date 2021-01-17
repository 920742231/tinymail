COMPOBJ = cd obj && gcc -c
INCLUDE = -I ../include

target : test/mailclient

test/mailclient : obj/mailclient.o obj/tcattrctl.o \
	obj/cmmd.o obj/mio.o obj/msocket.o
	gcc -o $@ $^ 

obj/mailclient.o : src/mailclient.c
	$(COMPOBJ) ../$^ $(INCLUDE)

obj/cmmd.o : src/cmmd.c
	$(COMPOBJ) ../$^ $(INCLUDE)

obj/mio.o : src/mio.c
	$(COMPOBJ) ../$^ $(INCLUDE)

obj/msocket.o : src/msocket.c
	$(COMPOBJ) ../$^ $(INCLUDE)

obj/tcattrctl.o : src/tcattrctl.c
	$(COMPOBJ) ../$^ $(INCLUDE)