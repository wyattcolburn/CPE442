//Include Libraries
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <pthread.h> 
// Namespaces
using namespace cv;
// Function Prototypes
struct MyStruct { 
    
    
    //init will all be the same frame, frame passed to struct 
    Mat orginal_frame; 
    Mat greyscale_frame; 
    Mat sobel_frame; 
    //starting coord for each thread 
    int x_coord;
    int y_coord;

    bool esc_flag;
    //the point of this is to create memory space once, and then it will overwritten but never created again, should improved performace
    MyStruct(int rows, int cols) : orginal_frame(rows, cols, CV_8UC3), greyscale_frame(rows, cols, CV_8UC1), sobel_frame(rows, cols, CV_8UC1) {
        // Additional initialization if needed...
    }
};


void thread_operation(MyStruct &thread1) {
    //grayscale operation portion of function
    //changes thread1.greyscale which sobel will eventaully use
    //thread1.greyscale_frame(thread1.orginal_frame.rows, thread1.orginal_frame.cols, CV_8UC1);
    for (int i = 0; i < thread1.orginal_frame.rows; ++i) 
    {
        for (int j = 0; j < thread1.orginal_frame.cols; ++j) 
        {
            Vec3b pixel = thread1.orginal_frame.at<Vec3b>(i, j);
            //weighted average to make greyscale pixel
            thread1.greyscale_frame.at<uchar>(i,j) = uchar(pixel[0] * .2126 + pixel[1] * .7152 + pixel[2] *.0722); 
 

        }
    }
    //sobel filter porition, 

    //thread1.sobel_frame(greyscale_color.rows, greyscale_color.cols, CV_8UC1);
    int sobel_x_matrix[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y_matrix[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    for (int i = 1; i < thread1.greyscale_frame.rows - 1; ++i) {
        for (int j = 1; j < thread1.greyscale_frame.cols - 1; ++j) {
            int sumX = 0;
            int sumY = 0;

            for (int m = -1; m <= 1; ++m) {
                for (int n = -1; n <= 1; ++n) {
                    sumX += thread1.greyscale_frame.at<uchar>(i + m, j + n) * sobel_x_matrix[m + 1][n + 1];
                    sumY += thread1.greyscale_frame.at<uchar>(i + m, j + n) * sobel_y_matrix[m + 1][n + 1];
                }
            }
            //write pixels of sobel_frame based on calculations above
            thread1.sobel_frame.at<uchar>(i, j) = saturate_cast<uchar>(abs(sumX) + abs(sumY));
        }
    }
}
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: image_viewer.out <Image_Path>\n");
        return -1;
    }
    VideoCapture cap(argv[1]);

    if (!cap.isOpened())
    {
        return 0;
    }

    while (1)
    {
        Mat frame;

        // Capture frame-by-frame and update orginal_frame in the struct
        cap >> frame;
        //create instance now that we know the size of the frame
        struct MyStruct thread1(frame.rows, frame.cols);
        thread1.orginal_frame = frame;
               
        thread1.esc_flag = true;
        // Call display function with the updated frame in the struct
        thread_operation(thread1);

                
        imshow("Frame", thread1.orginal_frame);
        imshow("Grey Frame", thread1.greyscale_frame);
        imshow("Sobel Filter", thread1.sobel_frame);

        // Wait for a short time (e.g., 25 ms) before the next frame
        char c = (char)waitKey(25);
        if (c == 27) {
            thread1.esc_flag = false;  // Break the loop if 'ESC' key is pressed
        }
    
        if (thread1.esc_flag == false)
        {
            break;
        }
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
