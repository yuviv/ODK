For Bubble/Checkbox Classification:
==========
SAMPLE IMAGES SETUP
I place all the empty images in the "empty" folder and filled ones in the "filled" folder.
Knowing the correct classification for % correctness and for training depends on the file path so we must place images correctly into each one.
There are some sub folders inside filled to distinguish between the different people who have filled out each form, but these are not necessary.

TO USE
If via command line, must have the command line argument be the root directory of the images we want to classify.
However, there may not be other folders inside it currently. For example, we can test it on "filled/f5" but not "filled". 
If no command line arguments are supplied, it will just run it on the root directory supplied in the first few lines of the program.

TO CLASSIFY WITH PCA_SVM VS PIXEL THRESHOLDING
Set the macro USE_PCA at the top to true or false to determine whether or not you'd like to classify using PCA.

RESULTS
PCA results are the most updated for both checkboxes and bubble classification. Stored in resultsPCA.txt in the respective projects