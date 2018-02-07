
#include "stream.h"

namespace mc
{
	namespace stream
	{

		// ==========================================================================================================================
		//
		// image encodings enum
		//
		// ==========================================================================================================================


		std::string getEncoding(const ImageEncodings& encodings)
		{
			switch (encodings)
			{
			case ImageEncodings::JPEG:
				return{ "*.jpg" };
			case ImageEncodings::JPEG_2000:
				return{ "*.jp2" };
			case ImageEncodings::PNG:
				return{ "*.png" };
			case ImageEncodings::PORTABLE_IMAGE_FORMAT:
				return{ "*.pbm" };
			case ImageEncodings::SUN_RASTERS:
				return{ "*.ras" };
			case ImageEncodings::TIFF:
				return{ "*.tif" };
			default:
				return{ "*.jpg" };
			}
		}


		// ==========================================================================================================================
		//
		// video stream parameter
		//
		// ==========================================================================================================================


		void VideoStreamParameter::read(const cv::FileNode& fn)
		{
			fn["port"] >> port;
			fn["period"] >> period;
			fn["width"] >> width;
			fn["height"] >> height;			
			fn["encoding"] >> encoding;
		}


		void VideoStreamParameter::write(cv::FileStorage& fs) const
		{
			fs << "{"
				<< "port" << port
				<< "period" << period
				<< "width" << width
				<< "height" << height
				<< "encoding" << encoding
				<< "}";
		}


		void read(const cv::FileNode& fn, VideoStreamParameter& streamPara, const VideoStreamParameter& default)
		{
			if (fn.empty())
				streamPara = default;
			else
				streamPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const VideoStreamParameter& streamPara)
		{
			streamPara.write(fs);
		}


		bool saveVideoStreamParameter(const VideoStreamParameter& streamPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << streamPara;
				return true;
			}

			return false;
		}


		bool readVideoStreamParameter(VideoStreamParameter& streamPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> streamPara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// video stream server
		//
		// ==========================================================================================================================


		VideoStreamServer::~VideoStreamServer()
		{
			ptr_io_service->stop();
			server_thread.join();
		}


		bool VideoStreamServer::storeMat(const cv::Mat& image)
		{
			if (image.empty() && image.channels() != 3)
				return false;

			cv::resize(image, dmy, { parameter.width, parameter.height });

			cv::imencode(parameter.encoding, dmy, shadow);

			std::lock_guard<std::mutex> guard(mtx);
			std::swap(buffer, shadow);

			return true;
		}


		const VideoStreamParameter& VideoStreamServer::getParameter() const
		{
			return parameter;
		}


		VideoStreamParameter& VideoStreamServer::getParameter()
		{
			return parameter;
		}


		void VideoStreamServer::server_function()
		{
			try
			{
				start_accept();
				ptr_io_service->run();
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}


		void VideoStreamServer::start_accept()
		{
			ptr_socket.reset(new boost::asio::ip::tcp::socket(*ptr_io_service));
			ptr_acceptor->async_accept(*ptr_socket,
				boost::bind(&VideoStreamServer::handle_accept, this,
					boost::asio::placeholders::error));
		}


		void VideoStreamServer::send_stream()
		{
			ptr_timer.reset(new boost::asio::deadline_timer(*ptr_io_service, boost::posix_time::milliseconds(parameter.period)));
			ptr_timer->async_wait(boost::bind(&VideoStreamServer::handle_timer, this, boost::asio::placeholders::error));
		}


		void VideoStreamServer::send_frame()
		{
			ptr_timer->expires_at(ptr_timer->expires_at() + boost::posix_time::milliseconds(parameter.period));

			std::lock_guard<std::mutex> guard(mtx);
			boost::asio::async_write(*ptr_socket, boost::asio::buffer(buffer),
				boost::bind(&VideoStreamServer::handle_write, this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}


		void VideoStreamServer::handle_accept(const boost::system::error_code& error)
		{
			if (!error)
				send_stream();
			else
				start_accept();
		}


		void VideoStreamServer::handle_timer(const boost::system::error_code& error)
		{
			if (!error)
				send_frame();
			else
				start_accept();
		}


		void VideoStreamServer::handle_write(const boost::system::error_code& error,
			size_t bytes_transferred)
		{
			if (!error)
				ptr_timer->async_wait(boost::bind(&VideoStreamServer::handle_timer, this, boost::asio::placeholders::error));
			else
				start_accept();
		}


		// ==========================================================================================================================
		//
		// image and contours stream helper functions
		//
		// ==========================================================================================================================


		bool reduceContour(const std::vector<cv::Point>& input, std::vector<cv::Point>& output, int take_every)
		{
			if (input.empty())
				return false;

			output.clear();
			output.reserve(input.size() / take_every + 10);

			for (auto&& c = 0; c < input.size(); ++c)
				if (c % take_every == 0)
					output.push_back(input[c]);

			return true;
		}


		// ==========================================================================================================================
		//
		// image and contours stream parameter
		//
		// ==========================================================================================================================


		void ImageAndContoursStreamParameter::read(const cv::FileNode& fn)
		{
			fn["port"] >> port;
			fn["period"] >> period;
			fn["width"] >> width;
			fn["height"] >> height;
			fn["reduce"] >> reduce;
			fn["encoding"] >> encoding;
		}


		void ImageAndContoursStreamParameter::write(cv::FileStorage& fs) const
		{
			fs << "{"
				<< "port" << port
				<< "period" << period
				<< "width" << width
				<< "height" << height
				<< "reduce" << reduce
				<< "encoding" << encoding
				<< "}";
		}


		void read(const cv::FileNode& fn, ImageAndContoursStreamParameter& streamPara, const ImageAndContoursStreamParameter& default)
		{
			if (fn.empty())
				streamPara = default;
			else
				streamPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const ImageAndContoursStreamParameter& streamPara)
		{
			streamPara.write(fs);
		}


		bool saveImageAndContoursStreamParameter(const ImageAndContoursStreamParameter& streamPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << streamPara;
				return true;
			}

			return false;
		}


		bool readImageAndContoursStreamParameter(ImageAndContoursStreamParameter& streamPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> streamPara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// image and contours stream server
		//
		// ==========================================================================================================================


		ImageAndContoursStreamServer::~ImageAndContoursStreamServer()
		{
			ptr_io_service->stop();
			server_thread.join();
		}


		bool ImageAndContoursStreamServer::storeData(const cv::Mat& image, const std::vector<std::vector<cv::Point>>& contours)
		{
			if (image.empty() && image.channels() != 3)
				return false;


			// may be we need to specify the buffer capacity within the parameter ... but his is actually some think which is not part of the encoding ... ?
			// otherwise ... the sending period is defined within the streaming parameter ...

			shadow.clear();
			shadow.reserve(250000);

			//

			// here comes our protocol
			alignStream<4>(shadow);

			cv::resize(image, dmyImage, { parameter.width, parameter.height });
			cv::imencode(parameter.encoding, dmyImage, dmyImageBuffer);

			appendEncodedImage<4, 4>(shadow, dmyImageBuffer);

			alignStream<4>(shadow);

			appendSize<4>(shadow, contours.size());

			for (auto& it : contours)
			{
				reduceContour(it, dmyContour, parameter.reduce);
				appendContour<4, 2, 4>(shadow, dmyContour);
			}

			//

			std::lock_guard<std::mutex> guard(mtx);
			std::swap(buffer, shadow);

			return true;
		}


		const ImageAndContoursStreamParameter& ImageAndContoursStreamServer::getParameter() const
		{
			return parameter;
		}


		ImageAndContoursStreamParameter& ImageAndContoursStreamServer::getParameter()
		{
			return parameter;
		}


		void ImageAndContoursStreamServer::server_function()
		{
			try
			{
				start_accept();
				ptr_io_service->run();
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}


		void ImageAndContoursStreamServer::start_accept()
		{
			ptr_socket.reset(new boost::asio::ip::tcp::socket(*ptr_io_service));
			ptr_acceptor->async_accept(*ptr_socket,
				boost::bind(&ImageAndContoursStreamServer::handle_accept, this,
					boost::asio::placeholders::error));
		}


		void ImageAndContoursStreamServer::send_stream()
		{
			ptr_timer.reset(new boost::asio::deadline_timer(*ptr_io_service, boost::posix_time::milliseconds(parameter.period)));
			ptr_timer->async_wait(boost::bind(&ImageAndContoursStreamServer::handle_timer, this, boost::asio::placeholders::error));
		}


		void ImageAndContoursStreamServer::send_frame()
		{
			ptr_timer->expires_at(ptr_timer->expires_at() + boost::posix_time::milliseconds(parameter.period));

			std::lock_guard<std::mutex> guard(mtx);
			boost::asio::async_write(*ptr_socket, boost::asio::buffer(buffer),
				boost::bind(&ImageAndContoursStreamServer::handle_write, this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}


		void ImageAndContoursStreamServer::handle_accept(const boost::system::error_code& error)
		{
			if (!error)
				send_stream();
			else
				start_accept();
		}


		void ImageAndContoursStreamServer::handle_timer(const boost::system::error_code& error)
		{
			if (!error)
				send_frame();
			else
				start_accept();
		}


		void ImageAndContoursStreamServer::handle_write(const boost::system::error_code& error,
			size_t bytes_transferred)
		{
			if (!error)
				ptr_timer->async_wait(boost::bind(&ImageAndContoursStreamServer::handle_timer, this, boost::asio::placeholders::error));
			else
				start_accept();
		}

	}
}