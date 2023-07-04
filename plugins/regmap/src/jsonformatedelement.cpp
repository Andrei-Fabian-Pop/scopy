#include "jsonformatedelement.hpp"

using namespace scopy::regmap;

JsonFormatedElement::JsonFormatedElement(QString fileName, QList<QString> *compatibleDevices, bool isAxiCompatible, bool useRegisterDescriptionAsName, bool useBifieldDescriptionAsName)
    : fileName(fileName),
    compatibleDevices(compatibleDevices),
    isAxiCompatible(isAxiCompatible),
    useRegisterDescriptionAsName(useRegisterDescriptionAsName),
    useBifieldDescriptionAsName(useBifieldDescriptionAsName)
{

}

QString JsonFormatedElement::getFileName() const
{
    return fileName;
}

QList<QString>* JsonFormatedElement::getCompatibleDevices() const
{
    return compatibleDevices;
}

void JsonFormatedElement::addCompatibleDevice(QString device)
{
    compatibleDevices->push_back(device);
}

bool JsonFormatedElement::getIsAxiCompatible() const
{
    return isAxiCompatible;
}

bool JsonFormatedElement::getUseBifieldNameAsDescription() const
{
    return useBifieldDescriptionAsName;
}

QString JsonFormatedElement::toString()
{
    return QString("Filename : " + fileName + " Compatible Dev: " + compatibleDevices->length() +
                   " isAxiCompatible :"  + isAxiCompatible +
                   " useRegisterDescriptionAsName: " + useRegisterDescriptionAsName +
                   " useBifieldDescriptionAsName: " + useBifieldDescriptionAsName);
}

bool JsonFormatedElement::getUseRegisterNameAsDescription() const
{
    return useRegisterDescriptionAsName;
}