#ifndef kohonen_h
#define kohonen_h

/**
 * Contain template class that implement kohene map algorithm. 
 *
 * @author 
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <vector>
#include <algorithm>
#include <assert.h>

/**
 * Requirments on TParamChanger:
 *	void SetSize(size_t x, size_t y);	// report initial kohonen map size
 *	void LearnStep();					// report single learning step
 *	double LearnPower(double x0, double y0, double x1, double y1);
 *										// calculate learn power, dependently on distance
 * Requirments on TDataType:
 *	behave like std::vector<int>
 * 
 * When network change, power of change is calculated as changerate * distance,
 * which are both obtain from TParamChanger.
 */
template <typename TDataType, typename TParamChanger> class Kohohen
{
public:
	/**
	 * Data are vector of some type.
	 */
	typedef std::vector<TDataType> DataType;
	/**
	 * Pointer to the function which calculate distance between vectors.
	 * Function muset return positive score match. 
	 * More similar data are the higher number should function return.
	 * So values has no upper bound.
	 */
	typedef double (*MatchFunction)(const DataType&, const DataType&);
	/**
	 * Describe single node in network.
	 */
	struct sNode
	{
	public:
		/**
		 * Weights of vector.
		 */
		DataType Weights;
		/**
		 * X position in network.
		 */
		size_t X;
		/**
		 * Y position in network.
		 */
		size_t Y;
		/**
		 * Stored value of node last respone.
		 */
		double LastResponse;
	};
	/**
	 * Node vector.
	 */
	typedef std::vector<sNode> NetworkType;
public:
	Kohohen() : mNetwork(0), mMatchFunction(0)
	{

	}
	~Kohohen()
	{
		Clear();
	}
public:
	/**
	 * @return kohonen's network
	 */
	const NetworkType& GetNetwork() const
	{
		return mNetwork;
	}
	/**
	 * Delete old neural network.
	 */
	void Clear()
	{
		mNetwork.clear();
	}
	/**
	 * Set kohonen's match function.
	 * @param[in] matchFunction Pointer to match function.
	 */
	void SetMatchFunction(MatchFunction matchFunction)
	{
		mMatchFunction = matchFunction;
	}
	/**
	 * Create new neural network. Weight values are set randomly in bounds defined by parameters.
	 * @param[in] sizeX X size of the network.
	 * @param[in] sizeY Y size of the network.
	 * @param[in] randomFunction Functor which set random values in DataType.
	 */
	template <typename RandFnc> void CreateNetwork(size_t sizeX, size_t sizeY, RandFnc randomFunction)
	{
		if (sizeX == 0 || sizeY == 0)
		{
			throw std::exception("sizeX or sizeY is equel to 0");
		}

		// setting size variable
		mSizeX = sizeX;
		mSizeY = sizeY;

		// creating array
		mNetwork.resize(sizeX * sizeY);

		size_t x = 0;
		size_t y = 0;

		// init nodes on random values and set theire position
		std::for_each (mNetwork.begin(), mNetwork.end(),
			[&x, &y, &sizeX, &randomFunction] (sNode& item)
			{
				if (x >= sizeX)
				{
					x = 0;
					y++;
				}
				item.X = x++;
				item.Y = y;
				// assign value 
				randomFunction(item.Weights);
			}
		);

		// set initial learning values
		mParam.SetSize(sizeX, sizeY);
	}
	/**
	 * Learn actual network on vector.
	 * @param[in] sample Vector to learn on.
	 */
	void Learn(const typename DataType& sample)
	{
		// find best match
		typename NetworkType::iterator best = FindBestMatch(sample);
		// apply best match
		ChangeWeights(best, sample);
		// upgrade learning params
		mParam.LearnStep();
	}
	/**
	 * Find best match node with given data.
	 * @param data Data to classify.
	 * @return best match unit
	 */
	typename NetworkType::const_iterator Classify(const DataType& data)
	{
		return FindBestMatch(data);
	}
	/**
	 * Return node on certain position in network. Does not check index boundaries.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @return node on position
	 */
	typename const sNode* GetOn(size_t x, size_t y)
	{	// we check range
		assert(x < mSizeX);
		assert(y < mSizeY);
		// return node
		return &mNetwork[x + (y * mSizeX)];
	}
private:
	/**
	 * X size of kohonens map.
	 */
	int mSizeX;
	/**
	 * Y size of kohonens map.
	 */
	int mSizeY;
	/**
	 * Neural network.
	 */
	NetworkType mNetwork;
	/**
	 * Pointer to function which is used to calculate match.
	 */
	MatchFunction mMatchFunction;
	/**
	 * Holds and transform learning params.
	 */
	typename TParamChanger mParam;
private:
	/**
	 * Find node with highest response to data.
	 * @param[in] data Data for which find BMU.
	 * @return iterator to BMU
	 */
	typename NetworkType::iterator FindBestMatch(const DataType& data)
	{
		// set some start value	
		NetworkType::iterator best = mNetwork.begin();
		double bestMatch = mMatchFunction((*best).Weights, data);

		auto iterEnd = mNetwork.end();
		for (NetworkType::iterator iter = mNetwork.begin() + 1; iter != iterEnd; ++iter)
		{
			(*iter).LastResponse = mMatchFunction((*iter).Weights, data);
			// we use distance .. so we are looking for closest value
			if (bestMatch > (*iter).LastResponse)
			{
				// we have found better one
				bestMatch = (*iter).LastResponse;
				best = iter;
			}
		}
		// return best match
		return best;
	}
	/**
	 * Change weight around center to by more like data, changes rate are obtained from GetChangeRate.
	 * @param[in] centerNeuron Neuron in center of changes (winning neuron).
	 * @param[in] data Data to be become more like.
	 */
	void ChangeWeights(typename const NetworkType::iterator& centerNeuron, const DataType& data)
	{
		// go through all nodes
		std::for_each(mNetwork.begin(), mNetwork.end(),
			[&, centerNeuron, data] (sNode& item)
			{				
				// transform
				double change = mParam.LearnPower(item.X, item.Y, (*centerNeuron).X, (*centerNeuron).Y);

				if (change == 0)
				{	// zero change no need to continue
					
				}
				else
				{
					double originalLeft = 1 - change;
					for (size_t i = 0; i < item.Weights.size(); ++i)
					{
						item.Weights[i] = (originalLeft * item.Weights[i]) + (change * (*centerNeuron).Weights[i]);
					}
				}
			}
		);
	}
};

#endif // kohonen_h