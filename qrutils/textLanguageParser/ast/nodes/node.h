#pragma once

#include <QtCore/QList>
#include <QtCore/QSharedPointer>

#include "textLanguageParser/ast/range.h"
#include "textLanguageParser/details/token.h"

#include "qrutils/utilsDeclSpec.h"

namespace textLanguageParser {
namespace ast {

/// Base class for all abstract syntax tree nodes.
class QRUTILS_EXPORT Node {
public:
	virtual ~Node() {}

	Connection const &start() const;
	Connection const &end() const;
	QList<Range> const &ranges() const;

	void connect(details::Token const &token);
	void connect(QSharedPointer<Node> const &node);

	template<typename NodeType>
	bool is() const
	{
		return dynamic_cast<NodeType const * const>(this) != nullptr;
	}

	virtual QList<QSharedPointer<Node>> children() const
	{
		return {};
	}

private:
	/// Connection of a node is a list of ranges of all its subnodes.
	QList<Range> mRanges;

	static Connection const noConnection;
};

}

template<typename TargetType, typename SourceType>
inline QSharedPointer<TargetType> as(QSharedPointer<SourceType> const &node)
{
	return node.template dynamicCast<TargetType>();
}

template<typename TargetType, typename SourceType>
inline QList<QSharedPointer<TargetType>> as(QList<QSharedPointer<SourceType>> const &list)
{
	QList<QSharedPointer<TargetType>> result;
	for (auto item : list) {
		result << as<TargetType>(item);
	}

	return result;
}

inline QSharedPointer<ast::Node> wrap(ast::Node *node)
{
	return QSharedPointer<ast::Node>(node);
}

}
