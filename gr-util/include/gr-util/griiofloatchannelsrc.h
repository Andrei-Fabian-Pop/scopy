#ifndef GRIIOFLOATCHANNELSRC_H
#define GRIIOFLOATCHANNELSRC_H

#include "scopy-gr-util_export.h"
#include "griiodevicesource.h"

namespace scopy::grutil {
class SCOPY_GR_UTIL_EXPORT GRIIOFloatChannelSrc : public GRIIOChannel {
public:
	GRIIOFloatChannelSrc(GRIIODeviceSource* dev,QString channelName,QObject *parent = nullptr);

	void build_blks(GRTopBlock *top);
	void destroy_blks(GRTopBlock *top);

	const iio_data_format *getFmt() const;

protected:
	gr::basic_block_sptr x2f;
private:
	const iio_data_format* fmt;
};
}
#endif // GRIIOFLOATCHANNELSRC_H
