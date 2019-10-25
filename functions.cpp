#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel ** createImage(int width, int height) {
    cout << "Start createImage... " << endl;

    // Create a one dimensional array on the heap of pointers to Pixels 
    //    that has width elements (i.e. the number of columns)
    Pixel ** image = new Pixel * [width];

    bool fail = false;

    for (int i = 0; i < width; ++i) { // loop through each column
        // assign that column to a one dimensional array on the heap of Pixels
        //  that has height elements (i.e. the number of rows)
        image[i] = new Pixel[height];

        if (image[i] == nullptr) { // failed to allocate
            fail = true;
        }
    }

    if (fail) { // if any allocation fails, clean up and avoid memory leak
        // deallocate any arrays created in for loop
        for (int i = 0; i < width; ++i) {
            delete[] image[i]; // deleting nullptr is not a problem
        }
        delete[] image; // delete array of pointers
        return nullptr;
    }

    // initialize cells
    // cout << "Initializing cells..." << endl;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            //cout << "(" << col << ", " << row << ")" << endl;
            image[col][row] = {0, 0, 0};
        }
    }
    cout << "End createImage... " << endl;
    return image;
}

void deleteImage(Pixel ** image, int width) {
    cout << "Start deleteImage..." << endl;
    // avoid memory leak by deleting the array
    for (int i = 0; i < width; ++i) {
        delete[] image[i]; // delete each individual array placed on the heap
    }
    delete[] image;
    image = nullptr;
}

int * createSeam(int length) {
    int * seam = new int[length];
    for (int i = 0; i < length; i++) {
        seam[i] = 0;
    }
    return seam;
}

void deleteSeam(int * seam) {
    delete[] seam;
}

bool loadImage(string filename, Pixel ** image, int width, int height) {
    // declare/define and open input file stream
    ifstream ifs(filename);

    // check if input stream opened successfully
    if (!ifs.is_open()) {
        cout << "Error: failed to open input file - " << filename << endl;
        return false;
    }

    // get type from preamble
    char type[3];
    ifs >> type; // should be P3
    if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
        cout << "Error: type is " << type << " instead of P3" << endl;
        return false;
    }

    // Get width (w) and height (h) from preamble.
    int w = 0, h = 0;
    ifs >> w >> h;
    if (!cin.good()) {
            cout << "Error: read a non-integer value" << endl;
            return false;
    }
    if (w != width) { // Check that width matches what was passed into the function
        cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;
        return false;
    }
    if (h != height) { // Check that height matches what was passed into the function
        cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
        return false;
    }

    // Get maximum value from preamble.
    int colorMax = 0;
    ifs >> colorMax;
    if (!cin.good()) {
        cout << "Error: read a non-integer value" << endl;
        return false;
    }
    if (colorMax != 255) {
        cout << "Error: file is not using RGB color values." << endl;
        return false;
    }

    // Read the RGB values and store them in the 2D array of Pixels.
    int r, g, b;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            // Take in RGB values and verify that there is enough color values in the file.
            ifs >> r >> g >> b;
            if (ifs.fail() && !ifs.eof()) {
                cout << "Error: read non-integer value" << endl;
                return false;
            }

            if (ifs.fail()) {
                cout << "Error: not enough color values" << endl;
                return false;
            }

            image[j][i].r = r;
            image[j][i].g = g;
            image[j][i].b = b;

            // Verify that each RGB value is valid.
            if (image[j][i].r > 255 || image[j][i].r < 0) {
                cout << "Error: invalid color value " << image[j][i].r << endl;
                return false;
            }
            if (image[j][i].g > 255 || image[j][i].g < 0) {
                cout << "Error: invalid color value " << image[j][i].g << endl;
                return false;
            }
            if (image[j][i].b > 255 || image[j][i].b < 0) {
                cout << "Error: invalid color value " << image[j][i].b << endl;
                return false;
            }
        }
    }

    while (!ifs.eof()) {
        string test;
        ifs >> test;
        if (isValidDigit(test) && test.size() != 0) {
            cout << "Error: too many color values" << endl;
            return false;
        }
    }

    return true;
}

bool outputImage(string filename, Pixel ** image, int width, int height) {
    ofstream ofs(filename);
    // check if output stream opened successfully
    if (ofs.is_open()) {
        // output preamble
        ofs << "P3\n" << width << " " << height << endl << "255" << endl;
        // output pixels
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                ofs << image[j][i].r << " " << image[j][i].g << " " << image[j][i].b << " ";
            }
            ofs << endl;
        }
    } else {
        cout << "Error: failed to open output file - " << filename << endl;
    }

    ofs.close();
    return true;
}

int energy(Pixel ** image, int x, int y, int width, int height) {
    int xEnergy = 0;
    int yEnergy = 0;

    // Each of these declarations gets the correct adjacent element.
    int xLeft = (x - 1 < 0) ? width - 1 : x - 1;
    int xRight = (x + 1 >= width) ? 0 : x + 1;
    int yUp = (y - 1 < 0) ? height - 1 : y - 1;
    int yDown = (y + 1 >= height) ? 0 : y + 1;

    // Calculate total energy along the horizontal by taking the sum of the adjacent squared RBG deltas.
    xEnergy += pow(abs(image[xLeft][y].r - image[xRight][y].r), 2);
    xEnergy += pow(abs(image[xLeft][y].g - image[xRight][y].g), 2);
    xEnergy += pow(abs(image[xLeft][y].b - image[xRight][y].b), 2);

    // Calculate total energy along the veritcal by taking the sum of the adjacent squared RBG deltas.
    yEnergy += pow(abs(image[x][yUp].r - image[x][yDown].r), 2);
    yEnergy += pow(abs(image[x][yUp].g - image[x][yDown].g), 2);
    yEnergy += pow(abs(image[x][yUp].b - image[x][yDown].b), 2);

    return xEnergy + yEnergy;
}

bool isValidDigit(string str) {
    for (unsigned int i = 0; i < str.size(); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
    seam[0] = start_col;
    int totalEnergy = energy(image, start_col, 0, width, height);
    int currColumn = start_col;

    for (int i = 1; i < height; i++) {
        int downEnergy = energy(image, currColumn, i, width, height); 
        int leftEnergy = (currColumn + 1 < width) ? energy(image, currColumn + 1, i, width, height) : 2147483647; 
        int rightEnergy = (currColumn - 1 >= 0) ? energy(image, currColumn - 1, i, width, height) : 2147483647;

        // Set the seam to the pixel with least amount of energy.
        if (downEnergy < leftEnergy && downEnergy < rightEnergy) {
            seam[i] = currColumn;
            totalEnergy += downEnergy;
        } else if (leftEnergy < downEnergy && leftEnergy < rightEnergy) {
            seam[i] = currColumn + 1;
            currColumn++;
            totalEnergy += leftEnergy;
        } else if (rightEnergy < downEnergy && rightEnergy < leftEnergy) {
            seam[i] = currColumn - 1;
            currColumn--;
            totalEnergy += rightEnergy;
        } else if (downEnergy == leftEnergy || downEnergy == rightEnergy) {
            seam[i] = currColumn;
            totalEnergy += downEnergy;
        } else if (leftEnergy == rightEnergy) {
            seam[i] = currColumn + 1;
            currColumn++;
            totalEnergy += leftEnergy;
        }
    }

    return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
    seam[0] = start_row;
    int totalEnergy = energy(image, 0, start_row, width, height);
    int currRow = start_row;

    for (int i = 1; i < width; i++) {
        int directEnergy = energy(image, i, currRow, width, height);
        // If the index is valid, calculate the energies. Otherwise, set the energies to maximum integer value.
        int leftEnergy = (currRow - 1 >= 0) ? energy(image, i, currRow - 1, width, height) : 2147483647;
        int rightEnergy = (currRow + 1 < height) ? energy(image, i, currRow + 1, width, height) : 2147483647;
        // Setting the invalid index energies to integer max values prohibits them from every being selected as the min energy.

        // Set the seam to the pixel with least amount of energy.
        if (directEnergy < leftEnergy && directEnergy < rightEnergy) {
            seam[i] = currRow;
            totalEnergy += directEnergy;
        } else if (leftEnergy < directEnergy && leftEnergy < rightEnergy) {
            seam[i] = currRow - 1;
            currRow--;
            totalEnergy += leftEnergy;
        } else if (rightEnergy < directEnergy && rightEnergy < leftEnergy) {
            seam[i] = currRow + 1;
            currRow++;
            totalEnergy += rightEnergy;
        } else if (directEnergy == leftEnergy || directEnergy == rightEnergy) {
            seam[i] = currRow;
            totalEnergy += directEnergy;
        } else if (leftEnergy == rightEnergy) {
            seam[i] = currRow - 1;
            currRow--;
            totalEnergy += leftEnergy;
        }
    }

    return totalEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
    int* minSeam = new int[height];
    int minSeamEnergy = loadVerticalSeam(image, 0, width, height, minSeam);
    int nextVerticalSeamEnergy;

    for (int i = 1; i < width; i++) {
        int* nextSeam = new int[height];
        nextVerticalSeamEnergy = loadVerticalSeam(image, i, width, height, nextSeam);

        if (minSeamEnergy > nextVerticalSeamEnergy) {
            minSeamEnergy = nextVerticalSeamEnergy;
            for (int i = 0; i < height; i++) {
                minSeam[i] = nextSeam[i];
            }
        }
        delete[] nextSeam;
    }

    return minSeam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
    int* minSeam = new int[width];
    int minSeamEnergy = loadHorizontalSeam(image, 0, width, height, minSeam);
    int nextHorizontalSeamEnergy; 
    
    for (int i = 1; i < height; i++) {
        int* nextSeam = new int[width];
        nextHorizontalSeamEnergy = loadHorizontalSeam(image, i, width, height, nextSeam);

        if (minSeamEnergy > nextHorizontalSeamEnergy) {
            minSeamEnergy = nextHorizontalSeamEnergy;
            for (int i = 0; i < width; i++) {
                minSeam[i] = nextSeam[i];
            }
        }
        delete[] nextSeam;
    }

    return minSeam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
    for (int i = 0; i < height; i++) {
        for (int j = verticalSeam[i]; j < width - 1; j++) {
            image[j][i] = image[j + 1][i];
        }
    }
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
    for (int i = 0; i < width; i++) {
        for (int j = horizontalSeam[i]; j < height - 1; j++) {
            image[i][j] = image[i][j + 1];
        }
    }
}
