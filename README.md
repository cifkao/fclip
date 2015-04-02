fclip Readme
============
`fclip` is a command-line clipboard for the filesystem. It allows to use a terminal
to manipulate files much like in a GUI file manager, but in a more powerful and
versatile way. It runs on Linux systems with the D-Bus message bus.

Installation
------------
Build `fclip` using `make`. `fclip` requires `dbus-c++`, `boost_system`, `boost_filesystem` and `boost_program_options`.

`fclip` consists of two components:
* the command-line utility, `fclip`,
* the server, `fclip-server`.

Each user wanting to use the command-line utility needs to have his own copy of
the server running. You will probably want to configure the server to start when
you log in. `fclip` also requires a D-Bus user message bus.

How To Use
----------
This is a brief introduction to using `fclip`. You can also read [the manual](http://github.com/cifkao/fclip/wiki/Fclip-Manual).

`fclip` doesn't store entire files, just file paths. The paths remain in the clipboard
until they are removed.

Use `fclip add` to add files (file paths) to the clipboard:

    $ fclip add index.html
    $ cd ~/Pictures
    $ fclip add kittens/*.jpg

Use `fclip rm` to un-add files:

    $ fclip rm kittens/ugly_kitten.jpg

`fclip each` performs an action on each of the files in the clipboard. Use the `-p` option to print their paths:

    $ fclip each -p
    /home/cifka/bordel/index.html
    /home/cifka/Pictures/kittens/black_kitten.jpg
    /home/cifka/Pictures/kittens/white_kitten.jpg
    /home/cifka/Pictures/kittens/some_other_kitten.jpg

Use `-c` to run a given command
(`{}` gets replaced by each of the file paths):

    $ fclip each -c 'cp {} ~/public_html'
    # ~/public_html now contains index.html and some .jpg files
    
You can supply a directory path to `fclip each` in order to only process files under that directory:

    $ fclip each ~/Pictures -p
    kittens/black_kitten.jpg
    kittens/white_kitten.jpg
    kittens/some_other_kitten.jpg

Clear the clipboard with `fclip clear`. To get an overview of the clipboard's contents, use `fclip status`.

To get help about individual commands and their options, run `fclip help [<command>]`.

### Stashing
If you want a clean clipboard but have some files in it that you want to use later, you can stash them away
for a while using `fclip stash`. Restore them using `fclip stash pop` or get rid of them using
`fclip stash drop`.

If you do `fclip stash` multiple times (without doing `pop` or `drop`), your stashes will be stored in a stack. 
To see the stashes you've stored, use `fclip stash list` or `fclip stash ls`:

    $ fclip stash ls
      #0: /home/cifka
      #1: /usr/local/bin
      
Each stash has a number that you can supply to `fclip stash pop` or `fclip stash drop` to restore
or remove it. If no number is provided, the topmost stash (#0) is chosen.
