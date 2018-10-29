# SecondGUIrepo
Qt project, "scientific programming"
A framework for EEG processing.
Includes:
matrix class for storing EEG data
edfFile class to read/write/edit data
myLib "library" with many useful functions
Classifier abstarct class and implemented some usual simple classifiers as derived classes
Net widget as GUI for classifying data
Cut widget for visual inspection/editing data
contains some ad hoc things, mainly in widgets/mainwindow_customF.cpp

Cut widget hotkeys:
O - open edf file
S - save file ("_new" is added to the  fileName)
A - prev file in current folder
D - next file in current folder
Q - scroll one frame backwards (also Mouse::Back and Shift + MouseWheel::Up)
E - scroll one frame forward (also Mouse::Forward and Shift + MouseWheel::Down)
Home or Shift + Mouse::Back - return to the beginning of the file
End or Shift + Mouse::Forward - move to the end of the file
MouseClick (namely, release) - set left/right edges of the interval
Z - zero the interval
X - erase the interval
Ctrl+X - cut the interval
Ctrl+C - copy the interval
Ctrl+V - paste the interval (where the left edge is)
Ctrl+Z - undo (should store the whole history on a file but can even crash the program)
C - save the interval
