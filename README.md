# c-webserver
***web server made in the C language that server an html file***

To run just compile the code with
```
gcc -o [name_of_output] webserver.c
```
and then run the output file with ```./[name_of_input]```

---
Currently the webserver reads from a file named **"TC2025.html"** but changing this file for any other html or plain text file should work.

Just put the route to the new file where "TC2025.html" appears in the code. The route takes the directoty of where the file resides as the root of the server.
