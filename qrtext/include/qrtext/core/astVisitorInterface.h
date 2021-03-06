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

#pragma once

#include <QtCore/QtGlobal>

namespace qrtext {
namespace core {

namespace ast {
class Node;
class Expression;
class BinaryOperator;
class UnaryOperator;
}

/// An interface for every entity that wants to operate with text language AST.
/// See 'visitor' design pattern (http://www.oodesign.com/visitor-pattern.html).
class AstVisitorInterface
{
public:
	virtual ~AstVisitorInterface() {}

	virtual void visit(const ast::Node &node)           { Q_UNUSED(node); }
	virtual void visit(const ast::Expression &node)     { Q_UNUSED(node); }
	virtual void visit(const ast::BinaryOperator &node) { Q_UNUSED(node); }
	virtual void visit(const ast::UnaryOperator &node)  { Q_UNUSED(node); }
};

}
}
