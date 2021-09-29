# todo

# about
![example screenshot](https://github.com/eastriverlee/todo/blob/master/example.png?raw=true)
todo is a minimal, portable cli-based task manager.

# install
after cloning this repository, normally all you need to do is:
```sh
make
```

note that this is equivalent to:
```sh
cc -o todo todo.c && mv todo /usr/local/bin
```
so, if the `make` command does not work for some reason,  
complie `todo.c` by yourself and move the binary file to one of your `$PATH` directory.

# usage
## add
> add tasks
```sh
todo <tasks>
todo 'take out trash' 'write clean code'
```

## list
> list all tasks
```sh
todo ls
```

## toggle
> toggle tasks' status(done/to-do) by indices
```sh
todo <indices>
todo 0 3 5
```

## swap
> swap two tasks' order by their indices
```sh
todo swap <index> <index>
todo swap 2 4
```

## remove
> remove tasks by indices
```sh
todo rm <indices>
todo rm 3 2
```

## clear
> clear all tasks
```sh
todo clear
```

# uninstall
```sh
make uninstall
```
which is equvalent to:
```sh
rm todo /usr/local/bin/todo
```
