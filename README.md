# xroads
C++23 utility classes for developing a game engine. To use, add all .cpp files from the src/ directory to your project. It is currently only tested with msys2/MinGW64.

## Dependencies

glfw - Window creation and input handling

glad - OpenGL bindings

soloud - Sound

dwarfstack - For human-readable stacktraces

## Coding standards

Use UpperCamelCase for function, method, class and namespace names. 

Use snake_case for variable names.

Use SHOUTING_SNAKE_CASE for enum names and values.

Use Allman brace syntax:

```cpp
i32 function(i32 param)
{
    return param*2;
}
```

Use four spaces for indentation, no tabs.

## Licence

Everything is MIT licensed.
