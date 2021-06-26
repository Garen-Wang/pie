# pie

## Introduction

Pie (abbr. Pie Is an Editor) is a cross-platform text editor, intended to be fast, efficient, easy-to-use, while having basic support for essential text editing features.

## Features

- graphical user interface

- modal editing

- syntax highlighting

- configurable

## Usage

![img1.png](https://i.loli.net/2021/06/26/iGeW4ktqBbO3Fg6.png)

![img2.png](https://i.loli.net/2021/06/26/MwS6G8yHIndVXtY.png)

With the feature of modal editing, Pie has normal mode and insert mode. 

insert mode is the only mode in traditional text editors, in which what users input is what is modified on the text. However, editors with only insert mode is not efficient in some scenarios that users should move a hand to press the arrow keys or use the mouse or touchpad.

To solve the problem mentioned above, Pie features modal editing. The initial mode of Pie is normal mode, in which users' input will not edit the text directly, but is mapped into the corresponding operations on the text.

First, you need to learn how to move your cursor. When in normal mode:

- Press i to move the cursor upward.
- Press k to move the cursor downward.
- Press j to move the cursor leftward.
- Press l to move the cursor rightward.

And there are also some buttons that you should also bear in mind, which may be frequently used. When in normal mode:

- Press u to insert at the cursor's current position.
- Press o to insert a new line under the cursor's current line.
- Press z to undo.
- Press y to redo.

Next is the feature of selection. When in normal mode:

- Press w to select the next word in the cursor's position.
- Press b to select the previous word in the cursor's position.

![img3.png](https://i.loli.net/2021/06/26/oIuHxlrVbO2WheG.png)

- Press m to select (i.e. match) the nearest code enclosed by parentheses, brackets, or curly braces.

![img4.png](https://i.loli.net/2021/06/26/1LhbG6qEQ3l24TU.png)

- Press x to select the whole line.

![img5.png](https://i.loli.net/2021/06/26/y5M6wqTEm8gvoLe.png)

After users select a selection, we can do operations against the selection, for example:

- Press d to delete the current selection.
- Press c to replace the current selection.

Also, the same features can be enjoyed when using Pie to edit Java source code and Python source code:

![img6.png](https://i.loli.net/2021/06/26/SanzOPphEBktvl4.png)

![img7.png](https://i.loli.net/2021/06/26/AR1xVPT6u4OkMpz.png)

In addition, settings of Pie editor can be adjusted by modifying .pierc, which is at the source root of Pie executable. For example, you can type the following statements into .pierc:

```
set fontFamily="Fira Code"
set FontSize=12
```

If you would like to change the system default font to your favorite one like Sarasa Gothic, Inconsolata or Mocano, simply overwrite the name of the target font, wrapped in a pair of double quotation marks. Similarly, font size can also be modified manually.

## Installation

### Windows

1. Download the pre-built version of Pie.
2. Extract the archive.
3. Run `pie.exe`.

### Linux

1. Download the Appimage of Pie.
2. Run `pie-x86_64.Appimage`.

### Build from Source

1. Clone the repository.
2. Build the project in QtCreator or Visual Studio or command line.
3. Copy `shl` and `test` to the source root of the build directory.
4. Run the executable.

## License

The repository is under WTFPL.
