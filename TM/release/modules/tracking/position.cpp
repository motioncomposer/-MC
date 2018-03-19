
#include "position.h"


namespace mc
{
	namespace position
	{

		// ==========================================================================================================================
		//
		// position estimator parameter
		//
		// ==========================================================================================================================


		void PositionEstimatorParameter::read(const cv::FileNode& fn)
		{
			/*
			fn["offsetFront"] >> offsetFront;
			fn["offsetBack"] >> offsetBack;
			fn["moveThreshold"] >> moveThreshold;
			fn["readyTimeMilli"] >> readyTimeMilli;
			*/
		}


		void PositionEstimatorParameter::write(cv::FileStorage& fs) const
		{
			/*
			fs << "{"
			<< "offsetFront" << offsetFront
			<< "offsetBack" << offsetBack
			<< "moveThreshold" << moveThreshold
			<< "readyTimeMilli" << readyTimeMilli
			<< "}";
			*/
		}


		void read(const cv::FileNode& fn, PositionEstimatorParameter& positionPara, const PositionEstimatorParameter& default)
		{
			if (fn.empty())
				positionPara = default;
			else
				positionPara.read(fn);
		}


		void write(cv::FileStorage& fs, const std::string&, const PositionEstimatorParameter& positionPara)
		{
			positionPara.write(fs);
		}


		bool savePositionEstimatorParameter(const PositionEstimatorParameter& positionPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::WRITE);

			if (fs.isOpened())
			{
				fs << key << positionPara;
				return true;
			}

			return false;
		}


		bool readPositionEstimatorParameter(PositionEstimatorParameter& positionPara,
			const std::string& filename, const std::string& key)
		{
			cv::FileStorage fs(filename, cv::FileStorage::READ);

			if (fs.isOpened())
			{
				fs[key] >> positionPara;
				return true;
			}

			return false;
		}


		// ==========================================================================================================================
		//
		// position estimator object
		//
		// ==========================================================================================================================


		bool PositionEstimator::apply(const cv::Mat& disp, const mc::structures::PlayerActivation activation, const mc::structures::PlayerSelection& selection, const std::vector<std::vector<cv::Point>>& contours,
			mc::structures::SharedData& shared, mc::structures::Result& result)
		{
			if (disp.empty() || disp.type() != CV_16SC1 || contours.empty())
				return false;


			// check if players are swapped
			// this will be more easy if we have the kernel structure ...

			// may be we don't need to swap but simply swap the indices ...
			// or we make them as pointer ...
			// problem is: the kernels are really big ... but they are moved ... so no copying ...
			if (selection[0] == trackerID[1] || selection[1] == trackerID[0])
			{
				std::swap(trackerID[0], trackerID[1]);

				std::swap(metricHeight[0], metricHeight[1]);
				std::swap(metricHeightFilter[0], metricHeightFilter[1]);

				// is there a need to swap these? thex can be derived from the previos one ...
				std::swap(pixelHeight[0], pixelHeight[1]);
				std::swap(topLvl[0], topLvl[1]);

				std::swap(metricCenter[0], metricCenter[1]);
				std::swap(metricCenterFilter[0], metricCenterFilter[1]);

				// is there a need to swap these?
				std::swap(pixelCenter[0], pixelCenter[1]);
				std::swap(midLvl[0], midLvl[1]);
			}

			if (selection[0] >= 0)
			{
				updatePlayerState(disp, selection, shared, contours, 0);
				if (activation[0])
					calculatePositionResult(/*disp, selection, shared, result, 0*/);
				else
					result.player[0].position = mc::structures::PositionResult();
			}
			else
			{
				// don't know if this is neede here ... as we don't have a data member yet ...
				//data[0] = mc::structures::PositionData();
				// here we need to perform a kernel reset ...
				result.player[0].position = mc::structures::PositionResult();
			}


			if (selection[1] >= 0)
			{
				updatePlayerState(disp, selection, shared, contours, 1);
				if (activation[1])
					calculatePositionResult(/*disp, selection, shared, result, 1*/);
				else
					result.player[1].position = mc::structures::PositionResult();
			}
			else
			{
				// don't know if this is neede here ... as we don't have a data member yet ...
				//data[0] = mc::structures::PositionData();
				result.player[1].position = mc::structures::PositionResult();
			}


			writeSharedData(shared, 0);
			writeSharedData(shared, 1);

			return true;
		}


		bool PositionEstimator::draw(cv::Mat& show, const mc::structures::SharedData& shared, const cv::Scalar& color_one, const cv::Scalar& color_two)
		{
			if (show.empty() || show.type() != CV_8UC3)
				return false;

			cv::line(show, { static_cast<int>(shared.trackerData[0].tl.x), static_cast<int>(topLvl[0]) },
			{ static_cast<int>(shared.trackerData[0].br.x), static_cast<int>(topLvl[0]) }, color_one, 2);


			cv::line(show, { static_cast<int>(shared.trackerData[1].tl.x), static_cast<int>(topLvl[1]) },
			{ static_cast<int>(shared.trackerData[1].br.x), static_cast<int>(topLvl[1]) }, color_two, 2);



			// here we need to check if the player is active or selected

			/*
			cv::circle(show, curBodyPartData[0].handLeft, 5, color_one, CV_FILLED);
			cv::circle(show, curBodyPartData[0].handRight, 5, color_one, CV_FILLED);
			cv::circle(show, curBodyPartData[0].footLeft, 5, color_one, CV_FILLED);
			cv::circle(show, curBodyPartData[0].footRight, 5, color_one, CV_FILLED);

			cv::circle(show, curBodyPartData[1].handLeft, 5, color_two, CV_FILLED);
			cv::circle(show, curBodyPartData[1].handRight, 5, color_two, CV_FILLED);
			cv::circle(show, curBodyPartData[1].footLeft, 5, color_two, CV_FILLED);
			cv::circle(show, curBodyPartData[1].footRight, 5, color_two, CV_FILLED);
			*/


			return true;
		}


		const BodyPartData& PositionEstimator::getBodyPartDataIdx0() const
		{
			return curBodyPartData[0];
		}


		const BodyPartData& PositionEstimator::getBodyPartDataIdx1() const
		{
			return curBodyPartData[1];
		}


		void PositionEstimator::smoothContour(const std::vector<cv::Point>& contour)
		{
			smoothedContour.clear();
			smoothedContour.resize(contour.size());

			for (auto&& c = 0; c < smoothedContour.size(); ++c)
			{
				smoothedContour[c] = { 0, 0 };
				for (auto w = -parameter.halfWindowSize; w <= parameter.halfWindowSize; ++w)
					smoothedContour[c] += contour[(smoothedContour.size() + c + w) % smoothedContour.size()];

				smoothedContour[c] /= ((parameter.halfWindowSize << 1) + 1);
			}
		}


		void PositionEstimator::scanForFeet(const mc::structures::SharedData& shared, uint32_t idx)
		{
			float scoreLeft(std::numeric_limits<float>::min());
			float scoreRight(std::numeric_limits<float>::min());

			for (auto&& c = 0; c < smoothedContour.size(); ++c)
			{
				if (smoothedContour[c].y < shared.trackerData[idx].cp.y)
					continue;

				float dmy = std::hypot(smoothedContour[c].x - shared.trackerData[idx].cp.x, smoothedContour[c].y - shared.trackerData[idx].cp.y);

				if (smoothedContour[c].x < shared.trackerData[idx].cp.x)
				{
					if (dmy > scoreLeft)
					{
						scoreLeft = dmy;
						contourIndices[idx].indexLeftFoot = c;
					}
				}
				else if (smoothedContour[c].x > shared.trackerData[idx].cp.x)
				{
					if (dmy > scoreRight)
					{
						scoreRight = dmy;
						contourIndices[idx].indexRightFoot = c;
					}

				}
				else continue;
			}
		}


		void PositionEstimator::scanForLeftHip(const mc::structures::SharedData& shared, uint32_t idx)
		{
			float score(std::numeric_limits<float>::max());

			for (auto c = contourIndices[idx].indexLeftFoot; c >= 0; --c)
			{
				float dmy = std::hypot(smoothedContour[c].x - shared.trackerData[idx].cp.x, smoothedContour[c].y - shared.trackerData[idx].cp.y);

				if (dmy < score)
				{
					score = dmy;
					contourIndices[idx].indexLeftHip = c;
				}

				if (smoothedContour[c].y < shared.trackerData[idx].cp.y)
					break;
			}

		}


		void PositionEstimator::scanForRightHip(const mc::structures::SharedData& shared, uint32_t idx)
		{
			float score(std::numeric_limits<float>::max());

			for (auto c = contourIndices[idx].indexRightFoot; c < smoothedContour.size(); ++c)
			{
				float dmy = std::hypot(smoothedContour[c].x - shared.trackerData[idx].cp.x, smoothedContour[c].y - shared.trackerData[idx].cp.y);

				if (dmy < score)
				{
					score = dmy;
					contourIndices[idx].indexRightHip = c;
				}

				if (smoothedContour[c].y < shared.trackerData[idx].cp.y)
					break;
			}
		}


		void PositionEstimator::scanForLeftHand(const mc::structures::SharedData& shared, uint32_t idx)
		{
			cv::Point anchor(shared.trackerData[idx].cp.x, topLvl[idx]);
			float score(std::numeric_limits<float>::min());

			for (auto c = contourIndices[idx].indexLeftHip; c >= 0; --c)
			{
				if (smoothedContour[c].x > shared.trackerData[idx].cp.x)
					continue;

				float dmy = (smoothedContour[c].y < anchor.y) ?
					std::hypot(smoothedContour[c].x - anchor.x, smoothedContour[c].y - anchor.y) :
					std::abs(smoothedContour[c].x - anchor.x);

				if (dmy > score)
				{
					score = dmy;
					contourIndices[idx].indexLeftHand = c;
				}

			}
		}


		void PositionEstimator::scanForRightHand(const mc::structures::SharedData& shared, uint32_t idx)
		{
			cv::Point anchor(shared.trackerData[idx].cp.x, topLvl[idx]);
			float score(std::numeric_limits<float>::min());

			for (auto c = contourIndices[idx].indexRightHip; c < smoothedContour.size(); ++c)
			{
				if (smoothedContour[c].x < shared.trackerData[idx].cp.x)
					continue;

				float dmy = (smoothedContour[c].y < anchor.y) ?
					std::hypot(smoothedContour[c].x - anchor.x, smoothedContour[c].y - anchor.y) :
					std::abs(smoothedContour[c].x - anchor.x);

				if (dmy > score)
				{
					score = dmy;
					contourIndices[idx].indexRightHand = c;
				}
			}
		}


		void PositionEstimator::findInitialBodyPartData(const mc::structures::SharedData& shared, uint32_t idx)
		{
			scanForFeet(shared, idx);

			scanForLeftHip(shared, idx);
			scanForRightHip(shared, idx);

			scanForLeftHand(shared, idx);
			scanForRightHand(shared, idx);

			curBodyPartData[idx].footLeft = smoothedContour[contourIndices[idx].indexLeftFoot];
			curBodyPartData[idx].footRight = smoothedContour[contourIndices[idx].indexRightFoot];

			curBodyPartData[idx].handLeft = smoothedContour[contourIndices[idx].indexLeftHand];
			curBodyPartData[idx].handRight = smoothedContour[contourIndices[idx].indexRightHand];
		}


		void PositionEstimator::findExtremalPoints(uint32_t idx)
		{
			extremalPoints[idx].mostLeft = extremalPoints[idx].mostRight = extremalPoints[idx].mostUpper = extremalPoints[idx].mostLower = smoothedContour[0];

			for (auto& it : smoothedContour)
			{
				extremalPoints[idx].mostLeft = extremalPoints[idx].mostLeft.x < it.x ? extremalPoints[idx].mostLeft : it;
				extremalPoints[idx].mostRight = extremalPoints[idx].mostRight.x > it.x ? extremalPoints[idx].mostRight : it;
				extremalPoints[idx].mostUpper = extremalPoints[idx].mostUpper.y < it.y ? extremalPoints[idx].mostUpper : it;
				extremalPoints[idx].mostLower = extremalPoints[idx].mostLower.y > it.y ? extremalPoints[idx].mostLower : it;
			}
		}


		void PositionEstimator::findHandRefinementPoints(const mc::structures::SharedData& shared, uint32_t idx)
		{

			float reference = midLvl[idx] - topLvl[idx];

			if (extremalPoints[idx].mostRight.x < shared.trackerData[idx].cp.x && curBodyPartData[idx].handLeft.y > shared.positionData[idx].topLvl && curBodyPartData[idx].handLeft.y < shared.positionData[idx].midLvl)
			{
				float score = std::min({ std::hypot(curBodyPartData[idx].handLeft.x - extremalPoints[idx].mostLeft.x, curBodyPartData[idx].handLeft.y - extremalPoints[idx].mostLeft.y),
					std::hypot(curBodyPartData[idx].handLeft.x - extremalPoints[idx].mostUpper.x, curBodyPartData[idx].handLeft.y - extremalPoints[idx].mostUpper.y),
					std::hypot(curBodyPartData[idx].handLeft.x - extremalPoints[idx].mostLower.x, curBodyPartData[idx].handLeft.y - extremalPoints[idx].mostLower.y) });

				score /= reference;

				if (score < 0.5f && handRefinementPoints[idx].handLeft.x > extremalPoints[idx].mostLeft.x)
					handRefinementPoints[idx].handLeft = extremalPoints[idx].mostLeft;

			}
			else if (extremalPoints[idx].mostLeft.x > shared.trackerData[idx].cp.x && curBodyPartData[idx].handRight.y > shared.positionData[idx].topLvl && curBodyPartData[idx].handRight.y < shared.positionData[idx].midLvl)
			{
				float score = std::min({ std::hypot(curBodyPartData[idx].handRight.x - extremalPoints[idx].mostRight.x, curBodyPartData[idx].handRight.y - extremalPoints[idx].mostRight.y),
					std::hypot(curBodyPartData[idx].handRight.x - extremalPoints[idx].mostUpper.x, curBodyPartData[idx].handRight.y - extremalPoints[idx].mostUpper.y),
					std::hypot(curBodyPartData[idx].handRight.x - extremalPoints[idx].mostLower.x, curBodyPartData[idx].handRight.y - extremalPoints[idx].mostLower.y) });

				score /= reference;

				if (score < 0.5f && handRefinementPoints[idx].handRight.x < extremalPoints[idx].mostRight.x)
					handRefinementPoints[idx].handRight = extremalPoints[idx].mostRight;

			}
		}


		void PositionEstimator::findMinimaPoints(const cv::Mat& disp, uint32_t idx)
		{

			cv::Rect handLeft(cv::Point(curBodyPartData[idx].handLeft.x - parameter.halfRectSize, curBodyPartData[idx].handLeft.y - parameter.halfRectSize),
				cv::Point(curBodyPartData[idx].handLeft.x + parameter.halfRectSize, curBodyPartData[idx].handLeft.y + parameter.halfRectSize));
			
			cv::Rect handRight(cv::Point(curBodyPartData[idx].handRight.x - parameter.halfRectSize, curBodyPartData[idx].handRight.y - parameter.halfRectSize),
				cv::Point(curBodyPartData[idx].handRight.x + parameter.halfRectSize, curBodyPartData[idx].handRight.y + parameter.halfRectSize));

			cv::Rect footLeft(cv::Point(curBodyPartData[idx].footLeft.x - parameter.halfRectSize, curBodyPartData[idx].footLeft.y - parameter.halfRectSize),
				cv::Point(curBodyPartData[idx].footLeft.x + parameter.halfRectSize, curBodyPartData[idx].footLeft.y + parameter.halfRectSize));

			cv::Rect footRight(cv::Point(curBodyPartData[idx].footRight.x - parameter.halfRectSize, curBodyPartData[idx].footRight.y - parameter.halfRectSize),
				cv::Point(curBodyPartData[idx].footRight.x + parameter.halfRectSize, curBodyPartData[idx].footRight.y + parameter.halfRectSize));

			const cv::Rect frame(cv::Point(0, 0), disp.size());

			handLeft &= frame;
			handRight &= frame;
			footLeft &= frame;
			footRight &= frame;

			//cv::minMaxLoc(disp(handLeft), NULL, &minimaPoints[idx].dispHandLeft, NULL, &minimaPoints[idx].handLeft);
			//cv::minMaxLoc(disp(handRight), NULL, &minimaPoints[idx].dispHandRight, NULL, &minimaPoints[idx].handRight);
			//cv::minMaxLoc(disp(footLeft), NULL, &minimaPoints[idx].dispFootLeft, NULL, &minimaPoints[idx].footLeft);
			//cv::minMaxLoc(disp(footRight), NULL, &minimaPoints[idx].dispFootRight, NULL, &minimaPoints[idx].footRight);

			minimaPoints[idx].handLeft += handLeft.tl();
			minimaPoints[idx].handRight += handRight.tl();
			minimaPoints[idx].footLeft += footLeft.tl();
			minimaPoints[idx].footRight += footRight.tl();
		}


		
		void PositionEstimator::combineResults(const mc::structures::SharedData& shared, uint32_t idx)
		{
			float metricFrontHandLeft(std::abs(shared.trackerData[idx].distance - ((values.focalLength * values.baseLineLength * 16.f) / static_cast<float>(minimaPoints[idx].dispHandLeft))));
			float metricFrontHandRight(std::abs(shared.trackerData[idx].distance - ((values.focalLength * values.baseLineLength * 16.f) / static_cast<float>(minimaPoints[idx].dispHandRight))));
			float metricFrontFootLeft(std::abs(shared.trackerData[idx].distance - ((values.focalLength * values.baseLineLength * 16.f) / static_cast<float>(minimaPoints[idx].dispFootLeft))));
			float metricFrontFootRight(std::abs(shared.trackerData[idx].distance - ((values.focalLength * values.baseLineLength * 16.f) / static_cast<float>(minimaPoints[idx].dispFootRight))));

			cv::Vec3f metricHandLeft = fs::core::backProjectionFromDepth(values, curBodyPartData[idx].handLeft, shared.trackerData[idx].distance);
			cv::Vec3f metricHandRight = fs::core::backProjectionFromDepth(values, curBodyPartData[idx].handRight, shared.trackerData[idx].distance);
			cv::Vec3f metricFootLeft = fs::core::backProjectionFromDepth(values, curBodyPartData[idx].footLeft, shared.trackerData[idx].distance);
			cv::Vec3f metricFootRight = fs::core::backProjectionFromDepth(values, curBodyPartData[idx].footRight, shared.trackerData[idx].distance);

			float metricSideHandLeft(std::abs(shared.trackerData[idx].metricCP[0] - metricHandLeft[0]));
			float metricSideHandRight(std::abs(shared.trackerData[idx].metricCP[0] - metricHandRight[0]));
			float metricSideFootLeft(std::abs(shared.trackerData[idx].metricCP[0] - metricFootLeft[0]));
			float metricSideFootRight(std::abs(shared.trackerData[idx].metricCP[0] - metricFootRight[0]));

			curBodyPartData[idx].handLeft = (metricSideHandLeft * curBodyPartData[idx].handLeft + metricFrontHandLeft * minimaPoints[idx].handLeft) / (metricSideHandLeft + metricFrontHandLeft);
			curBodyPartData[idx].handRight = (metricSideHandRight * curBodyPartData[idx].handRight + metricFrontHandRight * minimaPoints[idx].handRight) / (metricSideHandRight + metricFrontHandRight);
			curBodyPartData[idx].footLeft = (metricSideFootLeft * curBodyPartData[idx].footLeft + metricFrontFootLeft * minimaPoints[idx].footLeft) / (metricSideFootLeft + metricFrontFootLeft);
			curBodyPartData[idx].footRight = (metricSideFootRight * curBodyPartData[idx].footRight + metricFrontFootRight * minimaPoints[idx].footRight) / (metricSideFootRight + metricFrontFootRight);

		}


		void PositionEstimator::calculateState(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, const std::vector<std::vector<cv::Point>>& contours, uint32_t idx)
		{

			// this is basically just for drawing ... it should not be used to calculate some of the results ...

			// this here needs to be evaluated in every iteration, because this depends on the actually position in the image
			pixelHeight[idx] = values.focalLength * metricHeight[idx] / shared.trackerData[idx].distance;
			topLvl[idx] = shared.trackerData[idx].br.y - pixelHeight[idx];

			pixelShoulder[idx] = values.focalLength * (metricHeight[idx] - 1.5f * parameter.headSize) / shared.trackerData[idx].distance;
			shoulderLvl[idx] = shared.trackerData[idx].br.y - pixelShoulder[idx];
			
			pixelCenter[idx] = values.focalLength * metricCenter[idx] / shared.trackerData[idx].distance;
			midLvl[idx] = shared.trackerData[idx].br.y - pixelCenter[idx];

			// no only if it is needed for the computation of the data result ...
			// we can also write here the heightLvl stuff ...

			// jupp

			//

			for (auto&& c = 0; c < shared.trackerData[idx].contourIdx.size(); ++c)
			{
				smoothContour(contours[shared.trackerData[idx].contourIdx[c]]);

				if (c == 0)
				{
					findInitialBodyPartData(shared, idx);

					handRefinementPoints[idx].handLeft = curBodyPartData[idx].handLeft;
					handRefinementPoints[idx].handRight = curBodyPartData[idx].handRight;
				}
				else
					findHandRefinementPoints(shared, idx);
			}

			curBodyPartData[idx].handLeft = handRefinementPoints[idx].handLeft;
			curBodyPartData[idx].handRight = handRefinementPoints[idx].handRight;

			// we can do the same thing with the feet --> is is not working ... we need some other technique to prevent the points fromt jumping ...
			// this is somethink we kick out ... it looks like that we need a real tracking system ...
			/*
			if (selection[idx] == trackerID[idx])
			{

				if (std::hypot(curBodyPartData[idx].handLeft.x - oldBodyPartData[idx].handLeft.x, curBodyPartData[idx].handLeft.y - oldBodyPartData[idx].handLeft.y) > 0)
					curBodyPartData[idx].handLeft = oldBodyPartData[idx].handLeft;

				if (std::hypot(curBodyPartData[idx].handRight.x - oldBodyPartData[idx].handRight.x, curBodyPartData[idx].handRight.y - oldBodyPartData[idx].handRight.y) > 0)
					curBodyPartData[idx].handRight = oldBodyPartData[idx].handRight;
			}	
			else
				trackerID[idx] = selection[idx];

			oldBodyPartData[idx].handLeft = curBodyPartData[idx].handLeft;
			oldBodyPartData[idx].handRight = curBodyPartData[idx].handRight;
			*/

			findMinimaPoints(disp, idx);

			combineResults(shared, idx);

		}




		void PositionEstimator::learnInitialValues(const mc::structures::SharedData& shared, uint32_t idx)
		{
			// if we can get the hight above ground function ... this will change ...
			metricHeight[idx] = metricHeightFilter[idx].apply(shared.trackerData[idx].metricBR[1] - shared.trackerData[idx].metricTL[1]);			
			metricShoulder[idx] = metricHeight[idx] - parameter.headSize;
			metricCenter[idx] = metricCenterFilter[idx].apply(shared.trackerData[idx].metricBR[1] - shared.trackerData[idx].metricCP[1]);
		}




		// I don't know if we relly need 2 separated function for this ...
		// that heightLvl == 2 should always be true for the learning case as the heightLvl threshold is following the actual size of the blob ...

		void PositionEstimator::calculateProvisionalState(const cv::Mat& disp, const mc::structures::SharedData& shared,
			const std::vector<std::vector<cv::Point>>& contours, uint32_t idx)
		{

			// here we cannot use the heightLvl threshold ... but we assume that the hands are not above the head ...
			// so we only need to check for the extension sideways ...

			// so hand vertical cannot be more than 0.6 or something like that ...


			// this is when we do not have initial values ...
			//result.player[idx].position.heightLevel = 2;
		}




		void PositionEstimator::calculateCompleteState(const cv::Mat& disp, const mc::structures::SharedData& shared,
			const std::vector<std::vector<cv::Point>>& contours, uint32_t idx)
		{

			// don't now if these values are really needed ...
			// not at the momenent ...

			// actually we are working only on the contours ...
			auto metricTL = fs::core::backProjectionFromDepth(values, shared.trackerData[idx].tl, shared.trackerData[idx].distance);
			auto metricBR = fs::core::backProjectionFromDepth(values, shared.trackerData[idx].br, shared.trackerData[idx].distance);
			auto metricCP = fs::core::backProjectionFromDepth(values, shared.trackerData[idx].cp, shared.trackerData[idx].distance);

			cv::Size2f metricSize{ metricBR[0] - metricTL[0], metricBR[1] - metricTL[1] };


			// actually this stuff is realy noisy ... we said, that we want to find the arm width (left + right) first, and after this we add these two together as the total widhth (metric)
			// we need to provide some max value ... thex are learned ...
			//result.player[idx].position.width = metricSize.width;
			//result.player[idx].position.height = metricSize.height;

			// here we calculate heightLvl ... important ...

			// we have to think about other limits, we should also use floorLvl
			// and we have to put them into the position parameter
			// we wanted to use a schmitt trigger for this to prevent the system from continously switching
			auto heightRation = metricSize.height / metricHeight[idx];

			// I think this only relates to mc::structures::PositionResult ...
			/*
			if (heightRation > 1.f)
				result.player[idx].position.heightLevel = 3;
			else if (heightRation > 0.66f)
				result.player[idx].position.heightLevel = 2;
			else if (heightRation > 0.33f)
				result.player[idx].position.heightLevel = 1;
			else
				result.player[idx].position.heightLevel = 0;
				*/

			/*
			// here we can make two sub functions
			if (shared.trackerData[idx].contourIdx.size() == 1)
			{

				auto pixelHeight = values.focalLength * metricHeight[0] / shared.trackerData[idx].distance;

				cv::Point top(contours[shared.trackerData[idx].contourIdx[0]][0]);

				for (auto& it : contours[shared.trackerData[idx].contourIdx[0]])
				{
					// and we also need to find the point with the biggest score ... accordingly to ... a line we can put into the centroid
				}


				// in this case we only have one contour we have to analize
			}
			else
			{
				// we have more than one contour, which actually means that one of the contours we have added are hand contours ... or some garbage ...
				// first idx is the master blob ...
			}

			*/
		}


		// within this function ...
		void PositionEstimator::updatePlayerState(const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared, const std::vector<std::vector<cv::Point>>& contours, uint32_t idx)
		{
			// here we ctually need to use the selection ...

			if (shared.locationData[idx].present && !shared.locationData[idx].ready)
			{
				learnInitialValues(shared, idx);
				calculateState(disp, selection, shared, contours, idx);
				//calculateProvisionalState(disp, shared, contours, idx);
			}
			else if (shared.locationData[idx].present && shared.locationData[idx].ready)
			{
				calculateState(disp, selection, shared, contours, idx);
				//calculateCompleteState(disp, shared, contours, idx);
			}
			//else
			//	result.player[idx].position = mc::structures::PositionResult();
			// result should not be written here ...
		}





		//////////////////////////////////

		// in here disparity should not be needed ...
		void PositionEstimator::calculatePositionResult(/*const cv::Mat& disp, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
			const std::vector<std::vector<cv::Point>>& contours, mc::structures::Result& result, uint32_t idx*/)
		{
			
		}



		// actually we can directly hand in the image
		// don't know if position result is needed here

		/*
		// here we do the scaling ...
		void PositionEstimator::calculatePositionResult(const cv::Size& imageSize, const mc::structures::PlayerSelection& selection, const mc::structures::SharedData& shared,
			mc::structures::Result& result, uint32_t idx)
		{

		}
		*/

		

		void PositionEstimator::writeSharedData(mc::structures::SharedData& shared, uint32_t idx)
		{
			shared.positionData[idx].metricHeight = metricHeight[idx];
			shared.positionData[idx].pixelHeight = pixelHeight[idx];
			shared.positionData[idx].topLvl = topLvl[idx];

			shared.positionData[idx].metricShoulder = metricShoulder[idx];
			shared.positionData[idx].pixelShoulder = pixelShoulder[idx];
			shared.positionData[idx].shoulderLvl = shoulderLvl[idx];

			shared.positionData[idx].metricCenter = metricCenter[idx];
			shared.positionData[idx].pixelCenter = pixelCenter[idx];
			shared.positionData[idx].midLvl = midLvl[idx];
		}

	}
}