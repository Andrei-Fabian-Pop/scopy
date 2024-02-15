#ifndef DMMREADSTRATEGY_HPP
#define DMMREADSTRATEGY_HPP

#include "iio.h"
#include "ireadstrategy.hpp"
#include "scopy-datamonitor_export.h"

namespace scopy {
namespace datamonitor {

class SCOPY_DATAMONITOR_EXPORT DMMReadStrategy : public IReadStrategy
{
public:
	DMMReadStrategy(iio_device *dev, iio_channel *chn);

	void setUmScale(double scale);

	// IReadStrategy interface
public:
	void read();

	double offset() const;
	void setOffset(double newOffset);

private:
	iio_device *dev;
	iio_channel *chn;
	double testDataTime = 0;
	double m_umScale = 1;
	double m_offset = 0;
};
} // namespace datamonitor
} // namespace scopy
#endif // DMMREADSTRATEGY_HPP
