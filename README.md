# ZeOS
SO2-FIB Operating System project

# Credits
- [Isma Quiñones Gama](https://github.com/ismaqg)
- [Alex Rodríguez Navas](https://github.com/neoxelox)

# Running
`make emul`

# Executing Tests
The tests are unitary tests. They cover all the new syscalls and old syscalls that needed to be modified.<br>
The tests can be found in the directory `test`. You can run any test by uncommenting the selected one in the `user.c`.<br>

**NOTE:**<br>
As there is not enough space in the screen for all the tests outputs please enable the tests you want in order to<br>
see them all (uncomment them). If you want to see the implementation of each test, just navigate to: test/<feature>.c<br>
If a test passes it will output something like: `[PASSED] : <test title>` or: `[FAILED] : <test title>` if it does not pass.<br>
If a test requires screen output, that output will be sorrounded by a box like: `======= [OUTPUT] : <test title> =======`<br>
