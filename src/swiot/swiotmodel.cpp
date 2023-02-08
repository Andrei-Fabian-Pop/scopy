#include<iio.h>
#include "qdebug.h"
#include"swiotmodel.hpp"

using namespace adiscope;

SwiotAdModel::SwiotAdModel(struct iio_channel* iio_chnl):
	m_iio_chnl(iio_chnl)
{
	init();
}

SwiotAdModel::~SwiotAdModel()
{}

void SwiotAdModel::init(){

	if(m_iio_chnl!=nullptr){
		int chnlAttrNumber = iio_channel_get_attrs_count(m_iio_chnl);
		QStringList attrValues;
		for(int i=0;i<chnlAttrNumber;i++){
			QString attrName(iio_channel_get_attr(m_iio_chnl, i));
			attrValues = readChnlAttr(m_iio_chnl, attrName);
			m_chnlAttributes[attrName] = attrValues;
			attrValues.clear();
		}
	}
}

QMap<QString, QStringList> SwiotAdModel::getChnlAttrValues(){
	return m_chnlAttributes;
}

QStringList SwiotAdModel::readChnlAttr(struct iio_channel* iio_chnl, QString attrName){
	QStringList attrValues;
	char* buffer = new char[100];
	std::string s_attrName = attrName.toStdString();

	int returnCode = iio_channel_attr_read(iio_chnl, s_attrName.c_str(), buffer, 99);

	if(returnCode > 0){
		QString bufferValues(buffer);
		attrValues = bufferValues.split(" ");
	}

	delete[] buffer;
	return attrValues;
}

void SwiotAdModel::updateChnlAttributes(QMap<QString,QStringList> newValues, QString attrName){
	QStringList value = newValues.value(attrName);

	if(value.size()==1){
		QString attrVal = value.first();
		std::string s_attrValue = attrVal.toStdString();
		std::string s_attrName = attrName.toStdString();

		if(m_iio_chnl != nullptr){
			int retCode = iio_channel_attr_write(m_iio_chnl,s_attrName.c_str(),s_attrValue.c_str());
			if(retCode==0){
				m_chnlAttributes = newValues;
			}
		}
	}

}

