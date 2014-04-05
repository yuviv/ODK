#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
 char* imageName = argv[1];

 Mat image;
 image = imread( imageName, 1 );

 if( argc != 2 || !image.data )
 {
   printf( " No image data \n " );
   return -1;
 }

 /*Mat gray_image;
 cvtColor( image, gray_image, CV_BGR2GRAY );*/
 Mat roi;
 Rect r(10, 10, 100, 100);
 roi = image(r);

 imwrite( "Cropped_Image.jpg", roi );

 //namedWindow( imageName, CV_WINDOW_AUTOSIZE );
 //namedWindow( "roi", CV_WINDOW_AUTOSIZE );

 imshow( imageName, image );
 imshow( "Cropped", roi );

 waitKey(0);

 return 0;
}
