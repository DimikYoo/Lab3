all:

	gcc -o snbash snbash.c -fsanitize=address
	chmod 777 snbash
