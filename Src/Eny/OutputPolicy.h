#ifndef eny_outputPolicy_h
#define eny_outputPolicy_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

namespace Output {

/**
 * Contains policy for transformation main value for
 * calculation coordinates.
 * When using UserFirst/SecondAviable if no valid value is 
 * find then zero as value is used.
 * UseSecondAviable will find valud value only on indexis that are greater
 * then first index, if there are no such zero will be used as value.
 */
struct Policy 
{	
public:
	/**
	 * Name of main value.
	 */
	std::string Name;
	/**
	 * Index of first colum that will be used to obtain data.
	 * Or -1 if use zero values.
	 */
	int FirstColumn;
	/**
	 * Index of second colum that will be used to obtain data.
	 * Or -1 if use zero values.
	 */
	int SecondColumn;
	/**
	 * If true instead of FirstColumn is use first valid index.
	 */
	bool UseFirstAviable;
	/**
	 * If true instead of SecondColumn is use first valid index,
	 * that is higher than index for first column
	 */
	bool UseSecondAviable;
public:
	/**
	 * Base ctor, leave name blank, and set that first and second valid columns will be used;
	 */
	Policy()
		: Name(""), FirstColumn(-1), SecondColumn(-1), UseFirstAviable(true), UseSecondAviable(true)
	{ };
};

}

#endif // eny_outputPolicy_h