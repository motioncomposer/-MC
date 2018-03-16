#pragma once

#include "opencv2\core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2\highgui.hpp"

#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <memory>

#include <boost\asio.hpp>
#include <boost\bind.hpp>

// this is need for append / read fundamental type --> may be we change this later
#include "core.h"

#include "structures.h"


namespace mc
{
	namespace stream
	{

		// ==========================================================================================================================
		//
		// image encodings enum
		//
		// ==========================================================================================================================

		// for what do I need this structure?

		enum class ImageEncodings : int
		{
			JPEG = 0,
			JPEG_2000 = 1,
			PNG = 2,
			PORTABLE_IMAGE_FORMAT = 3,
			SUN_RASTERS = 4,
			TIFF = 5
		};


		std::string getEncoding(const ImageEncodings& encodings);


		// ==========================================================================================================================
		//
		// video stream parameter
		//
		// ==========================================================================================================================


		struct VideoStreamParameter
		{
			int port;

			int period;
			int width;
			int height;

			std::string encoding;

			VideoStreamParameter(int port = 50000, int period = 100, int width = 480, int height = 360, int encoding = 0) : port(port), period(period), width(width), height(height),
				encoding(getEncoding(static_cast<ImageEncodings>(encoding)))
			{}

			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;
		};


		void read(const cv::FileNode& fn, VideoStreamParameter& streamPara, const VideoStreamParameter& default = VideoStreamParameter());

		void write(cv::FileStorage& fs, const std::string&, const VideoStreamParameter& streamPara);

		bool saveVideoStreamParameter(const VideoStreamParameter& streamPara,
			const std::string& filename, const std::string& key);

		bool readVideoStreamParameter(VideoStreamParameter& streamPara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// video stream server
		//
		// ==========================================================================================================================


		// this streaming server only sends a single cv::Mat to a client --> see mc::stream::DataStreamServer in order to send
		// custom data
		class VideoStreamServer
		{
		public:

			VideoStreamServer(const VideoStreamParameter& param) : parameter(param), ptr_io_service(new boost::asio::io_service),
				ptr_acceptor(new boost::asio::ip::tcp::acceptor(*ptr_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), param.port))),
				ptr_socket(nullptr), ptr_timer(nullptr)
			{
				server_thread = std::thread(&VideoStreamServer::server_function, this);
			}


			~VideoStreamServer();

			bool storeMat(const cv::Mat& image);

			const VideoStreamParameter& getParameter() const;

			VideoStreamParameter& getParameter();


		private:

			std::string port;

			VideoStreamParameter parameter;

			std::shared_ptr<boost::asio::deadline_timer> ptr_timer;

			std::shared_ptr<boost::asio::io_service> ptr_io_service;
			std::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_acceptor;
			std::shared_ptr<boost::asio::ip::tcp::socket> ptr_socket;

			std::mutex mtx;
			std::vector<uchar> buffer;
			std::vector<uchar> shadow;

			cv::Mat dmy;

			std::thread server_thread;


			void server_function();


			void start_accept();

			void send_stream();

			void send_frame();



			void handle_accept(const boost::system::error_code& error);

			void handle_timer(const boost::system::error_code& error);

			void handle_write(const boost::system::error_code& error, size_t bytes_transferred);


		};


		// ==========================================================================================================================
		//
		// server helper templates to append data to buffer
		//
		// ==========================================================================================================================


		// this is actually not needed
		template<typename _Tp>
		void alignStream(std::vector<uchar>& bytes)
		{
			static_assert(std::is_fundamental<_Tp>::value, "datatype is not fundamental");

			const size_t align_size = sizeof(_Tp);

			static_assert(align_size != 1, "not allowed align size specifier");

			while (bytes.size() % align_size != 0)
				bytes.push_back('\0');
		}


		template<size_t align_size>
		void alignStream(std::vector<uchar>& bytes)
		{
			static_assert(align_size == 2 || align_size == 4 ||
				align_size == 8, "not allowed align size specifier");

			while (bytes.size() % align_size != 0)
				bytes.push_back('\0');
		}


		template<size_t type_size>
		void appendSize(std::vector<uchar>& bytes, size_t input)
		{
			static_assert(type_size == 2 || type_size == 4 ||
				type_size == 8, "not allowed type size specifier");

			using T = std::conditional<type_size == 8, uint64_t,
				std::conditional<type_size == 4, uint32_t, uint16_t>::type>::type;

			(void)fs::core::appendFundamentalType<T>(bytes, static_cast<T>(input));
		}


		template<size_t type_size>
		void appendPoint(std::vector<uchar>& bytes, const cv::Point& input)
		{
			static_assert(type_size == 2 || type_size == 4 ||
				type_size == 8, "not allowed type size specifier");

			using T = std::conditional<type_size == 8, int64_t,
				std::conditional<type_size == 4, int32_t, int16_t>::type>::type;

			(void)fs::core::appendFundamentalType<T>(bytes, static_cast<T>(input.x));
			(void)fs::core::appendFundamentalType<T>(bytes, static_cast<T>(input.y));
		}


		template<size_t type_size>
		void appendPoints(std::vector<uchar>& bytes, const std::vector<cv::Point>& input)
		{
			static_assert(type_size == 2 || type_size == 4 ||
				type_size == 8, "not allowed type size specifier");

			using T = std::conditional<type_size == 8, int64_t,
				std::conditional<type_size == 4, int32_t, int16_t>::type>::type;

			for (auto& it : input)
			{
				(void)fs::core::appendFundamentalType<T>(bytes, static_cast<T>(it.x));
				(void)fs::core::appendFundamentalType<T>(bytes, static_cast<T>(it.y));
			}
		}


		template<size_t size_type_size, size_t point_type_size, size_t align_size>
		void appendContour(std::vector<uchar>& bytes, const std::vector<cv::Point>& input)
		{
			appendSize<size_type_size>(bytes, input.size());

			alignStream<align_size>(bytes);

			appendPoints<point_type_size>(bytes, input);

			alignStream<align_size>(bytes);
		}


		template<size_t size_type_size, size_t align_size>
		void appendEncodedImage(std::vector<uchar>& bytes, const std::vector<uchar>& input)
		{
			appendSize<size_type_size>(bytes, input.size());

			alignStream<align_size>(bytes);

			for (auto& it : input)
				bytes.push_back(it);

			alignStream<align_size>(bytes);
		}


		// ==========================================================================================================================
		//
		// server helper templates to read data from buffer
		//
		// ==========================================================================================================================


		// this is actually not needed
		template<typename _Tp>
		void alignPointer(size_t& pos)
		{
			static_assert(std::is_fundamental<_Tp>::value, "datatype is not fundamental");

			const size_t align_size = sizeof(_Tp);

			static_assert(align_size != 1, "not allowed align size specifier");

			while (pos % align_size != 0)
				++pos;
		}


		template<size_t align_size>
		void alignPointer(size_t& pos)
		{
			static_assert(align_size == 2 || align_size == 4 ||
				align_size == 8, "not allowed align size specifier");

			while (pos % align_size != 0)
				++pos;
		}


		template<size_t type_size>
		size_t readSize(const std::vector<uchar>& bytes, size_t& pos)
		{
			static_assert(type_size == 2 || type_size == 4 ||
				type_size == 8, "not allowed type size specifier");

			using T = std::conditional<type_size == 8, uint64_t,
				std::conditional<type_size == 4, uint32_t, uint16_t>::type>::type;

			// we need a custom exception
			if (pos + type_size > bytes.size()) throw std::out_of_range("invalid vector<T> subscript");

			return static_cast<size_t>(fs::core::readFundamentalType<T>(bytes, pos));
		}


		template<size_t type_size>
		void readPoints(const std::vector<uchar>& bytes, std::vector<cv::Point>& output, size_t amount, size_t& pos)
		{
			static_assert(type_size == 2 || type_size == 4 ||
				type_size == 8, "not allowed type size specifier");

			using T = std::conditional<type_size == 8, int64_t,
				std::conditional<type_size == 4, int32_t, int16_t>::type>::type;

			// we need a custom exeption
			if (pos + (type_size * amount) > bytes.size()) throw std::out_of_range("invalid vector<T> subscript");
			//if (pos + (type_size * amount) > bytes.size()) throw std::out_of_range("wubba lubba dub dub");

			output.clear();
			output.reserve(amount);

			for (auto&& c = 0; c < amount; ++c)
			{
				output.push_back({
					static_cast<int32_t>(fs::core::readFundamentalType<T>(bytes, pos)),
					static_cast<int32_t>(fs::core::readFundamentalType<T>(bytes, pos))
				});
			}
		}


		template<size_t size_type_size, size_t point_type_size, size_t align_size>
		void readContour(const std::vector<uchar>& bytes, std::vector<cv::Point>& output, size_t& pos)
		{
			size_t amount = readSize<size_type_size>(bytes, pos);
			alignPointer<align_size>(pos);

			readPoints<point_type_size>(bytes, output, amount, pos);
			alignPointer<align_size>(pos);
		}


		template<size_t size_type_size, size_t align_size>
		void readEncodedImage(const std::vector<uchar>& bytes, std::vector<uchar>& output, size_t& pos)
		{
			size_t amount = readSize<size_type_size>(bytes, pos);
			alignPointer<align_size>(pos);

			output.clear();
			output.reserve(amount);

			// we need a custom exeption
			if (pos + amount > bytes.size()) throw std::out_of_range("invalid vector<T> subscript");

			for (auto&& c = 0; c < amount; ++c)
				output.push_back(bytes[pos + c]);

			pos += amount;

			alignPointer<align_size>(pos);
		}


		// ==========================================================================================================================
		//
		// image and contours stream helper functions
		//
		// ==========================================================================================================================


		bool reduceContour(const std::vector<cv::Point>& input, std::vector<cv::Point>& output, int take_every);


		// ==========================================================================================================================
		//
		// image and contours stream parameter
		//
		// ==========================================================================================================================


		struct ImageAndContoursStreamParameter
		{
			int port;

			int period;
			int reduce;

			float scale;

			std::string encoding;

			ImageAndContoursStreamParameter(int port = 50000, int period = 100, int reduce = 5, float scale = 0.75f, int encoding = 0) : port(port), period(period),
				reduce(reduce), scale(scale), encoding(getEncoding(static_cast<ImageEncodings>(encoding)))
			{}

			void read(const cv::FileNode& fn);

			void write(cv::FileStorage& fs) const;
		};


		void read(const cv::FileNode& fn, ImageAndContoursStreamParameter& streamPara, const ImageAndContoursStreamParameter& default = ImageAndContoursStreamParameter());

		void write(cv::FileStorage& fs, const std::string&, const ImageAndContoursStreamParameter& streamPara);

		bool saveImageAndContoursStreamParameter(const ImageAndContoursStreamParameter& streamPara,
			const std::string& filename, const std::string& key);

		bool readImageAndContoursStreamParameter(ImageAndContoursStreamParameter& streamPara,
			const std::string& filename, const std::string& key);


		// ==========================================================================================================================
		//
		// image and contours stream server
		//
		// ==========================================================================================================================


		// we can make this more generic ...
		class ImageAndContoursStreamServer
		{
		public:

			ImageAndContoursStreamServer(const ImageAndContoursStreamParameter& param) : parameter(param), ptr_io_service(new boost::asio::io_service),
				ptr_acceptor(new boost::asio::ip::tcp::acceptor(*ptr_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), param.port))),
				ptr_socket(nullptr), ptr_timer(nullptr)
			{
				server_thread = std::thread(&ImageAndContoursStreamServer::server_function, this);
			}


			~ImageAndContoursStreamServer();

			bool storeData(const cv::Mat& image, const mc::structures::StreamData& stream, const std::vector<std::vector<cv::Point>>& contours);

			const ImageAndContoursStreamParameter& getParameter() const;

			ImageAndContoursStreamParameter& getParameter();


		private:

			std::string port;

			ImageAndContoursStreamParameter parameter;

			std::shared_ptr<boost::asio::deadline_timer> ptr_timer;

			std::shared_ptr<boost::asio::io_service> ptr_io_service;
			std::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_acceptor;
			std::shared_ptr<boost::asio::ip::tcp::socket> ptr_socket;

			// so this is basically the send buffer we write out data on ...
			// we can put this into a single object ...
			std::mutex mtx;
			std::vector<uchar> buffer;
			std::vector<uchar> shadow;


			// this is actually just a helper construct for the protocol
			//
			cv::Mat dmyImage;
			std::vector<uchar> dmyImageBuffer;

			std::vector<cv::Point> dmyContour;

			//

			std::thread server_thread;

			void server_function();


			void start_accept();

			void send_stream();

			void send_frame();



			void handle_accept(const boost::system::error_code& error);

			void handle_timer(const boost::system::error_code& error);

			void handle_write(const boost::system::error_code& error, size_t bytes_transferred);


		};

	}
}
