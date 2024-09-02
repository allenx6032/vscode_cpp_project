

#ifdef MAIN_TEST



#include "mcv.h"

#include <cstdlib>
#include <vector>
#include <fstream>

int main() {
	

	//mcv::Mat img;
	//mcv::Mat img2;
	
	
	//img = mcv::imread("picture/face.png");
	//
	//mcv::imwrite("picture/test_png_decode_and_bmp_encode.bmp", img);

	//mcv::imwrite("picture/test_png_encode.png", img);


	//img2 = mcv::imread("picture/test_png_decode_and_bmp_encode.bmp");
	//mcv::imwrite("picture/test_bmp_decode.png", img2);


	//img = mcv::imread("picture/yuanyuan.bmp", 0);
	//mcv::imwrite("picture/yuanyuan_for_cmp.bmp", img);
	//img2 = mcv::imread("picture/yuanyuan_for_cmp.bmp", 0);

	//std::cout << memcmp(img.data_, img2.data_, img.total()) << std::endl;

	//img = mcv::imread("picture/lena_gray.png", 0);
	//mcv::imwrite("picture/lena_for_cmp.png", img);
	//img2 = mcv::imread("picture/lena_for_cmp.png", 0);

	//std::cout << memcmp(img.data_, img2.data_, img.total()) << std::endl;

	/*std::cout << img.total() << std::endl;
	mcv::Smooth(img, img2, mcv::SMOOTH_LAP_MASK1);
	mcv::imwrite("picture/yuanyuan_smooth.bmp", img2);*/

	//img = mcv::imread("picture/lena.bmp", 0);
	//mcv::imwrite("picture/lena_gray.png", img);

	//mcv::NegativeImg(img, img2);
	//mcv::imwrite("picture/lena_neg.bmp", img2);

	//mcv::CompressGray(img, img2);
	//mcv::imwrite("picture/lena_compress.bmp", img2);

	//mcv::Smooth(img, img2, mcv::SMOOTH_LAP_MASK1);
	//mcv::imwrite("picture/lena_smooth.bmp", img2);

	//img = mcv::imread("test_pic/cameraman.bmp", 0);
	//std::cout << img.channels() << "   " << img.total() << std::endl;

	//mcv::imwrite("test_pic/cameraman_bmp.bmp", img);

	//system("dir /b /d test_pic\\*.bmp > test_pic\\bmp.txt");

	//std::vector<std::string> filenames;
	//std::string filename;

	//std::ifstream file("test_pic/bmp.txt");

	//while (!file.eof()) {
	//	file >> filename;
	//	filenames.push_back(filename);
	//}

	//std::string tmp;
	//for (auto i = filenames.begin(); i != filenames.end(); ++i) {
	//	img = mcv::imread("test_pic/" + *i, 0);

		//tmp = "test_result/" + i->substr(0, i->find('.'));
		//mcv::imwrite(tmp + ".png", img);

		//mcv::NegativeImg(img, img2);
		//filename = tmp + "_neg.bmp";
		//mcv::imwrite(filename, img2);

		//mcv::CompressGray(img, img2);
		//filename = tmp + "_com.bmp";
		//mcv::imwrite(filename, img2);

		//mcv::Smooth(img, img2, mcv::SMOOTH_LAP_MASK1);
		//mcv::SmoothNosse(img, img2, mcv::SMOOTH_LAP_MASK1);
		//filename = tmp + "_smooth_lap1.bmp";
		//mcv::imwrite(filename, img2);
	//}


	/*for (int i = 0; i < 10; ++i) {
		mcv::Smooth(img, img2, mcv::SMOOTH_LAP_MASK1);
		mcv::SmoothNosse(img, img2, mcv::SMOOTH_LAP_MASK1);
	}*/

	return 0;
}


#endif // MAIN_TEST
