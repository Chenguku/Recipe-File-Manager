FILENAME = HM_Recipe_Program.c
HEADERNAME = HM_Recipe_Program.h

all : recipes

recipes : recipes.o
	gcc -o HM_Recipe_Program $(FILENAME)

recipes.o : $(FILENAME) $(HEADERNAME)
	gcc -c $(FILENAME) -g

clean : 
	rm -f $(Q1) $(Q2)
