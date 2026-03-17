# todo-query
todo-query is a simple C program that lists all TODOs in a file or directory.
 

## usage
`tq <file | dir>`

## flags
`-s<separator>` to set the separator between file name:line and TODO string. For example: `-s">"` means the output will look like

```
main.c:83 > Hello, TODO! 
```

By default, the separator is set to "|" and the output will look like

```
main.c:83 | Hello, TODO! 
```

---

`-a` to query all files. by default, todo-query excludes these file types:
- .tres
- .tscn
