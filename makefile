NAME=todo
SRC=todo.c

install: build
	mv $(NAME) /usr/local/bin/

build: $(SRC)
	$(CC) -o $(NAME) $(SRC)

uninstall:
	rm /usr/local/bin/$(NAME)

.PHONY: install build uninstall
