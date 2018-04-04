#include <iostream>
#include <algorithm>

#include <opencv2\highgui.hpp>
#include <opencv2\core.hpp>

#define MAX_DISP 96

void compute_disp_NCC(cv::Mat &, cv::Mat &, cv::Mat &);

int main(int argc, char** argv)
{
	cv::Mat speckle_mat = cv::imread("D:\\vs2015project\\opencv_project\\image_rectifity\\image_rectifity\\speckle.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat ref_mat = cv::imread("D:\\vs2015project\\opencv_project\\image_rectifity\\image_rectifity\\ref.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat disp_mat;

	compute_disp_NCC(speckle_mat, ref_mat, disp_mat);

	IplImage image(disp_mat);
	cvShowImage("image", &image);
	cvWaitKey(0);
}

void compute_disp_NCC(cv::Mat &speckle_mat, cv::Mat &ref_mat, cv::Mat &disp_mat)
{
	disp_mat.create(cv::Size(speckle_mat.cols - 8, speckle_mat.rows - 8), CV_8UC1);

	double* cost = new double[disp_mat.rows*disp_mat.cols*MAX_DISP];
	

	for (int row = 0; row < disp_mat.rows; row++)
	{
		for (int col = 0; col < disp_mat.cols; col++)
		{
			for (int d = 0; d < MAX_DISP && col - d >= 0; d++)
			{
				int sum_speckle = 0;
				int sum_ref = 0;
				int sum_diff = 0;
				double sum_square_diff_speckle = 0;
				double sum_square_diff_ref = 0;
				for (int i = -4; i <= 4; i++)
				{
					for (int j = -4; j <= 4; j++)
					{
						sum_speckle += speckle_mat.at<uchar>(row + 4 + i, col + 4 + j);
						sum_ref += ref_mat.at<uchar>(row + 4 + i, col + 4 + j - d);
					}
				}

				int ave_speckle = sum_speckle / (9 * 9);
				int ave_ref = sum_ref / (9 * 9);

				for (int i = -4; i <= 4; i++)
				{
					for (int j = -4; j <= 4; j++)
					{
						sum_diff += (speckle_mat.at<uchar>(row + 4 + i, col + 4 + j) - ave_speckle)
							*(ref_mat.at<uchar>(row + 4 + i, col + 4 + j - d) - ave_ref);
						sum_square_diff_speckle += pow(speckle_mat.at<uchar>(row + 4 + i, col + 4 + j) - ave_speckle, 2);
						sum_square_diff_ref +=	pow(ref_mat.at<uchar>(row + 4 + i, col + 4 + j - d) - ave_ref, 2);
					}
				}

				cost[row*disp_mat.cols*MAX_DISP + col*MAX_DISP + d] 
					= static_cast<double>(sum_diff) / sqrt(sum_square_diff_speckle*sum_square_diff_ref);
			}
		}
	}

	for (int row = 0; row < disp_mat.rows; row++)
	{
		for (int col = 0; col < disp_mat.cols; col++)
		{
			for (int d = 0; d < MAX_DISP && col - d >= 0; d++)
			{
				double temp_cost = 0;
				if (d == 0)
				{
					temp_cost = cost[row*disp_mat.cols*MAX_DISP + col*MAX_DISP + d];
					disp_mat.at<uchar>(row, col) = d;
				}
				if (cost[row*disp_mat.cols*MAX_DISP + col*MAX_DISP + d] > temp_cost)
				{
					temp_cost = cost[row*disp_mat.cols*MAX_DISP + col*MAX_DISP + d];
					disp_mat.at<uchar>(row, col) = d;
				}
			}
		}
	}
}