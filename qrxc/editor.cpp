/* Copyright 2007-2015 QReal Research Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "editor.h"

#include "xmlCompiler.h"
#include "diagram.h"
#include "type.h"
#include "enumType.h"
#include "portType.h"
#include "nameNormalizer.h"
#include <qrutils/outFile.h>

#include <QtCore/QDebug>

Editor::Editor(QDomDocument domDocument, XmlCompiler *xmlCompiler)
	: mXmlCompiler(xmlCompiler), mXmlDomDocument(domDocument), mLoadingComplete(false)
{}

Editor::~Editor()
{
	foreach(Diagram *diagram, mDiagrams.values())
		if (diagram)
			delete diagram;
}

bool Editor::isLoaded()
{
	return mLoadingComplete;
}

bool Editor::load(const QDir &currentDir)
{
	const QDomElement metamodel = mXmlDomDocument.firstChildElement("metamodel");
	if (metamodel.isNull())
	{
		qDebug() << "ERROR: metamodel tag not found";
		return false;
	}

	mVersion = metamodel.attribute("version");

	//Load includes
	for (QDomElement includeElement = metamodel.firstChildElement("include"); !includeElement.isNull();
		includeElement = includeElement.nextSiblingElement("include"))
	{
		QString includeFileName = includeElement.text();
		QFileInfo includeFileInfo(currentDir, includeFileName);
		QDir newDir = currentDir;
		newDir.cd(includeFileInfo.canonicalPath());
		Editor *includeFile = mXmlCompiler->loadXmlFile(newDir, includeFileInfo.fileName());
		if (!includeFile)
		{
			qDebug() << "ERROR: can't include file" << includeFileName;
			return false;
		}
		mIncludes.append(includeFile);
	}

	// Load diagrams part one: don't process inherited properties.
	for (QDomElement diagramElement = metamodel.firstChildElement("diagram"); !diagramElement.isNull();
		diagramElement = diagramElement.nextSiblingElement("diagram"))
	{
		QString diagramName = diagramElement.attribute("name");
		QString nodeName = diagramElement.attribute("nodeName", "");
		QString diagramDisplayedName = diagramElement.attribute("displayedName", diagramName);

		const Diagram *existingDiagram = mXmlCompiler->getDiagram(diagramName);
		if (existingDiagram)
		{
			qDebug() << "ERROR: diagram" << diagramName << "is already loaded";
			return false;
		}
		qDebug() << "parsing diagram" << diagramName;
		Diagram *diagram = new Diagram(diagramName, nodeName, diagramDisplayedName, this);
		if (!diagram->init(diagramElement))
		{
			qDebug() << "ERROR: diagram" << diagramName << "can't be parsed";
			delete diagram;
			return false;
		}
		qDebug() << "diagram" << diagramName << "parsed";
		mDiagrams[diagramName] = diagram;
	}

	// Load diagram part two: resolve all dependencies.
	foreach (Diagram *diagram, mDiagrams.values())
		if (!diagram->resolve())
			return false;

	mLoadingComplete = true;
	return true;
}

XmlCompiler* Editor::xmlCompiler()
{
	return mXmlCompiler;
}

QString Editor::version() const
{
	return mVersion;
}

Type* Editor::findType(const QString &name)
{
	for (Diagram *diagram : mDiagrams.values()) {
		for (Type *type : diagram->types()) {
			if (type->qualifiedName() == name) {
				return type;
			}
		}
	}

	for (Editor *editor : mIncludes) {
		Type *type = editor->findType(name);
		if (type != nullptr && type->qualifiedName() == name) {
			return type;
		}
	}

	return nullptr;
}

Type *Editor::findTypeByNormalizedName(const QString &name)
{
	for (Diagram *diagram : mDiagrams.values()) {
		for (Type *type : diagram->types()) {
			if (NameNormalizer::normalize(type->name()) == name) {
				return type;
			}
		}
	}

	for (Editor *editor : mIncludes) {
		Type *type = editor->findType(name);
		if (type != nullptr && NameNormalizer::normalize(type->name()) == name) {
			return type;
		}
	}

	return nullptr;
}

QSet<EnumType*> Editor::getAllEnumTypes()
{
	EnumType *current = nullptr;
	QSet<EnumType*> result;

	foreach (Diagram *diagram, mDiagrams.values()) {
		foreach (Type *type, diagram->types()) {
			current = dynamic_cast<EnumType*>(type);
			if (current)
				result << current;
		}
	}

	foreach (Editor *editor, mIncludes) {
		result += editor->getAllEnumTypes();
	}

	return result;
}

Diagram* Editor::findDiagram(const QString &name)
{
	if (mDiagrams.contains(name))
		return mDiagrams[name];
	return nullptr;
}

QMap<QString, Diagram*> Editor::diagrams()
{
	return mDiagrams;
}
