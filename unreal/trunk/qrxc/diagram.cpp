#include "diagram.h"
#include "type.h"
#include "enumType.h"
#include "numericType.h"
#include "stringType.h"
#include "q.h"
#include "edgeType.h"
#include "editor.h"

#include <QDebug>

Diagram::Diagram(QString const &name, QString const &displayedName, Editor *editor)
	: mDiagramName(name), mDiagramDisplayedName(displayedName), mEditor(editor)
{}

Diagram::~Diagram()
{
	foreach(Type *type, mTypes.values())
		if (type)
			delete type;
}

bool Diagram::init(QDomElement const &diagramElement)
{
	for (QDomElement element = diagramElement.firstChildElement(); 
		!element.isNull();
		element = element.nextSiblingElement())
	{
		if (element.nodeName() == "graphicTypes") {
			if (!initGraphicTypes(element))
				return false;
		} else if (element.nodeName() == "nonGraphicTypes") {
			if (!initNonGraphicTypes(element))
				return false;
		} else
			qDebug() << "ERROR: unknown tag" << element.nodeName();
	}
	return true;
}

bool Diagram::initGraphicTypes(QDomElement const &graphicTypesElement)
{
	for (QDomElement element = graphicTypesElement.firstChildElement(); 
		!element.isNull();
		element = element.nextSiblingElement())
	{
		if (element.nodeName() == "node") {
			Type *q = new NodeType(this);
			if (!q->init(element, mDiagramName)) {
				delete q;
				qDebug() << "Can't parse node";
				return false;
			}
			mTypes[q->qualifiedName()] = nodeType;
		} else if (element.nodeName() == "edge") {
			Type *edgeType = new EdgeType(this);
			if (!edgeType->init(element, mDiagramName)) {
				delete edgeType;
				qDebug() << "Can't parse edge";
				return false;
			}
			mTypes[edgeType->qualifiedName()] = edgeType;
		} else if (element.nodeName() == "import") {
			ImportSpecification import = {
				element.attribute("name", ""),
				element.attribute("as", ""),
				element.attribute("displayedName", "")
			};
			mImports.append(import);
		}
		else
		{
			qDebug() << "ERROR: unknown graphic type" << element.nodeName();
			return false;
		}
	}
	return true;
}

bool Diagram::initNonGraphicTypes(QDomElement const &nonGraphicTypesElement)
{
	for (QDomElement element = nonGraphicTypesElement.firstChildElement(); 
		!element.isNull();
		element = element.nextSiblingElement())
	{
		if (element.nodeName() == "enum")
		{
			Type *enumType = new EnumType();
			if (!enumType->init(element, mDiagramName))
			{
				delete enumType;
				qDebug() << "Can't parse enum";
				return false;
			}
			mTypes[enumType->qualifiedName()] = enumType;
		} else if (element.nodeName() == "numeric")
		{
			Type *numericType = new NumericType();
			if (!numericType->init(element, mDiagramName))
			{
				delete numericType;
				qDebug() << "Can't parse numeric type";
				return false;
			}
			mTypes[numericType->qualifiedName()] = numericType;
		} else if (element.nodeName() == "string")
		{
			Type *stringType = new StringType();
			if (!stringType->init(element, mDiagramName))
			{
				delete stringType;
				qDebug() << "Can't parse string type";
				return false;
			}
			mTypes[stringType->qualifiedName()] = stringType;
		}
		else
		{
			qDebug() << "ERROR: unknown non graphic type" << element.nodeName();
			return false;
		}
	}
	return true;
}

bool Diagram::resolve()
{
	foreach (ImportSpecification import, mImports) {
		Type *importedType = mEditor->findType(import.name);
		if (importedType == NULL) {
			qDebug() << "ERROR: imported type" << import.name << "not found, skipping";
			continue;
		}
		Type *copiedType = importedType->clone();
		copiedType->setName(import.as);
		copiedType->setDisplayedName(import.displayedName);
		copiedType->setDiagram(this);
		copiedType->setContext(mDiagramName);
		mTypes.insert(copiedType->qualifiedName(), copiedType);
	}

	foreach(Type *type, mTypes.values())
		if (!type->resolve()) {
			qDebug() << "ERROR: can't resolve type" << type->name();
			return false;
		}

	return true;
}

Editor* Diagram::editor() const
{
	return mEditor;
}

Type* Diagram::findType(QString name)
{
	if (mTypes.contains(name))
		return mTypes[name];
	else
		return mEditor->findType(name);
}

QMap<QString, Type*> Diagram::types() const
{
	return mTypes;
}

QString Diagram::name() const
{
	return mDiagramName;
}

QString Diagram::displayedName() const
{
	return mDiagramDisplayedName;
}
