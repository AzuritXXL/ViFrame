#ifndef eny_utilityColorGenerator
#define eny_utilityColorGenerator

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
#include <qcolor.h>

namespace Utility {

class ColorGenerator
{
public:
	enum eType {
		Base,	// Diferent colors
		FluentChange	// Color changes
	};
public:
	/**
	 * @param eType type Generation type.
	 * @param int count Number of colors to generate.
	 */
	ColorGenerator(eType type, int count);
	/**
	 * @param int index Color index.
	 * @return QColor
	 */
	QColor GenerateColor(int index);
protected:
	QColor BaseGenerator(int index);
	QColor FluentGenerator(int index);
private:
	/**
	 * Type of generation.
	 */
	eType mType;
	/**
	 * Number of generated colors.
	 */
	int mCount;
};

}

#endif // eny_utilityColorGenerator