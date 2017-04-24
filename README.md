# Image-Segmentation

Contains C++ codes for image segmentation i.e. dividing image into segments which are similar.

Libraries used are openCV(for reading and saving image) and openMP(for parallelizing the program)
Contents are-

1. seg.cpp- Parallel implementation of Boruvka's algorithm for image segmentation (Faster).

2. seg2.cpp- Serialized implementation of Boruvka's algorithm for image segmentation (Better Results for most images).

3. seg4.cpp- Parallel implementation of Boruvka's algorithm for image segmentation (much slower due to locks).

4. hist_seg.cpp- Parallel implementation of histogram thresholding method for image segmentation.

5. imgcmp.cpp- A program to compare two images. The output is a completely black image if the images are similar else at each point of difference there will be white color.

6. otsu.cpp- Serialized implementation of Otsu's method for image segmentation.

7. kmeans.cpp- 
