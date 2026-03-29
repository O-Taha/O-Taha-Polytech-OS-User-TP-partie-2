default : servBEUIP cliBEUIP

cliBEUIP : cliBEUIP.c
	cc -Wall -o cliBEUIP -DDEBUG cliBEUIP.c

servBEUIP : servBEUIP.c
	cc -Wall -o servBEUIP -DDEBUG servBEUIP.c

clean :
	rm -f cliBEUIP servBEUIP

