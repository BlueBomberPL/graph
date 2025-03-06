Basic Graph Generator (BGG)

This program gives an oportunity to work on simplified graphs. Each graph
consists of vertices that are connected together via arches.

Functionality:
  - adding/deleting vertices
  - creating/deleting arches
  - editing whole vertex at once
  - formatted printing altogether with basic info 
  - resizing/clearing graphs
  - finding arches
  - importing from files (soon)
  - generating by a chatbot (soon)

This program is really safe in terms of
the input security - all the words are
carefully analysed. Pretty color scheme
makes this program even more pleasant.
Works both on Linux and Windows OS.

---

Example usage:
```
size 10 -f        // Resizes the graph to 10 isolated vertices (without asking)
set 0: 1 2 3 4    // Making the 1st vertex point to the 2nd, 3rd, 4th and 5th vertices
add 0             // Adding new vertex pointing to the 1st one
arch add last 1   // Previously created vertex now also points to the 2nd one
arch del 0 4      // The 1st vertex no longer points to the 4th one
find 0 1          // Querying a connection (0 --> 1)
del 0             // Deleting 1st vertex
list -t           // Printing the graph with additional info
new -f            // Clearing the graph
cls               // Clearing the screen
exit              // Done
```
