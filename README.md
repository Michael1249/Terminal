# Terminal Discription

## Syntax
Terminal devide input to lines, which separate by one ore more new line symbol.
Every line deviding to words by only one space symbol.
Terminal read input line by line. Line undertend like command and arguments.
First word in line is command name, and others is arguments.

```MAIN << /foo arg1 arg2 [some args][enter]```

Every command return value, if value not equal "" (null) Terminal will write it in console.
If you want use this value in other commands, you want put it in variable.

```MAIN << var <= /foo arg1 arg2 [some args][enter]```

If you want set variable to specific value use "=" operator.
You can set variable only to one line.

```MAIN << var = /foo arg1 arg2 [some args][enter]```

So "var" equal "/foo arg1 arg2 [some args]".

## Commands
### ```//```
- insert comment
    
### ```/help```
- show command list

### ```/exit```
- close terminal
   
### ```/macros```
- add macrocommand, which can call meny others simple commands.
   
```
MAIN << /macros my_macros
/foo_1 arg1 arg2 [some args][enter]
/foo_2 arg1 arg2 [some args][enter]
[enter]
```
Now you can call macros "#my_macros" without arguments.

```MAIN << #my_macros[enter]```

### ```/var```
- add new variable

```MAIN << /var my_var 1 2 3[enter]```

So "my_var" equal "1 2 3".
   
### ```/vars```
- show list of variables
   
### ```/~var```
- delete variable by name
   
### ```/fread```
- read file and return value, takes file path
   
### ```/fmacros```
- create macros with file, takes two argument: name, file path.
